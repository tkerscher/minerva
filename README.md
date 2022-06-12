# Minerva

Minerva is a framework to create GPGPU application using the Vulkan API. \
It tries to provide all necessary boiler plate code to make using the GPU as
simple as possible, albeit it might loose some minor performance in doing so.

Since Minerva uses the Vulkan API it runs on all major OS including Linux,
Windows, Mac, iOS and Android and all major GPU vendors as AMD, Intel, NVIDIA
or Qualcom.

This project was created as part for an assignment for university and should be
treated as a proof of concept.

## Building

The only real dependencies are:
- A C++17 compatible compiler
- CMake at least version 3.14

All other dependencies will be fetched by the cmake script. \
See [Dependencies](#Dependencies) for a full list.

Additionally, if you want to build the debug version, you need the validation
layers installed. The easiest way to to so is to install the [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/).

To make actual use of `minerva`, you need a way to generate SPIR-V code.
For the [examples](examples), [glslc](https://github.com/google/shaderc) was
used, which is part of the Vulkan SDK.

## Dependencies

Following is the full list of dependencies:
- [stb](https://github.com/nothings/stb)
- [SPIR-V Reflect](https://github.com/KhronosGroup/SPIRV-Reflect)
- [volk](https://github.com/zeux/volk)
- [Vulkan Headers](https://github.com/KhronosGroup/Vulkan-Headers)
- [vulkan memory allocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)

Additionally, if you want to build the documentation you need:
- Doxygen
- Dots
- [Doxygen Awesome CSS](https://github.com/jothepro/doxygen-awesome-css)
