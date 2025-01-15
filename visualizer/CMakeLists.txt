cmake_minimum_required(VERSION 3.26)

# 프로젝트 이름 및 버전
project(aeronSW_visulaizer VERSION 1.0)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# OpenGL 및 GLFW 라이브러리 찾기
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# 소스 파일 및 헤더 파일 지정
set(SRC_FILES
    src/main.cpp
    src/camera.cpp
    src/space.cpp
)

set(HEADER_FILES
    src/camera.h
    src/space.h
)

# 실행 파일 생성
add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

# 라이브러리 연결
target_link_libraries(${PROJECT_NAME} 
    OpenGL::GL
    OpenGL::GLU
    GLUT::GLUT
    GLEW::GLEW
    glfw
)

# include 디렉토리 추가
target_include_directories(${PROJECT_NAME} PRIVATE src)
