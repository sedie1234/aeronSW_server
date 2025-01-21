#pragma once

#include <iostream>
#include <chrono>
#include <vector>

#define TIMEBUFFER_SIZE     5000

class TimeUtil{
public:
    TimeUtil();
    int timeCheck();
    bool timeOverFromLastTime_us(int us);
    bool timeOverFromLastTime_ms(int ms);
    void timebuffer_retrict();

private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point last_time;
    std::vector<std::chrono::high_resolution_clock::time_point> time_log;
};