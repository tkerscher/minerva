add_executable(mandelbrot mandelbrot.cpp)
target_link_libraries(mandelbrot minerva)
set_target_properties(mandelbrot PROPERTIES folder examples)

install(
	TARGETS mandelbrot
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	COMPONENT examples)
install(
	FILES mandelbrot.spv
	DESTINATION ${CMAKE_INSTALL_BINDIR}
	COMPONENT examples)
