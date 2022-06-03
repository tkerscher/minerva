#include <iostream>
#include <fstream>
#include <vector>

#include "minerva/minerva.hpp"

using namespace minerva;

int main() {
	//create context
	auto instance = createInstance();
	auto context = createContext(instance);

	//print selected device
	auto device = getDevice(context);
	std::cout << "Selected Device: " << device.name << "\n\n";

	//create cpu memory
	Buffer<uint32_t> buffer1(context, { 15, 36, 51, 12, 99, 102, 12, 33, 54, 22 });
	Buffer<uint32_t> buffer2(context, { 13, 12, 28, 23, 94, 56, 72, 28, 44, 13 });
	Buffer<uint32_t> buffer3(context, 10);
	//create gpu memory
	Tensor<uint32_t> tensor1(context, 10);
	Tensor<uint32_t> tensor2(context, 10);
	Tensor<uint32_t> tensor3(context, 10);

	//create program
	Program program(context, "add.spv");

	//create params
	auto param = program.createParameter();
	//fill params
	param.setArgument(tensor1, 0);
	param.setArgument(tensor2, 1);
	param.setArgument(tensor3, 2);

	//create commands
	auto copy1 = updateTensor(buffer1, tensor1);
	auto copy2 = updateTensor(buffer2, tensor2);
	auto copy3 = retrieveTensor(tensor3, buffer3);
	auto run = program.Run({ 10 }, param);

	//create sequence
	Timeline timeline(context);
	beginSequence(timeline)
		.And(copy1)
		.And(copy2)
		.Then(run)
		.Then(copy3)
		.Submit();
	timeline.waitValue(3);

	//read out
	for (auto v : buffer3.getMemory()) {
		std::cout << v << '\n';
	}
}
