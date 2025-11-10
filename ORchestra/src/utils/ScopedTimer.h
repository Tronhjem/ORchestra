#pragma once

#include <chrono>
#include <iostream>

class ScopedTimer
{
public:
    ScopedTimer(const std::string &name) :
        mName(name),
        mStartTime(std::chrono::high_resolution_clock::now())
    {
        
    }

    ~ScopedTimer()
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - mStartTime).count();
        std::cout << mName << " executed in " << duration << " µs" << std::endl;
    }

private:
    std::string mName;
    std::chrono::high_resolution_clock::time_point mStartTime;
};
