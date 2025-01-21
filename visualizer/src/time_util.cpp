#pragma once

#include <iostream>
#include <chrono>

#include "time_util.h"

TimeUtil::TimeUtil(){
    start_time = std::chrono::high_resolution_clock::now();
    last_time = std::chrono::high_resolution_clock::now(); 
}

int TimeUtil::timeCheck(){
    auto _time = std::chrono::high_resolution_clock::now();

    time_log.push_back(_time);
    last_time = _time;

    timebuffer_retrict();

    return time_log.size();
}

bool TimeUtil::timeOverFromLastTime_us(int us){
    auto _time = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(_time - last_time);

    if(duration.count() < us)
        return false;
    else
        return true;
    
}

bool TimeUtil::timeOverFromLastTime_ms(int ms){
    auto _time = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(_time - last_time);

    if(duration.count() < ms)
        return false;
    else
        return true;

}

void TimeUtil::timebuffer_retrict(){
    if(time_log.size() > TIMEBUFFER_SIZE){
        time_log.erase(time_log.begin());
    }
}