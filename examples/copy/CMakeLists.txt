add_executable(copy copy.cpp)
target_link_libraries(copy minerva)
set_target_properties(copy PROPERTIES folder examples)

install(
	TARGETS copy
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	COMPONENT examples
)
