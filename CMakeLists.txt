cmake_minimum_required(VERSION 3.5.0)
set(PROJECT_NAME "SendEmail_libcurl")

project(${PROJECT_NAME} LANGUAGES CXX)
project(${PROJECT_NAME}  VERSION 0.1.0)

find_package(CURL REQUIRED)

# Related files definition
file(GLOB_RECURSE RELATED_FILES
    "README.md"
)

# Source files definition
file(GLOB_RECURSE SRC_FILES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/smtp/*.cpp
)

# Files in project
add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${RELATED_FILES}
)


# Include dirs definition
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/smtp
)

SET(PROJECT_LIBRARIES
    curl
)

target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBRARIES})

# Install target
install(TARGETS ${PROJECT_NAME}
    DESTINATION bin
)
