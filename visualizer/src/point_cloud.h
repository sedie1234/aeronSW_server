#pragma once

#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

class PointCloud {
public:
    PointCloud();
    //filetype = 1 : pcap
    PointCloud(std::string filename, std::string metafilename, int filetype);
    void pcapParse(const std::string& pcap_path, const std::string& metadata_path);
    void printHelp(int index);
    void drawPoints();

private:
    std::vector<glm::vec3> points;    

};