add_executable(imageprocessing imageprocessing.cpp)
target_link_libraries(imageprocessing minerva)
set_target_properties(imageprocessing PROPERTIES folder examples)

install(
    TARGETS imageprocessing
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT examples
)
install(
    FILES imageprocessing.spv leaf.jpg
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT examples
)
