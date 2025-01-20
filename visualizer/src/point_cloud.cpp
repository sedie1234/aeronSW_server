#pragma once

#include <fstream>
#include <string>
#include <iomanip>
#include <iostream>
#include <vector>

// #include <pcap.h>

//ouster sdk
#include <ouster/client.h>
#include <ouster/lidar_scan.h>
#include <ouster/os_pcap.h>
// #include <ouster/types.h>

#include "point_cloud.h"

PointCloud::PointCloud(std::string filename, std::string metafilename, int filetype){

    if(filetype == 1){
        pcapParse(filename, metafilename);
    }else{
        std::cout << "File type " << filetype << " is not supported!!";
    }

}

#if 1

void PointCloud::pcapParse(const std::string& pcap_path, const std::string& metadata_path) {

    // ouster::sensor_utils::replay_initialize(pcap_path);
    auto stream_info = ouster::sensor_utils::get_stream_info(pcap_path);
    std::cout << *stream_info << std::endl;

}

#else
void PointCloud::pcapParse(const std::string& pcap_path, const std::string& metadata_path) {

    char errbuf[PCAP_ERRBUF_SIZE];

    // open pcap
    pcap_t* pcap = pcap_open_offline(pcap_path.c_str(), errbuf);
    if (!pcap) {
        std::cerr << "Failed to open pcap file: " << errbuf << std::endl;
        return;
    }

    // Ouster packet filter : udp port 
    const std::string filter = "udp port 7502";
    struct bpf_program fp;
    if (pcap_compile(pcap, &fp, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
        std::cerr << "Failed to compile pcap filter: " << pcap_geterr(pcap) << std::endl;
        pcap_close(pcap);
        return;
    }
    if (pcap_setfilter(pcap, &fp) == -1) {
        std::cerr << "Failed to set pcap filter: " << pcap_geterr(pcap) << std::endl;
        pcap_close(pcap);
        return;
    }

    std::cout << "Reading packets from pcap file: " << pcap_path << std::endl;

    // get info
    // open metadata

    auto info = ouster::sensor::metadata_from_json(metadata_path);
    if (info.format.columns_per_frame == 0 || info.format.pixels_per_column == 0) {
        std::cerr << "Invalid metadata file: " << metadata_path << std::endl;
        return;
    }

    // std::ifstream metadata_file(metadata_path);
    // if (!metadata_file) {
    //     std::cerr << "Failed to open metadata file: " << metadata_path << std::endl;
    //     return ;
    // }

    // // read metadata ,generte the sensor_info
    // std::string metadata_json((std::istreambuf_iterator<char>(metadata_file)),
    //                           std::istreambuf_iterator<char>());
    // auto sensor_info = ouster::sensor::metadata_to_sensor_info(metadata_json);


    // Ouster LidarScan
    ouster::sensor::packet_format pf = ouster::sensor::get_format(info);
    ouster::LidarScan scan(1024, 10);  // 1024x10 resolution

    struct pcap_pkthdr* header;
    const u_char* packet;

    // loop reading packet
    while (pcap_next_ex(pcap, &header, &packet) >= 0) {
        if (header->caplen >= pf.lidar_packet_size) {
            const uint8_t* lidar_packet = packet + (header->caplen - pf.lidar_packet_size);

            // read packet data
            std::cout << "packet_type : " << pf.pack
            // auto column_data = lidar_packet + pf.lidar_column_offset(0);
            // for (size_t i = 0; i < sensor_info.format.columns_per_frame; i++) {
            //     auto intensity = pf.intensity(column_data, i);
            //     auto range = pf.range(column_data, i);
            //     std::cout << "Column " << i << ": Intensity = " << intensity << ", Range = " << range << std::endl;
            // }
        }
    }

    pcap_close(pcap);

}
#endif

void PointCloud::printHelp(int index){
    if(index == 2){ //support type
        std::cout << "[PointCloud : Supporting file type]" << std::endl;
        std::cout << "File type 1 : pcap file" << std::endl;
    }
}