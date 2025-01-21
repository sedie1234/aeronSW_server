#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <string>

#include "camera.h"
#include "space.h"
#include "configs.h"
#include "point_cloud.h"
#include "time_util.h"


// Globals
Camera camera;
Space space;

bool leftMousePressed = false;
double lastMouseX = 0.0, lastMouseY = 0.0;
float horizontalAngle = glm::radians(INIT_CAM_HANGLE * 360.0f / 100.0f), verticalAngle = glm::radians(INIT_CAM_VANGLE * 360.0f / 100.0f); // 카메라의 회전 각도
float orbitRadius = INIT_CAM_RADIUS;                           // 궤도 반지름

// screen control
// 0 : play // 1 : pause // 2 : prev frame // 3 : next frame
int video_control = 0;
float video_speed_coeffi = 1.0f;
float prev_speed_coeffi = video_speed_coeffi;

// camera position offset
glm::vec3 cameraPosition(0.0f, 0.0f, 0.0f);
glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);


float x = orbitRadius * glm::cos(verticalAngle) * glm::cos(horizontalAngle) + cameraPosition.x;
float y = orbitRadius * glm::cos(verticalAngle) * glm::sin(horizontalAngle) + cameraPosition.y;
float z = orbitRadius * glm::sin(verticalAngle) + cameraPosition.z;

// argument callback function
void printHowToUse();
void printUsage();
void printErr(int index);
std::string getFileExtension(const std::string& fileName);

// Function Prototypes
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void setupViewport(const GLuint width, const GLuint height);


