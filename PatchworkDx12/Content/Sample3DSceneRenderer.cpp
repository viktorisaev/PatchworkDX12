#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include <ppltasks.h>
#include <synchapi.h>

#include "imgui.h"
#include "imgui_impl_dx12.h"


using namespace PatchworkDx12;

using namespace Concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Storage;

// Indices into the application state map.
//Platform::String^ RollKey = "Roll";
//Platform::String^ PitchKey = "Pitch";
Platform::String^ TrackingKey = "Tracking";

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_radiansPerMouse(XM_PI),	// rotation sensivity depends on mouse movements
	m_tracking(false),
	m_mappedConstantBuffer(nullptr),
	m_deviceResources(deviceResources),
	m_FillMode(2),		// solid+wireframe
	m_tessFactorEdge(4),		// default tess factor - edge
	m_tessFactorInside(4)		// default tess factor - inside
{
	LoadState();
	ZeroMemory(&m_constantBufferData, sizeof(m_constantBufferData));

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

Sample3DSceneRenderer::~Sample3DSceneRenderer()
{
	m_constantBuffer->Unmap(0, nullptr);
	m_mappedConstantBuffer = nullptr;
}



// forward declaration of helper functions

Microsoft::WRL::ComPtr<ID3D12PipelineState> createPipelineState(ID3D12Device* _d3dDevice, D3D12_FILL_MODE fillMode, D3D12_CULL_MODE cullMode, DXGI_FORMAT _RTVFormat, DXGI_FORMAT _DSVFormat, 
	ComPtr<ID3D12RootSignature> _RootSignature, D3D12_SHADER_BYTECODE _VertexShaderBlob, D3D12_SHADER_BYTECODE _HullShaderBlob, D3D12_SHADER_BYTECODE _DomainShaderBlob, D3D12_SHADER_BYTECODE _PixelShaderBlob);

Microsoft::WRL::ComPtr<ID3D12RootSignature> createRootSignature(ID3D12Device* _d3dDevice);

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> createTransformsAndColorsDescHeap(ID3D12Device* _d3dDevice);



void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	ID3D12Device* d3dDevice = m_deviceResources->GetD3DDevice();

	m_rootSignature = createRootSignature(d3dDevice);

	// Load shaders asynchronously.
	auto createVSTask = DX::ReadDataAsync(L"VertexShader.cso").then([this](std::vector<byte>& fileData) {
		m_vertexShader = fileData;
	});

	auto createHSTask = DX::ReadDataAsync(L"HullShader.cso").then([this](std::vector<byte>& fileData) {
		m_hullShader = fileData;
	});

	auto createDSTask = DX::ReadDataAsync(L"DomainShader.cso").then([this](std::vector<byte>& fileData) {
		m_domainShader = fileData;
	});

	auto createPSLightingTask = DX::ReadDataAsync(L"PixelShaderLighting.cso").then([this](std::vector<byte>& fileData) {
		m_pixelLightingShader = fileData;
	});
	auto createPSWhiteTask = DX::ReadDataAsync(L"PixelShaderWhite.cso").then([this](std::vector<byte>& fileData) {
		m_pixelWhiteShader = fileData;
	});

	// Create the pipeline state once the shaders are loaded.
	auto createPipelineStateTask = (createPSLightingTask && createPSWhiteTask && createHSTask && createDSTask && createVSTask).then([this]() {

		m_pipelineStateWireframe = createPipelineState(m_deviceResources->GetD3DDevice(), D3D12_FILL_MODE_WIREFRAME, D3D12_CULL_MODE_NONE, m_deviceResources->GetBackBufferFormat(), m_deviceResources->GetDepthBufferFormat(), m_rootSignature,
			CD3DX12_SHADER_BYTECODE(m_vertexShader.data(), m_vertexShader.size()),
			CD3DX12_SHADER_BYTECODE(m_hullShader.data(), m_hullShader.size()),
			CD3DX12_SHADER_BYTECODE(m_domainShader.data(), m_domainShader.size()),
			CD3DX12_SHADER_BYTECODE(&m_pixelWhiteShader[0], m_pixelWhiteShader.size())
		);

		m_pipelineStateSolid = createPipelineState(m_deviceResources->GetD3DDevice(), D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK, m_deviceResources->GetBackBufferFormat(), m_deviceResources->GetDepthBufferFormat(), m_rootSignature,
			CD3DX12_SHADER_BYTECODE(m_vertexShader.data(), m_vertexShader.size()),
			CD3DX12_SHADER_BYTECODE(m_hullShader.data(), m_hullShader.size()),
			CD3DX12_SHADER_BYTECODE(m_domainShader.data(), m_domainShader.size()),
			CD3DX12_SHADER_BYTECODE(&m_pixelLightingShader[0], m_pixelLightingShader.size())
		);

		// Shader data can be deleted once the pipeline state is created.
		m_vertexShader.clear();
		m_hullShader.clear();
		m_domainShader.clear();
		m_pixelLightingShader.clear();
		m_pixelWhiteShader.clear();
	});


	// Create and upload cube geometry resources to the GPU.
	auto createAssetsTask = createPipelineStateTask.then([this]() {
		auto d3dDevice = m_deviceResources->GetD3DDevice();


		// Create a command list.
		DX::ThrowIfFailed(d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_deviceResources->GetCommandAllocator(), nullptr, IID_PPV_ARGS(&m_commandList)));
        NAME_D3D12_OBJECT(m_commandList);

		// color and transform
		m_TransformsAndColorsDescHeap = createTransformsAndColorsDescHeap(d3dDevice);

		using PointType = decltype(TeapotData::points)::value_type;
		using TransformType = decltype(TeapotData::patchesTransforms)::value_type;
		using ColorType = decltype(TeapotData::patchesColors)::value_type;

		m_ControlPointsBuffer = createVertexBuffer(d3dDevice, TeapotData::points, L"control points");
		m_ControlPointsIndexBuffer = createIndexBuffer(d3dDevice, TeapotData::patches, L"patches");

		m_ControlPointsBufferView.BufferLocation = m_ControlPointsBuffer->GetGPUVirtualAddress();
		m_ControlPointsBufferView.StrideInBytes = static_cast<UINT>(sizeof(PointType));
		m_ControlPointsBufferView.SizeInBytes = static_cast<UINT>(m_ControlPointsBufferView.StrideInBytes * TeapotData::points.size());

		m_ControlPointsIndexBufferView.BufferLocation = m_ControlPointsIndexBuffer->GetGPUVirtualAddress();
		m_ControlPointsIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_ControlPointsIndexBufferView.SizeInBytes = static_cast<UINT>(TeapotData::patches.size() * sizeof(uint32_t));

		m_TransformsBuffer = createStructuredBuffer(d3dDevice, TeapotData::patchesTransforms, L"transforms");
		m_ColorsBuffer = createStructuredBuffer(d3dDevice, TeapotData::patchesColors, L"colors");
		m_WireframeColorsBuffer = createStructuredBuffer(d3dDevice, TeapotData::patchesColorsWireframe, L"wireframecolors");

		// descriptors
		createSrv<TransformType>(d3dDevice, m_TransformsAndColorsDescHeap.Get(), 0, m_TransformsBuffer.Get(), TeapotData::patchesTransforms.size());
		createSrv<ColorType>(d3dDevice, m_TransformsAndColorsDescHeap.Get(), 1, m_ColorsBuffer.Get(), TeapotData::patchesColors.size());

		createSrv<TransformType>(d3dDevice, m_TransformsAndColorsDescHeap.Get(), 2, m_TransformsBuffer.Get(), TeapotData::patchesTransforms.size());
		createSrv<ColorType>(d3dDevice, m_TransformsAndColorsDescHeap.Get(), 3, m_WireframeColorsBuffer.Get(), TeapotData::patchesColorsWireframe.size());


		// Create a descriptor heap for the constant buffers.
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.NumDescriptors = DX::c_frameCount;
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			// This flag indicates that this descriptor heap can be bound to the pipeline and that descriptors contained in it can be referenced by a root table.
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			DX::ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbvHeap)));

            NAME_D3D12_OBJECT(m_cbvHeap);
		}

		CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(DX::c_frameCount * c_alignedConstantBufferSize);
		DX::ThrowIfFailed(d3dDevice->CreateCommittedResource( &uploadHeapProperties , D3D12_HEAP_FLAG_NONE , &constantBufferDesc , D3D12_RESOURCE_STATE_GENERIC_READ , nullptr , IID_PPV_ARGS(&m_constantBuffer)));

        NAME_D3D12_OBJECT(m_constantBuffer);

		// Create constant buffer views to access the upload buffer.
		D3D12_GPU_VIRTUAL_ADDRESS cbvGpuAddress = m_constantBuffer->GetGPUVirtualAddress();
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle(m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
		m_cbvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int n = 0; n < DX::c_frameCount; n++)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			desc.BufferLocation = cbvGpuAddress;
			desc.SizeInBytes = c_alignedConstantBufferSize;
			d3dDevice->CreateConstantBufferView(&desc, cbvCpuHandle);

			cbvGpuAddress += desc.SizeInBytes;
			cbvCpuHandle.Offset(m_cbvDescriptorSize);
		}

		// Map the constant buffers.
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		DX::ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedConstantBuffer)));
		ZeroMemory(m_mappedConstantBuffer, DX::c_frameCount * c_alignedConstantBufferSize);
		// We don't unmap this until the app closes. Keeping things mapped for the lifetime of the resource is okay.


		// Close the command list and execute it to begin the vertex/index buffer copy into the GPU's default heap.
		DX::ThrowIfFailed(m_commandList->Close());


		// imgui
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_deviceResources->GetD3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap));

		ImGui_ImplDX12_Init(NULL, DX::c_frameCount, m_deviceResources->GetD3DDevice(), g_pd3dSrvDescHeap.Get()->GetCPUDescriptorHandleForHeapStart(), g_pd3dSrvDescHeap.Get()->GetGPUDescriptorHandleForHeapStart());

	});

	createAssetsTask.then([this]() {
		m_loadingComplete = true;
	});
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 45.0f * XM_PI / 180.0f;

	D3D12_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
	m_scissorRect = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height)};

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH( fovAngleY , aspectRatio , 1.0f , 100.0f	);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();
	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMMATRIX proj = perspectiveMatrix * orientationMatrix;

