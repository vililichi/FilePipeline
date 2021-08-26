#pragma once

#include <chrono>
#include <cstdint>
#include <queue>

using time_p = std::chrono::steady_clock::time_point;

struct tachymeterSample
{
    uint32_t state = 0;
    time_p time;
};

class Tachymeter
{
protected:
    time_p startTime;
    time_p endTime;
    bool runing;
    std::queue<tachymeterSample> samples;
    uint32_t totalBytesNbr;

public:
    Tachymeter();

    uint32_t speed();
    uint32_t avgSpeed();
    void addSample(uint32_t addedBytes);
    void start();
    void stop();
};

class chronometer
{
protected:
    uint32_t totalTime;
    time_p startTime;
    bool running;

public:
    chronometer();
    void start();
    void stop();
    void reset();
    uint32_t get();
};
