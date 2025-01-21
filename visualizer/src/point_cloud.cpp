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

void PointCloud::get_complete_scan(std::shared_ptr<ouster::sensor_utils::playback_handle> handle) {
    // Make sure we start at beginning
    ouster::sensor_utils::replay_reset(*handle);

    // Helper variable to help us identify first full frame
    int prev_frame_id = 0;

    //get packet format
    auto pf = get_format(info);
    ouster::ScanBatcher batch_to_scan(info);

    //declare scan for mono_frame
    ouster::LidarScan scan(info);
    // auto scan = ouster::LidarScan(info);

    // Buffer to store raw packet data
    ouster::sensor::LidarPacket packet(pf.lidar_packet_size);

    ouster::sensor_utils::packet_info packet_info;

    // copy packets to scan vector
    while (ouster::sensor_utils::next_packet_info(*handle, packet_info)) { //move to next packet
        auto packet_size = ouster::sensor_utils::read_packet(
            *handle, packet.buf.data(), packet.buf.size());

        // std::cout << "packet size : " << packet_size << std::endl;

        if (packet_size == pf.lidar_packet_size &&
            packet_info.dst_port == info.config.udp_port_lidar) {
            // packet to scan
            if (batch_to_scan(packet, scan)) {
                // assume that packet has no frame id 0
                // next frame, copy scan to vec_scan
                // delete dynamic alloc memory of scan and alloc new memory
                // (reset scan)
                if (prev_frame_id == 0){
                    prev_frame_id = scan.frame_id;
                } else if (prev_frame_id != scan.frame_id){ 
                    vec_scan.push_back(scan);
                    prev_frame_id = scan.frame_id;
                    scan = ouster::LidarScan(info);
                    batch_to_scan(packet, scan);
                }
            }
            
        }
    }

}

PointCloud::PointCloud(std::string filename, std::string metafilename, int filetype){

    if(filetype == 1){
        getScanVecFromPcap(filename, metafilename);
    }else{
        std::cout << "File type " << filetype << " is not supported!!";
    }

}

// Get scan as parse pcap file
void PointCloud::getScanVecFromPcap(const std::string& pcap_path, const std::string& metadata_path) {

    // ouster::sensor_utils::replay_initialize(pcap_path);
    // auto stream_info = ouster::sensor_utils::get_stream_info(pcap_path);
    // std::cout << *stream_info << std::endl;

    auto handle = ouster::sensor_utils::replay_initialize(pcap_path);
    info = ouster::sensor::metadata_from_json(metadata_path);

    size_t w = info.format.columns_per_frame;
    size_t h = info.format.pixels_per_column;

    // auto scan = ouster::LidarScan(info);

    std::cerr << "Reading in scan from pcap..." << std::endl;


    get_complete_scan(handle);

    lut = ouster::make_xyz_lut(info, true);

}

void PointCloud::clearField(){
    field.clear();
    std::vector<glm::vec3>().swap(field);
}

void PointCloud::setField(int index){

        auto it = vec_scan.begin();
        auto range = (it + index)->field(ouster::sensor::ChanField::RANGE);
        std::cout << "here!!\n";
        auto cloud = cartesian(range, lut);
        //! [doc-etag-cpp-xyz]
        //
        
        for(int i=0; i<range.size(); i++){
            glm::vec3 point(cloud(i, 0), cloud(i, 1), cloud(i, 2));
            field.push_back(point);
        }
        return;
}

int PointCloud::getFrameSize(){
   return vec_scan.size(); 
}

void PointCloud::drawPoints(){

    for(auto &point: field){
        space.addPoint(point);
    }
    
    return;
}

void PointCloud::printHelp(int index){
    if(index == 2){ //support type
        std::cout << "[PointCloud : Supporting file type]" << std::endl;
        std::cout << "File type 1 : pcap file" << std::endl;
    }
    return;
}