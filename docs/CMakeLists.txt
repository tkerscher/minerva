find_package(Doxygen REQUIRED dot)

#we are gonna use a custom doxygen theme from github
FetchContent_Declare(
	doxygen_awesome
	GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
	GIT_TAG v2.0.3
	GIT_PROGRESS TRUE
	USES_TERMINAL_DOWNLOAD TRUE)
FetchContent_GetProperties(doxygen_awesome)
if (NOT doxygen_awesome_POPULATED)
	FetchContent_Populate(doxygen_awesome)
	set(DOXYGEN_THEME_DIR ${doxygen_awesome_SOURCE_DIR})
endif()

#configure doxygen
set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in)
set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

set(DOXYGEN_INPUT ${PROJECT_SOURCE_DIR}/include/)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/)
set(PAGES_DIR ${PROJECT_SOURCE_DIR}/docs/pages/)
set(MAINPAGE "mainpage.md")

configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

#create target
add_custom_target(docs ALL
	COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating documentation using doxygen..."
	VERBATIM
)

#install
install(
	DIRECTORY ${DOXYGEN_OUTPUT_DIR}/html/
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
	COMPONENT docs
)
