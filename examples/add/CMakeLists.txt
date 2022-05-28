add_executable(add add.cpp)
target_link_libraries(add minerva)
set_target_properties(add PROPERTIES folder examples)

install(
	TARGETS add
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	COMPONENT examples
)
install(
	FILES add.spv
	DESTINATION ${CMAKE_INSTALL_BINDIR}
	COMPONENT examples
)
