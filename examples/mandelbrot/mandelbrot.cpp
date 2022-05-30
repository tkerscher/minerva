#include <iostream>
#include <fstream>

#include "minerva/minerva.hpp"

//8k resolution
constexpr uint32_t width = 7680;
constexpr uint32_t height = 4320;

using namespace minerva;

int main() {
	//create context
	auto instance = createInstance();
	auto context = createContext(instance);

	//print selected device
	auto device = getDevice(context);
	std::cout << "Selected Device: " << device.name << "\n\n";

	//allocate memory
	Image image(context, ImageFormat::R8G8B8A8_UNORM, width, height);
	ImageBuffer buffer(context, width, height);

	//create program
	Program program(context, "mandelbrot.spv");

	//create param
	auto param = program.createParameter();
	param.setArgument(image);

	//create commands
	auto run = program.Run(width / 4, height / 4, 1, param);
	auto ret = retrieveImage(image, buffer);

	//create sequence
	std::cout << "Rendering...\n";
	Timeline timeline(context);
	beginSequence(timeline)
		.And(run)
		.Then(ret)
		.Submit();
	timeline.waitValue(2);

	//write image in ppm format
	std::cout << "Saving...\n";
	buffer.save("mandelbrot.png");
	std::cout << "Done!\n";
}
