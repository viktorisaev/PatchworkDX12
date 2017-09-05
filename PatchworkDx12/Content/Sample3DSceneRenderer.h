#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "..\Common\DirectXHelper.h"

namespace PatchworkDx12
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~Sample3DSceneRenderer();
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void Update(DX::StepTimer const& timer, Windows::Foundation::Point _PointerPosition);
		bool Render();
		void SaveState();

		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }

	private:
		void LoadState();
		void Rotate(float _Pitch, float _Roll);

	private:
		// Constant buffers must be 256-byte aligned.
		static const UINT c_alignedConstantBufferSize = (sizeof(ModelViewProjectionConstantBuffer) + 255) & ~255;

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_commandList;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pipelineStateSolid;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pipelineStateWireframe;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_cbvHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_constantBuffer;
		ModelViewProjectionConstantBuffer					m_constantBufferData;
		UINT8*												m_mappedConstantBuffer;
		UINT												m_cbvDescriptorSize;
		D3D12_RECT											m_scissorRect;
		std::vector<byte>									m_vertexShader;
		std::vector<byte>									m_hullShader;
		std::vector<byte>									m_domainShader;
		std::vector<byte>									m_pixelShader;
		D3D12_VERTEX_BUFFER_VIEW							m_vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW								m_indexBufferView;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_TransformsAndColorsDescHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_TransformsBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ColorsBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ControlPointsBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_ControlPointsBufferView;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ControlPointsIndexBuffer;
		D3D12_INDEX_BUFFER_VIEW m_ControlPointsIndexBufferView;


		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_radiansPerMouse;
		bool	m_tracking;

		bool m_IsWireframe;	// true=wireframe piplene state, false=solid pipline state
		int m_tessFactorEdge;	// tesselation factor - edge
		int m_tessFactorInside;	// tesselation factor - inside

		float m_Pitch;
		float m_Roll;


		DirectX::XMMATRIX m_viewproj;

		// imgui
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        g_pd3dSrvDescHeap;
	};




	template<typename T>
	void createSrv(ID3D12Device* device, ID3D12DescriptorHeap* descHeap, int offset, ID3D12Resource* resource, size_t numElements)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_UNKNOWN; // https://msdn.microsoft.com/en-us/library/windows/desktop/dn859358(v=vs.85).aspx#shader_resource_view
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = static_cast<UINT>(numElements);
		srvDesc.Buffer.StructureByteStride = static_cast<UINT>(sizeof(T));
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		static UINT descriptorSize{ device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) };
		D3D12_CPU_DESCRIPTOR_HANDLE d{ descHeap->GetCPUDescriptorHandleForHeapStart() };
		d.ptr += descriptorSize * offset;
		device->CreateShaderResourceView(resource, &srvDesc, d);
	}


	namespace details
	{
		template<typename T>
		Microsoft::WRL::ComPtr<ID3D12Resource> createDefaultBuffer(ID3D12Device* device, const std::vector<T>& data, D3D12_RESOURCE_STATES finalState, std::wstring name = L"")
		{
			UINT elementSize{ static_cast<UINT>(sizeof(T)) };
			UINT bufferSize{ static_cast<UINT>(data.size() * elementSize) };

			D3D12_HEAP_PROPERTIES heapProps;
			ZeroMemory(&heapProps, sizeof(heapProps));
			heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
			heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProps.CreationNodeMask = 1;
			heapProps.VisibleNodeMask = 1;

			D3D12_RESOURCE_DESC resourceDesc;
			ZeroMemory(&resourceDesc, sizeof(resourceDesc));
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resourceDesc.Alignment = 0;
			resourceDesc.Width = bufferSize;
			resourceDesc.Height = 1;
			resourceDesc.DepthOrArraySize = 1;
			resourceDesc.MipLevels = 1;
			resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
			resourceDesc.SampleDesc.Count = 1;
			resourceDesc.SampleDesc.Quality = 0;
			resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;
			DX::ThrowIfFailed(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(defaultBuffer.ReleaseAndGetAddressOf())));

			defaultBuffer->SetName(name.c_str());

			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

			Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
			DX::ThrowIfFailed(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(uploadBuffer.ReleaseAndGetAddressOf())));

			ComPtr<ID3D12CommandAllocator> commandAllocator;
			DX::ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.ReleaseAndGetAddressOf())));

			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
			DX::ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.ReleaseAndGetAddressOf())));

			D3D12_COMMAND_QUEUE_DESC queueDesc;
			ZeroMemory(&queueDesc, sizeof(queueDesc));
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDesc.NodeMask = 0;

			Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
			DX::ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue.ReleaseAndGetAddressOf())));

			void* pData;
			DX::ThrowIfFailed(uploadBuffer->Map(0, NULL, &pData));
			memcpy(pData, data.data(), bufferSize);
			uploadBuffer->Unmap(0, NULL);

			commandList->CopyBufferRegion(defaultBuffer.Get(), 0, uploadBuffer.Get(), 0, bufferSize);

			D3D12_RESOURCE_BARRIER barrierDesc;
			ZeroMemory(&barrierDesc, sizeof(barrierDesc));
			barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrierDesc.Transition.pResource = defaultBuffer.Get();
			barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barrierDesc.Transition.StateAfter = finalState;
			barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			commandList->ResourceBarrier(1, &barrierDesc);

			commandList->Close();
			std::vector<ID3D12CommandList*> ppCommandLists{ commandList.Get() };
			commandQueue->ExecuteCommandLists(static_cast<UINT>(ppCommandLists.size()), ppCommandLists.data());

			UINT64 initialValue{ 0 };
			Microsoft::WRL::ComPtr<ID3D12Fence> fence;
			DX::ThrowIfFailed(device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf())));

			HANDLE fenceEventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (fenceEventHandle == NULL)
			{
				throw("Error creating a fence event.");
			}

			DX::ThrowIfFailed(commandQueue->Signal(fence.Get(), 1));

			DX::ThrowIfFailed(fence->SetEventOnCompletion(1, fenceEventHandle));

			DWORD wait{ WaitForSingleObject(fenceEventHandle, 10000) };
			if (wait != WAIT_OBJECT_0)
			{
				throw("Failed WaitForSingleObject().");
			}

			return defaultBuffer;
		}
	}

	template<typename T>
	Microsoft::WRL::ComPtr<ID3D12Resource> createVertexBuffer(ID3D12Device* device, const std::vector<T>& data, std::wstring name = L"")
	{
		return details::createDefaultBuffer(device, data, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, name);
	}

	template<typename T>
	Microsoft::WRL::ComPtr<ID3D12Resource> createIndexBuffer(ID3D12Device* device, const std::vector<T>& data, std::wstring name = L"")
	{
		return details::createDefaultBuffer(device, data, D3D12_RESOURCE_STATE_INDEX_BUFFER, name);
	}

	template<typename T>
	Microsoft::WRL::ComPtr<ID3D12Resource> createStructuredBuffer(ID3D12Device* device, const std::vector<T>& data, std::wstring name = L"")
	{
		return details::createDefaultBuffer(device, data, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, name);
	}



}

