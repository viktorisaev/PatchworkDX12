#include "pch.h"

// https://www.sjbaker.org/wiki/index.php?title=The_History_of_The_Teapot
// http://www.gamasutra.com/view/feature/131755/curved_surfaces_using_bzier_.php?print=1

#define SINGLE_16PATCH	// single 16-points patch


std::vector<DirectX::XMFLOAT3> TeapotData::points
{

#if defined(SINGLE_16PATCH)
	// single 16-points patch
	{0.0f, 2.0f, 0.0f},
	{1.0f, 0.0f, 0.0f},
	{2.0f, 0.0f, 0.0f},
	{3.0f, -1.0f, 0.0f},
	{0.0f, 0.0f, 1.0f},
	{1.0f, 0.0f, 1.0f},
	{2.0f, 0.0f, 1.0f},
	{3.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 2.0f},
	{1.0f, 0.0f, 2.0f},
	{2.0f, 0.0f, 2.0f},
	{3.0f, 0.0f, 2.0f},
	{0.0f, -1.0f, 3.0f},
	{1.0f, 0.0f, 3.0f},
	{2.0f, 0.0f, 3.0f},
	{3.0f, 2.0f, 3.0f},

#else
	// 16 points
	{0.2000f, 0.0000f, 2.70000f},
	{0.2000f, -0.1120f, 2.70000f},
	{0.1120f, -0.2000f, 2.70000f},
	{0.0000f, -0.2000f, 2.70000f},
	{1.3375f, 0.0000f, 2.53125f},
	{1.3375f, -0.7490f, 2.53125f},
	{0.7490f, -1.3375f, 2.53125f},
	{0.0000f, -1.3375f, 2.53125f},
	{1.4375f, 0.0000f, 2.53125f},
	{1.4375f, -0.8050f, 2.53125f},
	{0.8050f, -1.4375f, 2.53125f},
	{0.0000f, -1.4375f, 2.53125f},
	{1.5000f, 0.0000f, 2.40000f},
	{1.5000f, -0.8400f, 2.40000f},
	{0.8400f, -1.5000f, 2.40000f},
	{0.0000f, -1.5000f, 2.40000f},

	{1.7500f, 0.0000f, 1.87500f},
	{1.7500f, -0.9800f, 1.87500f},
	{0.9800f, -1.7500f, 1.87500f},
	{0.0000f, -1.7500f, 1.87500f},
	{2.0000f, 0.0000f, 1.35000f},
	{2.0000f, -1.1200f, 1.35000f},
	{1.1200f, -2.0000f, 1.35000f},
	{0.0000f, -2.0000f, 1.35000f},
	{2.0000f, 0.0000f, 0.90000f},
	{2.0000f, -1.1200f, 0.90000f},
	{1.1200f, -2.0000f, 0.90000f},
	{0.0000f, -2.0000f, 0.90000f},
	{-2.0000f, 0.0000f, 0.90000f},
	{2.0000f, 0.0000f, 0.45000f},
	{2.0000f, -1.1200f, 0.45000f},
	{1.1200f, -2.0000f, 0.45000f},
	{0.0000f, -2.0000f, 0.45000f},
	{1.5000f, 0.0000f, 0.22500f},
	{1.5000f, -0.8400f, 0.22500f},
	{0.8400f, -1.5000f, 0.22500f},
	{0.0000f, -1.5000f, 0.22500f},
	{1.5000f, 0.0000f, 0.15000f},
	{1.5000f, -0.8400f, 0.15000f},
	{0.8400f, -1.5000f, 0.15000f},
	{0.0000f, -1.5000f, 0.15000f},
	{-1.6000f, 0.0000f, 2.02500f},
	{-1.6000f, -0.3000f, 2.02500f},
	{-1.5000f, -0.3000f, 2.25000f},
	{-1.5000f, 0.0000f, 2.25000f},
	{-2.3000f, 0.0000f, 2.02500f},
	{-2.3000f, -0.3000f, 2.02500f},
	{-2.5000f, -0.3000f, 2.25000f},
	{-2.5000f, 0.0000f, 2.25000f},
	{-2.7000f, 0.0000f, 2.02500f},
	{-2.7000f, -0.3000f, 2.02500f},
	{-3.0000f, -0.3000f, 2.25000f},
	{-3.0000f, 0.0000f, 2.25000f},
	{-2.7000f, 0.0000f, 1.80000f},
	{-2.7000f, -0.3000f, 1.80000f},
	{-3.0000f, -0.3000f, 1.80000f},
	{-3.0000f, 0.0000f, 1.80000f},
	{-2.7000f, 0.0000f, 1.57500f},
	{-2.7000f, -0.3000f, 1.57500f},
	{-3.0000f, -0.3000f, 1.35000f},
	{-3.0000f, 0.0000f, 1.35000f},
	{-2.5000f, 0.0000f, 1.12500f},
	{-2.5000f, -0.3000f, 1.12500f},
	{-2.6500f, -0.3000f, 0.93750f},
	{-2.6500f, 0.0000f, 0.93750f},
	{-2.0000f, -0.3000f, 0.90000f},
	{-1.9000f, -0.3000f, 0.60000f},
	{-1.9000f, 0.0000f, 0.60000f},
	{1.7000f, 0.0000f, 1.42500f},
	{1.7000f, -0.6600f, 1.42500f},
	{1.7000f, -0.6600f, 0.60000f},
	{1.7000f, 0.0000f, 0.60000f},
	{2.6000f, 0.0000f, 1.42500f},
	{2.6000f, -0.6600f, 1.42500f},
	{3.1000f, -0.6600f, 0.82500f},
	{3.1000f, 0.0000f, 0.82500f},
	{2.3000f, 0.0000f, 2.10000f},
	{2.3000f, -0.2500f, 2.10000f},
	{2.4000f, -0.2500f, 2.02500f},
	{2.4000f, 0.0000f, 2.02500f},
	{2.7000f, 0.0000f, 2.40000f},
	{2.7000f, -0.2500f, 2.40000f},
	{3.3000f, -0.2500f, 2.40000f},
	{3.3000f, 0.0000f, 2.40000f},
	{2.8000f, 0.0000f, 2.47500f},
	{2.8000f, -0.2500f, 2.47500f},
	{3.5250f, -0.2500f, 2.49375f},
	{3.5250f, 0.0000f, 2.49375f},
	{2.9000f, 0.0000f, 2.47500f},
	{2.9000f, -0.1500f, 2.47500f},
	{3.4500f, -0.1500f, 2.51250f},
	{3.4500f, 0.0000f, 2.51250f},
	{2.8000f, 0.0000f, 2.40000f},
	{2.8000f, -0.1500f, 2.40000f},
	{3.2000f, -0.1500f, 2.40000f},
	{3.2000f, 0.0000f, 2.40000f},
	{0.0000f, 0.0000f, 3.15000f},
	{0.8000f, 0.0000f, 3.15000f},
	{0.8000f, -0.4500f, 3.15000f},
	{0.4500f, -0.8000f, 3.15000f},
	{0.0000f, -0.8000f, 3.15000f},
	{0.0000f, 0.0000f, 2.85000f},
	{1.4000f, 0.0000f, 2.40000f},
	{1.4000f, -0.7840f, 2.40000f},
	{0.7840f, -1.4000f, 2.40000f},
	{0.0000f, -1.4000f, 2.40000f},
	{0.4000f, 0.0000f, 2.55000f},
	{0.4000f, -0.2240f, 2.55000f},
	{0.2240f, -0.4000f, 2.55000f},
	{0.0000f, -0.4000f, 2.55000f},
	{1.3000f, 0.0000f, 2.55000f},
	{1.3000f, -0.7280f, 2.55000f},
	{0.7280f, -1.3000f, 2.55000f},
	{0.0000f, -1.3000f, 2.55000f},
	{1.3000f, 0.0000f, 2.40000f},
	{1.3000f, -0.7280f, 2.40000f},
	{0.7280f, -1.3000f, 2.40000f},
	{0.0000f, -1.3000f, 2.40000f}
#endif
};

