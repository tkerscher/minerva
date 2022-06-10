#include <array>
#include <iostream>
#include <string>
#include <unordered_map>

#include "minerva/minerva.hpp"
using namespace minerva;

constexpr auto GroupSizeX = 8;
constexpr auto GroupSizeY = 8;

using Kernel = std::array<float, 9>;

constexpr Kernel Blur = { {
	0.0625f, 0.125f, 0.0625f,
	0.125f, 0.25f, 0.125f,
	0.0625f, 0.125f, 0.0625f
} };
constexpr Kernel Edge = { {
	0.0f, -1.0f, 0.0f,
	-1.0f, 4.0f, -1.0f,
	0.0f, -1.0f, 0.0f
} };
constexpr Kernel Sharper = { {
	-1.0f, -1.0f, -1.0f,
	-1.0f, 9.0f, -1.0f,
	-1.0f, -1.0f, -1.0f
} };

int main(int argc, char* argv[]) {
	//we expect three arguments: kernel name, input, output
	if (argc != 4) {
		std::cerr << "Wrong amount of arguments!\nUsage imageprocessing kernel input output\n";
		return -1;
	}

	//select given kernel
	std::unordered_map<std::string, Kernel> kernels = {
		{"blur", Blur},
		{"edge", Edge},
		{"sharp", Sharper}
	};
	auto it = kernels.find(argv[1]);
	if (it == kernels.end()) {
		std::cerr << "The given kernel is unknown!\n";
		return -2;
	}
	auto& kernel = it->second;

	//create context
	auto instance = createInstance();
	auto context = createContext(instance);

	//print selected device
	auto device = getDevice(context);
	std::cout << "Selected Device: " << device.name << "\n\n";

	//first load image to get dimensions
	std::cout << "Loading image...\n";
	ImageBuffer buffer = ImageBuffer::load(context, argv[2]);
	std::cout << "Image loaded. Size: " << buffer.getWidth() << "x" << buffer.getHeight() << '\n';
	//create images on gpu side
	auto inImage = buffer.createImage(context);
	auto outImage = buffer.createImage(context);

	//create program
	Program convolution(context, "imageprocessing.spv", kernel);

	//create params
	auto param = convolution.createParameter();
	param.setArgument(inImage, 0);
	param.setArgument(outImage, 1);

	//create commands
	auto groupX = (buffer.getWidth() + GroupSizeX - 1) / GroupSizeX;
	auto groupY = (buffer.getHeight() + GroupSizeY - 1) / GroupSizeY;
	auto process = convolution.Run({ groupX, groupY }, param);
	auto upload = updateImage(buffer, inImage);
	auto retrieve = retrieveImage(outImage, buffer);

	//create sequence
	std::cout << "Processing...\n";
	Timeline timeline(context);
	beginSequence(timeline)
		.And(upload)
		.Then(process)
		.Then(retrieve)
		.Submit();
	timeline.waitValue(3);

	//save image
	std::cout << "Saving image...\n";
	buffer.save(argv[3]);
	std::cout << "Done!\n";
}