//	XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32 eye = { 0.0f, -5.0f, -1.0f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 0.0f, 1.0f, 0.0f };

	m_viewproj = XMMatrixLookAtLH(eye, at, up) * proj;

	XMStoreFloat4x4(&m_constantBufferData.viewproj, XMMatrixTranspose(m_viewproj));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer, Point _PointerPosition)
{
	if (m_loadingComplete)
	{
		if (!m_tracking)
		{
			m_Pitch = _PointerPosition.X * m_radiansPerMouse;
			m_Roll = _PointerPosition.Y * m_radiansPerMouse;

			Rotate(m_Pitch, m_Roll);
		}

		// Update the constant buffer resource.
		UINT8* destination = m_mappedConstantBuffer + (m_deviceResources->GetCurrentFrameIndex() * c_alignedConstantBufferSize);
		memcpy(destination, &m_constantBufferData, sizeof(m_constantBufferData));
	}
}


// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float _Pitch, float _Roll)
{

//	XMMATRIX viewProjMatrixDX = XMLoadFloat4x4(&m_constantBufferData.view);

	XMMATRIX modelMatrixRotationDX = XMMatrixRotationRollPitchYaw(_Roll, 0.0f, _Pitch);
	XMMATRIX modelMatrixTranslationDX = XMMatrixTranslation(-1.5f, 0.0f, -1.5f);
	XMMATRIX modelMatrixDX = modelMatrixTranslationDX * modelMatrixRotationDX;
	XMStoreFloat4x4(&m_constantBufferData.model, modelMatrixDX);
}



