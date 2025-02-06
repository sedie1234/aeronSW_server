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
    int getFreq();

    // whether field exist or not?
    bool isFieldAvailable(ouster::LidarScan& scan, const char* _field);

    // get scan from pcap file
    void get_complete_scan(std::shared_ptr<ouster::sensor_utils::playback_handle> handle);

    std::vector<std::pair<glm::vec3, uint32_t>> color_field;
private:

    // fields = array of points, 1 frame to draw
    std::vector<glm::vec3> field;

    // new version field with color

    // vec_scan = array of frame, video, include range, intesity, ... and so on.
    std::vector<ouster::LidarScan> vec_scan;

    // range to cartesian LUT
    ouster::XYZLut lut;

    //lidar info
    ouster::sensor::sensor_info info;

    std::vector<Eigen::Array<uint32_t, -1, -1, Eigen::RowMajor>> reflectivities;

    std::vector<uint32_t> destaggered_reflectivities;

    int reflect_flag;

};