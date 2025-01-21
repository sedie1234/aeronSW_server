#cmake version
cmake_minimum_required(VERSION 3.26)

# project name and version
project(aeronSW_visualizer VERSION 1.0)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# OpenGL, GLFW Library
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Set ouster SDK path
# set(OUSTER_SDK_DIR "../third_party/ouster-sdk")
# include_directories(${OUSTER_SDK_DIR}/ouster_client/include)
# link_directories(${OUSTER_SDK_DIR}/build/)

#pcap library
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)

# OusterSDK
set(CMAKE_PREFIX_PATH "/usr/local/lib/cmake/OusterSDK" ${CMAKE_PREFIX_PATH})
find_package(OusterSDK REQUIRED COMPONENTS Static)

# eigen library
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# jsoncpp library
find_package(jsoncpp REQUIRED)
include_directories(/usr/include/jsoncpp)
link_directories(/usr/lib/x86_64-linux-gnu)

#optional-lite
find_path(OPTIONAL_LITE_INCLUDE_DIRS "optional.hpp" PATHS "/usr/local/include" "/usr/include")
include_directories(${OPTIONAL_LITE_DIR})


# Source file and header
set(SRC_FILES
    src/main.cpp
    src/camera.cpp
    src/space.cpp
    src/point_cloud.cpp
    src/time_util.cpp
)

set(HEADER_FILES
    src/camera.h
    src/space.h
    src/configs.h
    src/point_cloud.h
    src/time_util.h
)

# Execution files
add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

# Link
target_link_libraries(${PROJECT_NAME} 
    OpenGL::GL
    OpenGL::GLU
    GLUT::GLUT
    GLEW::GLEW
    glfw
    ouster_client
    jsoncpp
    Eigen3::Eigen
    ${PCAP_LIBRARIES}
    OusterSDK::ouster_client
    OusterSDK::ouster_pcap
)

# Add the include directory
target_include_directories(${PROJECT_NAME} PRIVATE 
    src
    ${EIGEN3_INCLUDE_DIR}
    ${PCAP_INCLUDE_DIRS}

)
