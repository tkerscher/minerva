#include <iostream>
#include <fstream>

#include "minerva/minerva.hpp"

//8k resolution
constexpr uint32_t width = 7680;
constexpr uint32_t height = 4320;

using namespace minerva;

int main() {
	//load shader
	auto code = loadShader("mandelbrot.spv");
	if (code.empty()) {
		std::cerr << "Could not load shader code!";
		return -1;
	}

	//create context
	auto instance = createInstance();
	auto context = createContext(instance);

	//print selected device
	auto device = getDevice(context);
	std::cout << "Selected Device: " << device.name << "\n\n";

	//allocate memory
	Buffer<uint8_t> buffer(context, 4 * width * height);
	Image image(context, ImageFormat::R8G8B8A8_UNORM, width, height);

	//create program
	Program program(context, code);

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
	std::ofstream file("mandelbrot.ppm", std::ios::out | std::ios::binary);
	file << "P6\n" << width << '\n' << height << '\n' << 255 << '\n';
	auto data = buffer.getMemory();
	//write image, but skip alpha channel
	auto bytes_written = 0;
	for (auto b : buffer.getMemory()) {
		if (bytes_written % 4 != 3) {
			file << b;
		}
		bytes_written++;
	}
	file.close();
	std::cout << "Done!\n";
}
