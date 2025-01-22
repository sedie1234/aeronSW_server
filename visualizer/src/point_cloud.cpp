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

    auto handle = ouster::sensor_utils::replay_initialize(pcap_path);
    info = ouster::sensor::metadata_from_json(metadata_path);

    size_t w = info.format.columns_per_frame;
    size_t h = info.format.pixels_per_column;

    std::cerr << "Reading in scan from pcap..." << std::endl;

    get_complete_scan(handle);

    lut = ouster::make_xyz_lut(info, true);

}

int PointCloud::getFreq(){
    return info.format.fps;
}

void PointCloud::clearField(){
    field.clear();
    std::vector<glm::vec3>().swap(field);

    color_field.clear();
    std::vector<std::pair<glm::vec3, uint32_t>>().swap(color_field);
}

void PointCloud::setField(int index){

        auto it = vec_scan.begin();
        auto range = (it + index)->field(ouster::sensor::ChanField::RANGE);
        auto cloud = cartesian(range, lut);

        //////////////////test
        Eigen::Array<uint32_t, -1, -1, Eigen::RowMajor> reflectivity;
        reflectivity = (it+index)->field<uint8_t>(ouster::sensor::ChanField::REFLECTIVITY).cast<uint32_t>();
        reflectivities.push_back(reflectivity);
        // auto signal = (it+index)->field<uint8_t>(ouster::sensor::ChanField::SIGNAL).cast<uint32_t>();
        auto reflectivity_destaggered = ouster::destagger<uint32_t>(reflectivity, info.format.pixel_shift_by_row);

        // std::cout << "range size : " << range.size() << std::endl;
        // std::cout << "signal size : " << signal.size() << std::endl;
        // std::cout << "reflectivity size : " << reflectivity.size() << std::endl;

        // reflectivity example
        size_t w = info.format.columns_per_frame;   //data along channel
        size_t h = info.format.pixels_per_column;   //channels
        
        // std::cout << "reflectivity : " << reflectivity(print_row, print_column) << " : " << 
                    // reflectivity_destaggered(print_row, print_column) << std::endl;

        // signal example
        if(isFieldAvailable(*(it+index), ouster::sensor::ChanField::SIGNAL)){
            // if signal data exist, this is the way how to get signal data
            auto signal_field = (it+index)->field<uint32_t>(ouster::sensor::ChanField::SIGNAL);
            std::cout << signal_field(0, 0) << std::endl;
        }else{
            // test data has no signal data
        } 
        //////////////////test


        //! [doc-etag-cpp-xyz]
        //

        if(range.size() == w*h){
            reflect_flag = 1;
        }else{
            std::cout << "range size is not equal to w*h" << std::endl;
            reflect_flag = 0;
        }

        for(int i=0; i<range.size(); i++){
            glm::vec3 point(cloud(i, 0), cloud(i, 1), cloud(i, 2));
            field.push_back(point);

            if(reflect_flag){
                // std::cout << "w : " << i%w << ", h : " << i/w << std::endl;
                uint32_t reflect = reflectivity_destaggered(i/w, i%w);
                destaggered_reflectivities.push_back(reflect);
                std::pair<glm::vec3, uint32_t> _color_point(point, reflect);
                color_field.push_back(_color_point);
            }

            //////////////////test
            // std::cout << i << "th range : " << cloud(i, 0) << ", " << cloud(i, 1) << ", " << cloud(i, 2) << std::endl;
            // std::cout << i << "th signal : " << signal << std::endl;
            
            //////////////////test
        }
        return;
}

bool PointCloud::isFieldAvailable(ouster::LidarScan& scan, const char* _field){
            
    try{
        scan.field<uint32_t>(_field);
        return true;
    }catch(const std::invalid_argument&){
        return false;
    }
}

int PointCloud::getFrameSize(){
   return vec_scan.size(); 
}

void PointCloud::drawPoints(){

    float color_sense;

    if(reflect_flag){
        // for(int i=0; i<field.size(); i++){
        //     color_sense = destaggered_reflectivities[i]/200.0f;
        //     glm::vec3 color(1.0f - color_sense, color_sense, 0.0f);
        //     space.addPoint(field[i], color);
        // }
        for(auto &point: color_field){
            color_sense = point.second / 26.54f;
            glm::vec3 color(0.0f, 0.973f - color_sense, 0.364f + color_sense);
            space.addPoint(point.first, color);
        }

    }else{
        for(auto &point: field){
            space.addPoint(point);
        }
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