// Saves the current state of the renderer.
void Sample3DSceneRenderer::SaveState()
{
	auto state = ApplicationData::Current->LocalSettings->Values;

	//if (state->HasKey(RollKey))
	//{
	//	state->Remove(RollKey);
	//}
	if (state->HasKey(TrackingKey))
	{
		state->Remove(TrackingKey);
	}

//	state->Insert(AngleKey, PropertyValue::CreateSingle(m_angle));
	state->Insert(TrackingKey, PropertyValue::CreateBoolean(m_tracking));
}

// Restores the previous state of the renderer.
void Sample3DSceneRenderer::LoadState()
{
	auto state = ApplicationData::Current->LocalSettings->Values;
	//if (state->HasKey(AngleKey))
	//{
	//	m_angle = safe_cast<IPropertyValue^>(state->Lookup(AngleKey))->GetSingle();
	//	state->Remove(AngleKey);
	//}
	if (state->HasKey(TrackingKey))
	{
		m_tracking = safe_cast<IPropertyValue^>(state->Lookup(TrackingKey))->GetBoolean();
		state->Remove(TrackingKey);
	}
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		//float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		//Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
bool Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return false;
	}

	DX::ThrowIfFailed(m_deviceResources->GetCommandAllocator()->Reset());

	// The command list can be reset anytime after ExecuteCommandList() is called.
	DX::ThrowIfFailed(m_commandList->Reset(m_deviceResources->GetCommandAllocator(), m_pipelineStateSolid.Get() ));		// set pipeline state depends on "wireframe" mode

	PIXBeginEvent(m_commandList.Get(), 0, L"Draw the cube");
	{
		// Set the graphics root signature and descriptor heaps to be used by this frame.
		m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
		//ID3D12DescriptorHeap* ppHeaps[] = { m_cbvHeap.Get() };
		//m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		// Bind the current frame's constant buffer to the pipeline.
//		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_cbvHeap->GetGPUDescriptorHandleForHeapStart(), m_deviceResources->GetCurrentFrameIndex(), m_cbvDescriptorSize);
//		m_commandList->SetGraphicsRootDescriptorTable(0, gpuHandle);

		// set MVP
		m_commandList->SetGraphicsRootConstantBufferView(0, m_constantBuffer->GetGPUVirtualAddress() + m_deviceResources->GetCurrentFrameIndex() * c_alignedConstantBufferSize);

		// set tesselator
		int rootConstants[]{ m_tessFactorEdge, m_tessFactorInside };
		m_commandList->SetGraphicsRoot32BitConstants(1, _countof(rootConstants), rootConstants, 0);

		// descriptor heap
		ID3D12DescriptorHeap* ppTransHeaps[] = { m_TransformsAndColorsDescHeap.Get() };
		m_commandList->SetDescriptorHeaps(1, ppTransHeaps);


		// Set the viewport and scissor rectangle.
		D3D12_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
		m_commandList->RSSetViewports(1, &viewport);
		m_commandList->RSSetScissorRects(1, &m_scissorRect);

		// Indicate this resource will be in use as a render target.
		m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_deviceResources->GetRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Record drawing commands.
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView = m_deviceResources->GetRenderTargetView();
		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = m_deviceResources->GetDepthStencilView();
		m_commandList->ClearRenderTargetView(renderTargetView, DirectX::Colors::CornflowerBlue, 0, nullptr);
		m_commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		m_commandList->OMSetRenderTargets(1, &renderTargetView, false, &depthStencilView);

		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST);	// useprimitive topology as 16-points patch
		std::vector<D3D12_VERTEX_BUFFER_VIEW> myArray { m_ControlPointsBufferView };
		m_commandList->IASetVertexBuffers(0, static_cast<UINT>(myArray.size()), myArray.data());
		m_commandList->IASetIndexBuffer(&m_ControlPointsIndexBufferView);
		uint32_t numIndices = m_ControlPointsIndexBufferView.SizeInBytes / sizeof(uint32_t);

		if (m_FillMode != 1)
		{
			// transforms and solid colors
			D3D12_GPU_DESCRIPTOR_HANDLE d = m_TransformsAndColorsDescHeap->GetGPUDescriptorHandleForHeapStart();
			m_commandList->SetGraphicsRootDescriptorTable(2, d);

			// pipeline = solid (by creation of command list)
			m_commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
		}

		if (m_FillMode != 0)
		{
			// wireframe colors
			D3D12_GPU_DESCRIPTOR_HANDLE wrfrmColorsDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_TransformsAndColorsDescHeap->GetGPUDescriptorHandleForHeapStart(), 2, m_deviceResources->GetD3DDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			m_commandList->SetGraphicsRootDescriptorTable(2, wrfrmColorsDescriptorHandle);

			// draw wireframe on top
			m_commandList->SetPipelineState(m_pipelineStateWireframe.Get());
			m_commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
		}

		// ImGui
		Size outputSize = m_deviceResources->GetOutputSize();
		ImGui_ImplDX12_NewFrame(m_commandList.Get(), int(outputSize.Width), int(outputSize.Height));

		// 1. Show a simple window
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		{
			ImGui::SetNextWindowSize(ImVec2(450, 150), ImGuiSetCond_FirstUseEver);
			ImGui::Begin("London control");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("%i x %i", int(outputSize.Width), int(outputSize.Height));
			ImGui::Text("Pitch, Roll=( %.3f , %.3f )", m_Pitch, m_Roll);
			ImGui::Combo("Fill mode", &m_FillMode, "Solid\0Wireframe\0Solid+Wireframe\0\0" , 3);
			ImGui::SliderInt("Tess Factor Edge", &m_tessFactorEdge, 1, 20);
			ImGui::SliderInt("Tess Factor Inside", &m_tessFactorInside, 1, 20);
			ImGui::End();

			//ImVec4 clear_col = ImColor(114, 144, 154);
			//ImGui::ColorEdit3("clear color", (float*)&clear_col);
		}

		m_commandList.Get()->SetDescriptorHeaps(1, g_pd3dSrvDescHeap.GetAddressOf());
		ImGui::Render();

		// Indicate that the render target will now be used to present when the command list is done executing.
		m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_deviceResources->GetRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	}
	PIXEndEvent(m_commandList.Get());

	DX::ThrowIfFailed(m_commandList->Close());

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	m_deviceResources->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Helpers

