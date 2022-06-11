#include <array>
#include <iostream>

#include "minerva/minerva.hpp"
using namespace minerva;

//You should use something like glm
//Here for simplicity we just use arrays
using Mat = std::array<float, 4>;
using Vec = std::array<float, 2>;

struct UBO {
	Mat m;
	Vec b;
};

constexpr UBO ubo1{ {{1.f, 0.f, 0.f, 1.f}}, {{1.f, -1.f}} };
constexpr UBO ubo2{ {{1.f, 2.f, 2.f, 1.f}}, {{.5f, .5f}} };

int main() {
	//create context
	auto instance = createInstance();
	auto context = createContext(instance);

	//print selected device
	auto device = getDevice(context);
	std::cout << "Selected Device: " << device.name << "\n\n";

	//create memory
	Buffer<Vec> buffer(context, { {1.f, 0.f}, {0.f, 1.f}, {2.f, 0.f}, {0.f, 2.f} });
	Tensor<Vec> tensor(context, buffer.size());
	Uniform<UBO> ubo(context, ubo1);

	//Create program & params
	Program program(context, "uniform.spv", 4u);
	auto param = program.createParameter();
	param.setArgument(tensor, 0);
	param.setArgument(ubo, 1);

	//create commands
	auto update = updateTensor(buffer, tensor);
	auto retrieve = retrieveTensor(tensor, buffer);
	auto run = program.Run({1}, param);

	//create sequence
	Timeline timeline(context);
	beginSequence(timeline)
		.And(update)
		.Then(run)
		.WaitFor(3) //step on cpu
		.And(run)
		.Then(retrieve)
		.Submit();

	//wait for first run to finish
	timeline.waitValue(2);
	//update ubo
	*ubo = ubo2;
	//Notify gpu we're done on our side
	timeline.setValue(3);

	//wait for gpu to finish
	timeline.waitValue(5);

	//print result
	for (auto& v : buffer.getMemory()) {
		std::cout << v[0] << ' ' << v[1] << '\n';
	}
}