int main(int argc, char* argv[]) {

    /**** Option argument parsing (with no external lib) ****/

    std::string lidar_filename;
    std::string lidar_metadata_filename;
    int lidar_data_type=0; //0 : none, 1:pcap

    if(argc < 0){
        printHowToUse();
        printUsage();
    }

    // parser
    for (int i=0; i<argc; i++){
        std::string arg = argv[i];
        if(arg == "-h" || arg == "--help"){
            printUsage();
        }else if((arg == "-d" || arg == "--data") ){
            if(1 + i < argc){
                i++;
                std::string arg2 = argv[i];
                if(arg2 == "-p"){ // file type = pcap type : -d -p [pcapfile] [json meta file]

                    if(2 + i < argc){
                        lidar_data_type = 1;
                        lidar_filename = argv[i+1];
                        lidar_metadata_filename = argv[i+2];
                        
                        if(getFileExtension(lidar_filename) != "pcap" ||
                            getFileExtension(lidar_metadata_filename) != "json"){
                            printErr(3);
                            return 0;
                        }
                        
                        i+=2;
                        
                    }else{
                        printErr(3); // -d -p error print
                    }
                    

                }else{
                    printErr(2);
                    return 0;
                }
            }else{
                printErr(2); // -d error print
                return 0;
            }
        }
    }



    /**** pcap lidar data example ****/
    PointCloud _cloud(lidar_filename, lidar_metadata_filename, lidar_data_type);

    /**** time util ****/
    TimeUtil time_util;
    int ms_per_frame = 1000 / _cloud.getFreq() * VIDEO_SPEED;

    /**** Initialize GLFW ****/ 
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Orbiting Camera", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    setupViewport(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);


    /**** Set callbacks ****/ 
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    /*********************************************/
    /******************set scene******************/
    /*********************************************/
    
    //scene set    
    space.drawGrid();
    

    space.addPoint(glm::vec3(0.0f, 0.0f, 0.0f));  // (0,0,0)
    space.addPoint(glm::vec3(1.0f, 1.0f, 0.0f));  // (1,1,0)
    space.addPoint(glm::vec3(-1.0f, -1.0f, 0.0f));  // (-1,-1,0)
    space.addPoint(glm::vec3(-1.0f, -1.0f, -1.0f));  // (-1,-1,-1)
    space.addPoint(glm::vec3(1.0f, 1.0f, -1.0f));  // (1,1,-1)
    space.addObj(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    space.addLine(glm::vec3(1.1f, 2.2f, -1.2f), glm::vec3(-5.0f, 10.0f, 13.0f), 1.0f, 0.0f, 1.0f);


    // Add central object
    space.addPoint(glm::vec3(0.0f, 0.0f, 0.0f));  // 중앙 객체

    // Add Floor
    for (int i = 0; i < 100; ++i) {
        float angle = glm::radians(i * 360.0f / 100.0f);
        space.addPoint(glm::vec3(glm::cos(angle) * 2.0f, glm::sin(angle) * 2.0f, 0.0f));
    }

    /*********************************************/
    /******************set scene******************/
    /*********************************************/

    /**** Main loop ****/ 
    while (!glfwWindowShouldClose(window)) {
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        

        /**** draw screen ****/

        // init space
        space.clearPoints();
        space.clearLines();
        space.drawGrid();

        // frame init
        static int frame_index;
        _cloud.clearField();

        // set frame
        _cloud.setField(frame_index);

        int new_ms_per_frame = ms_per_frame * video_speed_coeffi;
        if(prev_speed_coeffi != video_speed_coeffi){
            prev_speed_coeffi = video_speed_coeffi;
            std::cout << "video speed changed! : coefficient = " << video_speed_coeffi << std::endl;
        }
        
        if(time_util.timeOverFromLastTime_ms(new_ms_per_frame) && (video_control == 0)){ //video
            // next frame
            frame_index++;
            // time count
            time_util.timeCheck();
        }

        if(video_control == 2){
            if(frame_index > 0)
                frame_index--;
            video_control = 1;
        }

        if(video_control == 3){
            if(frame_index < _cloud.getFrameSize())
                frame_index++;
            video_control = 1;
        }

        // draw points of frame
        _cloud.drawPoints();

        // if last frame, stay
        if(frame_index == _cloud.getFrameSize()){
            frame_index = _cloud.getFrameSize() - 1;
        }

        // example for adding and showing object x, y, z axis
        space.addObj(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::vec3 obj1Position(2.0f, 3.0f, -5.0f);
        glm::vec3 obj1E1(0.577f, 0.577f, 0.577f);  // 대각선 방향
        glm::vec3 obj1E2(-0.707f, 0.707f, 0.0f);   // X-Y 평면에서 직교
        glm::vec3 obj1E3(-0.408f, -0.408f, 0.816f); // Z축에서 직교

        glm::vec3 obj3Position(-2.0f, -1.0f, 4.0f);
        glm::vec3 obj3E1(0.577f, 0.577f, 0.577f);  // X, Y, Z 축 동일 기여
        glm::vec3 obj3E2(-0.707f, 0.707f, 0.0f);   // X-Y 평면에서 대각선
        glm::vec3 obj3E3(-0.408f, -0.408f, 0.816f); // Z축 기여 증가


        space.addObj(obj1Position, obj1E1, obj1E2, obj1E3);
        space.addObj(obj3Position, obj3E1, obj3E2, obj3E3);

        // example for drawing spring

        // 스프링 매개변수
        int numPoints = 200;       // 총 포인트 개수
        float radius = 1.0f;      // 스프링 반지름
        float height = 10.0f;     // 스프링 전target체 높이
        int turns = 3;            // 스프링의 회전 수

        // 포인트 저장 벡터
        std::vector<glm::vec3> points;

        // 스프링 포인트 계산 및 저장
        for (int i = 0; i < numPoints; ++i) {
            float t = static_cast<float>(i) / numPoints; // 0 ~ 1 비율
            float theta = glm::two_pi<float>() * turns * t; // 나선형 각도
            float z = height * t;                          // 높이 증가

            float x = radius * glm::cos(theta); // X 좌표
            float y = radius * glm::sin(theta); // Y 좌표

            points.emplace_back(x+5, y+3, z+1);
        }

        for(auto &point : points){
            space.addPoint(point);
        }



        // Update camera position based on spherical coordinates
        float x = orbitRadius * glm::cos(verticalAngle) * glm::cos(horizontalAngle) + cameraPosition.x;
        float y = orbitRadius * glm::cos(verticalAngle) * glm::sin(horizontalAngle) + cameraPosition.y;
        float z = orbitRadius * glm::sin(verticalAngle) + cameraPosition.z;
        camera.setPosition(glm::vec3(x, y, z));
        camera.setTarget(cameraTarget);

        // Apply camera view matrix
        glm::mat4 view = camera.getViewMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(&view[0][0]);

        // Render scene
        space.render();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void printErr(int index){

    std::cout << "[Occur the Error!!]" << std::endl;
    if(index == 2){ // Error Number 2 : wrong argument with option -d
        std::cout << "Option -d or --data need the additional option [filename]" << std::endl;
        std::cout << "Usage : ./aeronSW-visualizer -d [Lidar Data filename]" << std::endl;
        std::cout << "or check the arguments : ./aeronSW-visualizer -h" << std::endl;
    }else if(index == 3){ // Error Number 3 : wron argumnet with option -d -p
        std::cout << "Option -d -p need the additional options [pcap filename] [json filename]" << std::endl;
        std::cout << "Usage : ./aeronSW-visualizer -d -p [lidar_filename.pcap] [metadata_filename.json]" << std::endl;
    }
}

void printHowToUse(){
    std::cout << "./aeronSW-visualizer -d [Lidar Data filename]" << std::endl;
}

void printUsage(){
    std::cout << "[Arguments]" << std::endl;
    std::cout << "-h, --help : Show you the Usage" << std::endl;
    std::cout << "-d, --data [option] [lidar filename] [metadata filename]\n"
                            "\t\t\t\t: Data file path that you want to show" << std::endl;
    std::cout << "-d -p [pcap_filename] [json_filename]" << std::endl;
}

std::string getFileExtension(const std::string& fileName) {
    // 마지막 '.'의 위치를 찾음
    size_t dotPos = fileName.rfind('.');
    if (dotPos == std::string::npos) {
        // '.'이 없다면 확장자가 없는 경우
        return "";
    }
    // '.' 이후의 문자열 반환
    return fileName.substr(dotPos + 1);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        glm::vec3 cameraDirection(glm::cos(horizontalAngle),
                                  glm::sin(horizontalAngle),
                                  0.0f);
        glm::vec3 cameraOrthoDirection(-glm::sin(horizontalAngle),
                                       glm::cos(horizontalAngle),
                                       0.0f);
        if (key == GLFW_KEY_W) {                            //w
            cameraPosition -= CAMERA_SPEED * cameraDirection; // 앞으로 이동
            cameraTarget -= CAMERA_SPEED * cameraDirection; // 앞으로 이동
        } else if (key == GLFW_KEY_S) {                     //s
            cameraPosition += CAMERA_SPEED * cameraDirection; // 뒤로 이동
            cameraTarget += CAMERA_SPEED * cameraDirection; // 뒤로 이동
        } else if (key == GLFW_KEY_A) {                     //a
            cameraPosition -= CAMERA_SPEED * cameraOrthoDirection; // 왼쪽 이동
            cameraTarget -= CAMERA_SPEED * cameraOrthoDirection; // 왼쪽 이동
        } else if (key == GLFW_KEY_D) {                     //d
            cameraPosition += CAMERA_SPEED * cameraOrthoDirection; // 오른쪽 이동
            cameraTarget += CAMERA_SPEED * cameraOrthoDirection; // 오른쪽 이동
        } else if (key == GLFW_KEY_ESCAPE) {                //esc
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        } else if (key == GLFW_KEY_P) {                     //p
            if(video_control != 1)
                video_control = 1;                          //pause
            else
                video_control = 0;                          //play
        } else if (key == GLFW_KEY_Q) {                     //q
            video_control = 2;                              //previous frame
        } else if (key == GLFW_KEY_E) {                     //e
            video_control = 3;                              //next frame
        } else if (key == GLFW_KEY_PAGE_DOWN){              //page down
            if (video_speed_coeffi < VIDEO_SPEED_COEFFI_MAX)
                video_speed_coeffi += VIDEO_SPEED_CONTROL_RESOLUTION; //video speed down
        } else if (key == GLFW_KEY_PAGE_UP){                //page up
            if (video_speed_coeffi > VIDEO_SPEED_COEFFI_MIN)
                video_speed_coeffi -= VIDEO_SPEED_CONTROL_RESOLUTION; //video speed up
        }
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        leftMousePressed = (action == GLFW_PRESS);
    }
}


void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (leftMousePressed) {
        double dx = xpos - lastMouseX;
        double dy = ypos - lastMouseY;

        horizontalAngle += (float)(dx * ORBIT_SPEED);
        verticalAngle -= (float)(dy * ORBIT_SPEED);
        
        //when I click first_click, why vertialAngle go to zero???

        // 상하 회전 제한 (90도 범위 유지)
        if (verticalAngle > glm::radians(89.0f)) verticalAngle = glm::radians(89.0f);
        if (verticalAngle < glm::radians(-89.0f)) verticalAngle = glm::radians(-89.0f);


    }
    lastMouseX = xpos;
    lastMouseY = ypos;

}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    orbitRadius -= (float)(yoffset * ZOOM_SPEED);
    if (orbitRadius < 1.0f) orbitRadius = 1.0f; // 최소 거리 제한
}

void setupViewport(const GLuint width, const GLuint height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(width) / height, 0.1, 100.0);
}

