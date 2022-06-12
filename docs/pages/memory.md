# Memory

Vulkan provides many ways of allocating and presenting memory in compute shaders.
`minerva` uses a subset of it to make things simpler, that should tackle most needs.
The types of memory supported will be showcased in the following.

## Tensor (Storage Buffer Object)

Probably the most useful way to handle memory are storage buffer objects, which
are represented in `minerva` by `Tensor`. The live purely in the VRAM so they
are on the faster site. More importantly the support both read and write operations
and can handle large sizes of data.

In the shader they can be defined like the following:

```glsl
layout(std430, binding = 0) buffer tensor { float x[]; };
```

See the official wiki for more details: https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object.

## Image

Images are somewhat similar to storage buffer objects in that their size is also
not known to the shader at compile time, but can be queried via `imageSize()`.
It also lives purely in the VRAM. Its most important benefit is the more natural
way to interact with data in two or three dimensions, plus the corresponding
load and store functions can handle out-of-bounds calls, making explicit if
guards redundant.

Images can have different formats, that must be defined in the declaration in the
shader:

```glsl
layout(binding = 0, rgba8) uniform image2D outImage;
```

See the official wiki for more details: https://www.khronos.org/opengl/wiki/Image_Load_Store

## Uniform (Uniform Buffer Object)

Uniform buffer object also live on the GPU (when the allocator feels like it),
but are accessible by the CPU (modern GPUs usually have 256MB special memory for
that). They should be a bit slower than tensors, but make the process of updating
way easier. Also, they can only be read form in the shader.

They are defined in the shader like the following:

```glsl
layout(std140, binding = 1) uniform UBO {
    float m11, m12, m21, m22;
    float b1, b2;
} ubo;
```

And their counterparts on the host side allows direct manipulation:

```cpp
using Mat = std::array<float, 4>;
using Vec = std::array<float, 2>;

struct UBO {
	Mat m;
	Vec b;
};

constexpr UBO ubo1{ {{1.f, 0.f, 0.f, 1.f}}, {{1.f, -1.f}} };
constexpr UBO ubo2{ {{1.f, 2.f, 2.f, 1.f}}, {{.5f, .5f}} };

Uniform<UBO> ubo(context, ubo1);
*ubo = ubo2;
```

## Push Constant

Push Constant are a special type of memory. They live in the command and can be
issued during calls to `Program::Run()`. They are expected to be the fastest
possible, but are limited in size. Vulkan only guarentees 128 bytes(!).

They can be defined in the shader like the folllowing:

```glsl
layout(push_constant) uniform constants {
    vec2 trans;
    float scale;
} push;
```

And on the host side:

```cpp
struct Push {
	float transX;
	float transY;
	float scale;
};

// *** some other stuff ***

auto run = program.Run<Push>({ 16, 16, push }, param);
```

See the documentation of Dispatch for more details.

## Buffer

Buffer is special type of memory, as it lives on the host side and cant be
accessed from the GPU. It's used to populate Tensor and Buffer and retrieve their
data back to CPU.

See the [Quick Start](quickstart.md) or the documentation for more details.
