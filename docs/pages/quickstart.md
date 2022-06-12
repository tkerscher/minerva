# Quick Start

We assume you already have a compute shader. In the following is shown a quick
start guide on how to get it running using `minerva`.

We'll be using the compute shader from the add example:

```glsl
#version 460

layout(local_size_x = 1) in;

layout(binding = 0) readonly buffer tensorA { int in_a[]; };
layout(binding = 1) readonly buffer tensorB { int in_b[]; };
layout(binding = 2) writeonly buffer tensorOut { int out_c[]; };

void main() {
    uint idx = gl_GlobalInvocationID.x;
    out_c[idx] = in_a[idx] + in_b[idx];
}
```

## 0. Compiling the Shader

If not already done, the first step should be to compile the shader to SPIR-V.
`minerva` targets Vulkan 1.2 so compiling a compute shader `compute.comp` using
`glslc` might look like the following:

```bash
glslc compute.comp -o compute.spv --target-env=vulkan1.2 -O
```

## 1. Creating a context

To do anything you first need to create a context. A context holds common objects
needed by `minerva` to interact with Vulkan. Context can be created from an
instance so let's start with that:

```cpp
auto instance = minerva::createInstance();
```

One could now get all installed devices on the system that can support `minerva`
which is essentially any graphics cards since 2012. Most of the time either only
one GPU is installed anyway or there's an integrated and a discrete one. To
select the discrete one if there is one and create a context, it's enough to call

```cpp
auto context = minerva::createContext(instance);
```

## 2. Allocate Memory

Next up is to allocate the memory we want to use. In the example code we have
three storage buffer objects. They're counterparts in `minerva` is `Tensor`:

```cpp
minerva::Tensor<uint32_t> tensor1(context, 10);
minerva::Tensor<uint32_t> tensor2(context, 10);
minerva::Tensor<uint32_t> tensor3(context, 10);
```

Since this tensors live on the GPU, we can't directly access their memory.
Instead we need a counterpart on the CPU side, called `Buffer`, which are
generally used to stage and retrieve data from GPU local memory. First, let's
create them:

```cpp
minerva::Buffer<uint32_t> buffer1(context, { 15, 36, 51, 12, 99, 102, 12, 33, 54, 22 });
minerva::Buffer<uint32_t> buffer2(context, { 13, 12, 28, 23, 94, 56, 72, 28, 44, 13 });
minerva::Buffer<uint32_t> buffer3(context, 10);
```

To copy data back and forth from the GPU, we need create commands. Commands hold
instructions for the driver to execute at a latter time, so creating these
commands do not execute them immediately. We see how to do so later on. Right now,
let's create commands for copying the data:

```cpp
auto copy1 = minerva::updateTensor(buffer1, tensor1);
auto copy2 = minerva::updateTensor(buffer2, tensor2);
auto copy3 = minerva::retrieveTensor(tensor3, buffer3);
```

## 3. Create Progam

Next up is to tell `minerva` about the compute shader. These are managed by
`Program`, so let's create one:

```cpp
minerva::Program program(context, "compute.spv");
```

Right know, our program knows nothing about the tensors. This happens during
dispatch, as we will see in a little. In Vulkan parameters are bundled into sets.
By default it's set 0, so we didn't need to specify it in the compute shader.
To pass the tensors to our program, we need to bind them to such a parameter set,
managed by `Paremeter` created by our program:

```cpp
auto param = program.createParameter();
param.setArgument(tensor1, 0);
param.setArgument(tensor2, 1);
param.setArgument(tensor3, 2);
```

We can finally create a command like the ones we created for copying data earlier
for running our shader:

```cpp
auto run = program.Run({ 10 }, param);
```

See the documentation of `Dispatch` for further details.

## 4. Create command sequence

A quirk of Vulkan is that submitted commands are not guaranteed to finish in order
and won't wait for another. This means that without coordination, our copy
commands might already finished before our compute shader actually wrote any data.

`minerva` provides the `Timeline` class, for coordinating and submitting commands, 
so let's start with creating one:

```cpp
minerva::Timeline timeline(context);
```

`Timeline` internally holds a `uint64_t` storing the current step a sequence of
commands is. Commands and the CPU can wait for the `Timeline` to reach a certain
step, while commands and the CPU can raise the value. Note that the value must
always be increasing. Lowering it results in an exception! See the documentation
of `Timeline` for more details.

A simple sequence orchestrated by the newly created timeline can be done using
the `beginSequence` builder function:

```cpp
minerva::beginSequence(timeline)
    .And(copy1)
    .And(copy2)
    .Then(run)
    .Then(copy3)
    .Submit();
```

After submission, the CPU does not wait for the GPU to finish its work. We can
use the `Timeline` to wait for it to finish, by calling `waitValue` with the
final step reached after the last command:

```cpp
timeline.waitValue(3);
```
