#To shorten things
set(INCROOT ${PROJECT_SOURCE_DIR}/include/minerva)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src)

#Public API
set(PUBLIC_HEADERS
    ${INCROOT}/Buffer.hpp
    ${INCROOT}/Copy.hpp
    ${INCROOT}/Device.hpp
    ${INCROOT}/Instance.hpp
    ${INCROOT}/Sequence.hpp
    ${INCROOT}/Tensor.hpp
    ${INCROOT}/Timeline.hpp
    ${INCROOT}/config.hpp
    ${INCROOT}/minerva.hpp
    ${INCROOT}/version.hpp
)

#Source
set(SRC
    ${SRCROOT}/Buffer.cpp
    ${SRCROOT}/Copy.cpp
    ${SRCROOT}/Device.cpp
    ${SRCROOT}/Instance.cpp
    ${SRCROOT}/Sequence.cpp
    ${SRCROOT}/Tensor.cpp
    ${SRCROOT}/Timeline.cpp
    ${SRCROOT}/array.hpp
    ${SRCROOT}/version.cpp
#vulkan
    ${SRCROOT}/vk/Buffer.hpp
    ${SRCROOT}/vk/Buffer.cpp
    ${SRCROOT}/vk/Context.hpp
    ${SRCROOT}/vk/Context.cpp
    ${SRCROOT}/vk/Command.hpp
    ${SRCROOT}/vk/Command.cpp
    ${SRCROOT}/vk/Instance.hpp
    ${SRCROOT}/vk/Instance.cpp
    ${SRCROOT}/vk/Settings.hpp
    ${SRCROOT}/vk/Structs.hpp
    ${SRCROOT}/vk/Timeline.hpp
    ${SRCROOT}/vk/Timeline.cpp
    ${SRCROOT}/vk/result.hpp
    ${SRCROOT}/vk/vma.cpp
)

#create library
add_library(minerva)
target_sources(minerva PRIVATE ${SRC})
target_include_directories(minerva
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${PROJECT_SOURCE_DIR}/src
)
target_compile_features(minerva PUBLIC cxx_std_17)

#tell the compiler we want to export only symbols we marked so
set_target_properties(minerva PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(minerva PROPERTIES CMAKE_VISIBILITY_INLINES_HIDDEN 1)
target_compile_definitions(minerva PRIVATE MINERVA_EXPORTS)
#We need an extra switch for static builds
if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(minerva PUBLIC MINERVA_STATIC)
endif()

#link libraries
target_link_libraries(minerva PRIVATE Vulkan::Headers)
target_link_libraries(minerva PRIVATE volk)
target_link_libraries(minerva PRIVATE VulkanMemoryAllocator)

#Postfix for static and/or debugs
if(BUILD_SHARED_LIBS)
	set_target_properties(minerva PROPERTIES DEBUG_POSTFIX -d)
else()
    set_target_properties(minerva PROPERTIES DEBUG_POSTFIX -s-d)
    set_target_properties(minerva PROPERTIES RELEASE_POSTFIX -s)
    set_target_properties(minerva PROPERTIES MINSIZEREL_POSTFIX -s)
    set_target_properties(minerva PROPERTIES RELWITHDEBINFO_POSTFIX -s)
endif()

#install lib
install(
    TARGETS minerva
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/minerva
)
