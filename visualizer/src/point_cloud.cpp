#pragma once

#include <fstream>
#include <string>
#include <iomanip>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

//ouster sdk
#include <ouster/client.h>
#include <ouster/lidar_scan.h>
#include <ouster/os_pcap.h>
#include <ouster/types.h>

#include "point_cloud.h"
#include "space.h"

extern Space space;

constexpr std::size_t BUF_SIZE = 65536;

void get_complete_scan(
    std::shared_ptr<ouster::sensor_utils::playback_handle> handle,
    ouster::LidarScan& scan, ouster::sensor::sensor_info& info) {
    // Make sure we start at beginning
    ouster::sensor_utils::replay_reset(*handle);

    // Helper variable to help us identify first full frame
    int first_frame_id = 0;

    auto pf = get_format(info);
    ouster::ScanBatcher batch_to_scan(info);

    // Buffer to store raw packet data
    ouster::sensor::LidarPacket packet(pf.lidar_packet_size);

    ouster::sensor_utils::packet_info packet_info;

    while (ouster::sensor_utils::next_packet_info(*handle, packet_info)) {
        auto packet_size = ouster::sensor_utils::read_packet(
            *handle, packet.buf.data(), packet.buf.size());

        if (packet_size == pf.lidar_packet_size &&
            packet_info.dst_port == info.config.udp_port_lidar) {
            if (batch_to_scan(packet, scan)) {
                if (first_frame_id == 0) {
                    // end of first frame -- assume it is incomplete and skip
                    first_frame_id = scan.frame_id;
                } else if (first_frame_id != scan.frame_id)
                    break;
            }
        }
    }
}

PointCloud::PointCloud(std::string filename, std::string metafilename, int filetype){

    if(filetype == 1){
        pcapParse(filename, metafilename);
    }else{
        std::cout << "File type " << filetype << " is not supported!!";
    }

}


void PointCloud::pcapParse(const std::string& pcap_path, const std::string& metadata_path) {

    // ouster::sensor_utils::replay_initialize(pcap_path);
    // auto stream_info = ouster::sensor_utils::get_stream_info(pcap_path);
    // std::cout << *stream_info << std::endl;

    auto handle = ouster::sensor_utils::replay_initialize(pcap_path);
    auto info = ouster::sensor::metadata_from_json(metadata_path);

    size_t w = info.format.columns_per_frame;
    size_t h = info.format.pixels_per_column;

    auto scan = ouster::LidarScan(info);

    std::cerr << "Reading in scan from pcap..." << std::endl;
    get_complete_scan(handle, scan, info);

    // 1. Getting XYZ
    std::cerr << "1. Calculating 3d Points... " << std::endl;
    //! [doc-stag-cpp-xyz]
    ouster::XYZLut lut = ouster::make_xyz_lut(info, true);
    auto range = scan.field(ouster::sensor::ChanField::RANGE);
    auto cloud = cartesian(range, lut);
    //! [doc-etag-cpp-xyz]
    //
    std::cout << "range : " << range.size() << std::endl;
    
    std::cerr << "\nLet's see what the 2000th point in this cloud is...  ("
              << cloud(2000, 0) << ", " << cloud(2000, 1) << ", "
              << cloud(2000, 2) << ")" << std::endl;

    for(int i=0; i<range.size(); i++){
        glm::vec3 point(cloud(i, 0), cloud(i, 1), cloud(i, 2));
        points.push_back(point);
        // std::cerr << "\nLet's see what the " << i << "th point in this cloud is...  ("
        //       << cloud(i, 0) << ", " << cloud(i, 1) << ", "
        //       << cloud(i, 2) << ")" << std::endl;
    }

    

}

void PointCloud::drawPoints(){

    for(auto &point: points){
        space.addPoint(point);
    }
    return;

}

void PointCloud::printHelp(int index){
    if(index == 2){ //support type
        std::cout << "[PointCloud : Supporting file type]" << std::endl;
        std::cout << "File type 1 : pcap file" << std::endl;
    }
}