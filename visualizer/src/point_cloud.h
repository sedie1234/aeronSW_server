#pragma once

#include <iostream>
#include <string>

class PointCloud {
public:
    PointCloud();
    //filetype = 1 : pcap
    PointCloud(std::string filename, std::string metafilename, int filetype);
    void pcapParse(const std::string& pcap_path, const std::string& metadata_path);
    void printHelp(int index);

private:

};