Microsoft::WRL::ComPtr<ID3D12PipelineState> createPipelineState(ID3D12Device* _d3dDevice, D3D12_FILL_MODE fillMode, D3D12_CULL_MODE cullMode, DXGI_FORMAT _RTVFormat, DXGI_FORMAT _DSVFormat,
	ComPtr<ID3D12RootSignature> _RootSignature, D3D12_SHADER_BYTECODE _VertexShaderBlob, D3D12_SHADER_BYTECODE _HullShaderBlob, D3D12_SHADER_BYTECODE _DomainShaderBlob, D3D12_SHADER_BYTECODE _PixelShaderBlob
)
{
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.FillMode = fillMode;
	rasterizerDesc.CullMode = cullMode;
	rasterizerDesc.DepthBias = (fillMode == D3D12_FILL_MODE_WIREFRAME) ? -3000 : 0;	// the bias should be quite large due to depth buffer format: https://stackoverflow.com/questions/17199817/directx11-wireframe-z-fighting-help-or-why-d3d11-rasterizer-desc-depthbias-is-a

	D3D12_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0] = {
		FALSE,FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	};

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = (fillMode == D3D12_FILL_MODE_WIREFRAME) ?  D3D12_COMPARISON_FUNC_LESS_EQUAL : D3D12_COMPARISON_FUNC_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	depthStencilDesc.FrontFace = defaultStencilOp;
	depthStencilDesc.BackFace = defaultStencilOp;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
	pipelineStateDesc.InputLayout = { inputElementDescs.data(), static_cast<UINT>(inputElementDescs.size()) };
	pipelineStateDesc.pRootSignature = _RootSignature.Get();
	pipelineStateDesc.VS = _VertexShaderBlob;
	pipelineStateDesc.HS = _HullShaderBlob;
	pipelineStateDesc.DS = _DomainShaderBlob;
	pipelineStateDesc.PS = _PixelShaderBlob;
	pipelineStateDesc.RasterizerState = rasterizerDesc;
	pipelineStateDesc.BlendState = blendDesc;
	pipelineStateDesc.DepthStencilState = depthStencilDesc;
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;	// enable patch topology.
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = _RTVFormat;
	pipelineStateDesc.DSVFormat = _DSVFormat;
	pipelineStateDesc.SampleDesc.Count = 1;

	ComPtr<ID3D12PipelineState> pipelineState;
	DX::ThrowIfFailed(_d3dDevice->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(pipelineState.ReleaseAndGetAddressOf())));

	return pipelineState;
}


