add_executable(deviceinfo deviceinfo.cpp)
target_link_libraries(deviceinfo PRIVATE minerva)
set_target_properties(deviceinfo PROPERTIES folder tools)

install(
    TARGETS deviceinfo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT tools
)
