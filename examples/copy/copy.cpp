#include <iostream>

#include "minerva/minerva.hpp"

using namespace minerva;

int main() {
	//create context
	auto instance = createInstance();
	auto context = createContext(instance);

	//print selected device
	auto device = getDevice(context);
	std::cout << "Selected Device: " << device.name << "\n\n";

	//create memory
	Buffer<uint32_t> buffer1(context, 10), buffer2(context, 10);
	Tensor<uint32_t> tensor(context, 10);

	//write data
	uint32_t i = 1;
	for (auto& v : buffer1.getMemory()) {
		v = i;
		i <<= 1;
	}

	//copy data
	auto copyTo = updateTensor(buffer1, tensor);
	auto copyFrom = retrieveTensor(tensor, buffer2);
	Timeline timeline(context);
	beginSequence(timeline)
		.And(copyTo)
		.Then(copyFrom)
		.Submit();
	
	//wait for roundtrip
	std::cout << "Uploading Data...\n";
	timeline.waitValue(1);
	std::cout << "Fetching Data..\n";
	timeline.waitValue(2);

	//read out data
	for (auto& v : buffer2.getMemory()) {
		std::cout << v << '\n';
	}
}