// root signature

Microsoft::WRL::ComPtr<ID3D12RootSignature> createRootSignature(ID3D12Device* _d3dDevice)
{
	D3D12_DESCRIPTOR_RANGE dsTransformAndColorSrvRange = {};
	dsTransformAndColorSrvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	dsTransformAndColorSrvRange.NumDescriptors = 2;
	dsTransformAndColorSrvRange.BaseShaderRegister = 0;
	dsTransformAndColorSrvRange.RegisterSpace = 0;
	dsTransformAndColorSrvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER dsTransformAndColorSrv = {};
	dsTransformAndColorSrv.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	dsTransformAndColorSrv.DescriptorTable = { 1, &dsTransformAndColorSrvRange };
	dsTransformAndColorSrv.ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;

	// for domain - register(b0) - model
	D3D12_ROOT_PARAMETER dsObjCb0 = {};
	dsObjCb0.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	dsObjCb0.Descriptor = { 0, 0 };
	dsObjCb0.ShaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;

	// for hull - register(b0)
	D3D12_ROOT_PARAMETER hsTessFactorsCb = {};
	hsTessFactorsCb.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	hsTessFactorsCb.Constants = { 0, 0, 2 };
	hsTessFactorsCb.ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL;

	D3D12_ROOT_PARAMETER rootParameters[]{ dsObjCb0, hsTessFactorsCb, dsTransformAndColorSrv };

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags{
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS
	};

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = static_cast<UINT>(_countof(rootParameters));
	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;
	rootSignatureDesc.Flags = rootSignatureFlags;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	DX::ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.ReleaseAndGetAddressOf(), error.ReleaseAndGetAddressOf()));

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	DX::ThrowIfFailed(_d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignature.ReleaseAndGetAddressOf())));
	NAME_D3D12_OBJECT(rootSignature);

	return rootSignature;
}


Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> createTransformsAndColorsDescHeap(ID3D12Device* _d3dDevice)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	ZeroMemory(&heapDesc, sizeof(heapDesc));
	heapDesc.NumDescriptors = 4;	// (transform + colors) + (transform  + wireframecolors)
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NodeMask = 0;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> transformsAndColorsDescHeap;
	DX::ThrowIfFailed(_d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(transformsAndColorsDescHeap.ReleaseAndGetAddressOf())));
	NAME_D3D12_OBJECT(transformsAndColorsDescHeap);

	return transformsAndColorsDescHeap;
}


