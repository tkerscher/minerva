add_executable(uniform uniform.cpp)
target_link_libraries(uniform minerva)
set_target_properties(uniform PROPERTIES folder examples)

install(
    TARGETS uniform
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT examples
)
install(
    FILES uniform.spv
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT examples
)
