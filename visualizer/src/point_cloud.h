#pragma once

#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include <ouster/client.h>
#include <ouster/lidar_scan.h>
#include <ouster/os_pcap.h>
#include <ouster/types.h>


class PointCloud {
public:
    PointCloud();
    //filetype = 1 : pcap
    PointCloud(std::string filename, std::string metafilename, int filetype);
    
    void printHelp(int index);
    void drawPoints();
    void getScanVecFromPcap(const std::string& pcap_path, const std::string& metadata_path);
    
    // set field (points) to draw frame, get index's field data from vec_scan
    void setField(int index);
    void clearField();
    int getFrameSize();

    // get scan from pcap file
    void get_complete_scan(std::shared_ptr<ouster::sensor_utils::playback_handle> handle);

private:

    // fields = array of points, 1 frame to draw
    std::vector<glm::vec3> field;

    // vec_scan = array of frame, video, include range, intesity, ... and so on.
    std::vector<ouster::LidarScan> vec_scan;

    // range to cartesian LUT
    ouster::XYZLut lut;

    //lidar info
    ouster::sensor::sensor_info info;

};