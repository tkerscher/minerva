cmake_minimum_required(VERSION 3.14)

include(FetchContent)
include(GNUInstallDirs)

set(FETCHCONTENT_QUIET FALSE)

#read version from source
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp" ver)
string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${ver})
set(VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${ver})
set(VERSION_MINOR ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_PATCH = ([0-9]*)" _ ${ver})
set(VERSION_PATCH ${CMAKE_MATCH_1})

#create project
project("minerva"
    VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
    DESCRIPTION "Simple Framework for using the Vulkan API for computing"
    LANGUAGES C CXX
)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

#Fetch the vulkan headers
FetchContent_Declare(
    VulkanHeaders
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG v1.3.212
)
FetchContent_MakeAvailable(VulkanHeaders)
#Fetch volk loader
FetchContent_Declare(
    volk
    GIT_REPOSITORY https://github.com/zeux/volk.git
    GIT_TAG 1.3.204
)
FetchContent_MakeAvailable(volk)
#Fetch vulkan memory allocator
FetchContent_Declare(
    vma
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG v3.0.0
)
FetchContent_GetProperties(vma)
if(NOT vma_POPULATED)
    FetchContent_Populate(vma)
    add_library(VulkanMemoryAllocator INTERFACE)
    target_include_directories(VulkanMemoryAllocator INTERFACE ${vma_SOURCE_DIR}/include)
endif()
#fetch spirv-reflect
#Let's create the target ourselves to skip some things
FetchContent_Declare(
    reflect
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect.git
    GIT_TAG 41ea032be77bd7f1a82f3de0fe3e18778c74e689
)
FetchContent_GetProperties(reflect)
if(NOT reflect_POPULATED)
    FetchContent_Populate(reflect)
    #for whatever reason google decided to put the important files at the root
    #with all the other files...
    add_library(spirv-reflect STATIC
        ${reflect_SOURCE_DIR}/spirv_reflect.cpp
        ${reflect_SOURCE_DIR}/spirv_reflect.c)
    target_include_directories(spirv-reflect PUBLIC ${reflect_SOURCE_DIR})
endif()
#stb image - it has no CMakeLists.txt so we make the target ourselves
FetchContent_Declare(
    stb_s
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG af1a5bc352164740c1cc1354942b1c6b72eacb8a
)
FetchContent_GetProperties(stb_s)
if (NOT stb_s_POPULATED)
    FetchContent_Populate(stb_s)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${stb_s_SOURCE_DIR})
endif()

#source code
add_subdirectory(src)

#tools
add_subdirectory(tools)

#examples
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

#docs
option(BUILD_DOCS "Build documentation" OFF)
if(BUILD_DOCS)
    add_subdirectory(docs)
endif()
