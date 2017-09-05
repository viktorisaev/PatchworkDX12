#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"

// Renders Direct3D content on the screen.
namespace PatchworkDx12
{
	class PatchworkDx12Main
	{
	public:
		PatchworkDx12Main();
		void CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void Update(Windows::Foundation::Point _PointerPosition);	// _PointerPosition x=(-1..1), y=(-1..1)
		bool Render();

		void OnWindowSizeChanged();
		void OnSuspending();
		void OnResuming();
		void OnDeviceRemoved();

		void OnMouseMoved(int x, int y);
		void OnMousePressedReleased(bool left, bool right);

	private:
		// TODO: Replace with your own content renderers.
		std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;

		// Rendering loop timer.
		DX::StepTimer m_timer;
	};
}