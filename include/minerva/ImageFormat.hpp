#pragma once

namespace minerva {
	enum class ImageFormat {
		R8G8B8A8_UNORM = 37,
		R8G8B8A8_SNORM = 38,
		R8G8B8A8_UINT = 41,
		R8G8B8A8_SINT = 42,
		R16G16B16A16_UINT = 95,
		R16G16B16A16_SINT = 96,
		//R16G16B16A16_SFLOAT,
		R32_UINT = 98,
		R32_SINT = 99,
		R32_SFLOAT = 100,
		R32G32_UINT = 101,
		R32G32_SINT = 102,
		R32G32_SFLOAT = 103,
		R32G32B32A32_UINT = 107,
		R32G32B32A32_SINT = 108,
		R32G32B32A32_SFLOAT = 109,

		//Extended Formats

		R8_UNORM = 9,
		R8_SNORM = 10,
		R8_UINT = 13,
		R8_SINT = 14,
		R8G8_UNORM = 16,
		R8G8_SNORM = 17,
		R8G8_UINT = 20,
		R8G8_SINT = 21,
		R16_UNORM = 70,
		R16_SNORM = 71,
		R16_UINT = 74,
		R16_SINT = 75,
		//R16_SFLOAT,
		R16G16_UNORM = 77,
		R16G16_SNORM = 78,
		R16G16_UINT = 81,
		R16G16_SINT = 82,
		//R16G16_SFLOAT,
		R16G16B16A16_UNORM = 91,
		R16G16B16A16_SNORM = 92
	};

	template<class T>
	struct Vec2 {
		union {
			struct { T x, y; };
			struct { T r, g; };
		};
	};

	template<class T>
	struct Vec4 {
		union {
			struct { T x, y, z, w; };
			struct { T r, g, b, a; };
		};
	};

	template<ImageFormat IF> struct ImageElementType {};

	template<> struct ImageElementType<ImageFormat::R8_UNORM> { using ElementType = uint8_t; };
	template<> struct ImageElementType<ImageFormat::R8_SNORM> { using ElementType = int8_t; };
	template<> struct ImageElementType<ImageFormat::R8_UINT> { using ElementType = uint8_t; };
	template<> struct ImageElementType<ImageFormat::R8_SINT> { using ElementType = int8_t; };

	template<> struct ImageElementType<ImageFormat::R8G8_UNORM> { using ElementType = Vec2<uint8_t>; };
	template<> struct ImageElementType<ImageFormat::R8G8_SNORM> { using ElementType = Vec2<int8_t>; };
	template<> struct ImageElementType<ImageFormat::R8G8_UINT> { using ElementType = Vec2<uint8_t>; };
	template<> struct ImageElementType<ImageFormat::R8G8_SINT> { using ElementType = Vec2<int8_t>; };

	template<> struct ImageElementType<ImageFormat::R8G8B8A8_UNORM> { using ElementType = Vec4<uint8_t>; };
	template<> struct ImageElementType<ImageFormat::R8G8B8A8_SNORM> { using ElementType = Vec4<int8_t>; };
	template<> struct ImageElementType<ImageFormat::R8G8B8A8_UINT> { using ElementType = Vec4<uint8_t>; };
	template<> struct ImageElementType<ImageFormat::R8G8B8A8_SINT> { using ElementType = Vec4<int8_t>; };

	template<> struct ImageElementType<ImageFormat::R16_UNORM> { using ElementType = uint16_t; };
	template<> struct ImageElementType<ImageFormat::R16_SNORM> { using ElementType = int16_t; };
	template<> struct ImageElementType<ImageFormat::R16_UINT> { using ElementType = uint16_t; };
	template<> struct ImageElementType<ImageFormat::R16_SINT> { using ElementType = int16_t; };

	template<> struct ImageElementType<ImageFormat::R16G16_UNORM> { using ElementType = Vec2<uint16_t>; };
	template<> struct ImageElementType<ImageFormat::R16G16_SNORM> { using ElementType = Vec2<int16_t>; };
	template<> struct ImageElementType<ImageFormat::R16G16_UINT> { using ElementType = Vec2<uint16_t>; };
	template<> struct ImageElementType<ImageFormat::R16G16_SINT> { using ElementType = Vec2<int16_t>; };

	template<> struct ImageElementType<ImageFormat::R16G16B16A16_UNORM> { using ElementType = Vec4<uint16_t>; };
	template<> struct ImageElementType<ImageFormat::R16G16B16A16_SNORM> { using ElementType = Vec4<int16_t>; };
	template<> struct ImageElementType<ImageFormat::R16G16B16A16_UINT> { using ElementType = Vec4<uint16_t>; };
	template<> struct ImageElementType<ImageFormat::R16G16B16A16_SINT> { using ElementType = Vec4<int16_t>; };

	template<> struct ImageElementType<ImageFormat::R32_UINT> { using ElementType = uint32_t; };
	template<> struct ImageElementType<ImageFormat::R32_SINT> { using ElementType = int32_t; };
	template<> struct ImageElementType<ImageFormat::R32_SFLOAT> { using ElementType = float; };

	template<> struct ImageElementType<ImageFormat::R32G32_UINT> { using ElementType = Vec2<uint32_t>; };
	template<> struct ImageElementType<ImageFormat::R32G32_SINT> { using ElementType = Vec2<int32_t>; };
	template<> struct ImageElementType<ImageFormat::R32G32_SFLOAT> { using ElementType = Vec2<float>; };

	template<> struct ImageElementType<ImageFormat::R32G32B32A32_UINT> { using ElementType = Vec4<uint32_t>; };
	template<> struct ImageElementType<ImageFormat::R32G32B32A32_SINT> { using ElementType = Vec4<int32_t>; };
	template<> struct ImageElementType<ImageFormat::R32G32B32A32_SFLOAT> { using ElementType = Vec4<float>; };

	template<ImageFormat IF>
	using ImageElementTypeValue = typename ImageElementType<IF>::ElementType;

	template<ImageFormat IF>
	struct IsExtendedFormat {
		static constexpr bool Value = false;
	};

	template<> struct IsExtendedFormat<ImageFormat::R8_UNORM> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R8_SNORM> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R8_UINT> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R8_SINT> { static constexpr bool Value = true; };

	template<> struct IsExtendedFormat<ImageFormat::R8G8_UNORM> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R8G8_SNORM> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R8G8_UINT> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R8G8_SINT> { static constexpr bool Value = true; };

	template<> struct IsExtendedFormat<ImageFormat::R16_UNORM> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R16_SNORM> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R16_UINT> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R16_SINT> { static constexpr bool Value = true; };

	template<> struct IsExtendedFormat<ImageFormat::R16G16_UNORM> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R16G16_SNORM> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R16G16_UINT> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R16G16_SINT> { static constexpr bool Value = true; };

	template<> struct IsExtendedFormat<ImageFormat::R16G16B16A16_UNORM> { static constexpr bool Value = true; };
	template<> struct IsExtendedFormat<ImageFormat::R16G16B16A16_SNORM> { static constexpr bool Value = true; };

	template<ImageFormat IF>
	constexpr bool IsExtendedFormatValue = IsExtendedFormat<IF>::Value;
}
