#include <iostream>
#include <fstream>

#include "minerva/minerva.hpp"

//8k resolution
constexpr uint32_t width = 7680;
constexpr uint32_t height = 4320;

struct Push {
	float transX;
	float transY;
	float scale;
};

using namespace minerva;

int main(int argc, char* argv[]) {
	//Either use default params or parse args
	Push push{ 0.f, 0.f, 1.f };
	if (argc == 4) {
		push.transX = std::atof(argv[1]);
		push.transY = std::atof(argv[2]);
		push.scale = std::atof(argv[3]);
	}

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
	auto run = program.Run<Push>({ width / 4, height / 4, push }, param);
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