std::vector<uint32_t> TeapotData::patches
{
#if defined(SINGLE_16PATCH)
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15	// single patch
#else
	// rim
	102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	// body 1
	12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
	12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
	12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
	12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
	// body 2
	24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	// lid 1
	96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,
	96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,
	96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,
	96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,
	// lid 2
	0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117,
	0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117,
	0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117,
	0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117,
	// handle 1
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
	// handle 2
	53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 28, 65, 66, 67,
	53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 28, 65, 66, 67,
	// spout 1
	68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
	68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
	// spout 2
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95
#endif
};

std::vector<DirectX::XMFLOAT4X4> TeapotData::patchesTransforms
{
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(0.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f),

	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(0.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f),

	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(0.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f),

	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(0.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f),

	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(0.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(90.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f),
	getRotationMatrix(0.0f, DirectX::XMConvertToRadians(270.0f), 0.0f),

	getScalingMatrix(1.0f, 1.0f, 1.0f),
	getScalingMatrix(1.0f, -1.0f, 1.0f),

	getScalingMatrix(1.0f, 1.0f, 1.0f),
	getScalingMatrix(1.0f, -1.0f, 1.0f),

	getScalingMatrix(1.0f, 1.0f, 1.0f),
	getScalingMatrix(1.0f, -1.0f, 1.0f),

	getScalingMatrix(1.0f, 1.0f, 1.0f),
	getScalingMatrix(1.0f, -1.0f, 1.0f)
};

std::vector<DirectX::XMFLOAT3> TeapotData::patchesColors
{
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX },
	{ static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX, static_cast<float>(std::rand()) / RAND_MAX }
};

std::vector<DirectX::XMFLOAT3> TeapotData::patchesColorsWireframe
{
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) },
	{ DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) }
};