#pragma once

#include <chrono>
#include <queue>
#include <stdint.h>

typedef std::chrono::steady_clock::time_point time_p;

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
	std::queue <tachymeterSample> samples;
	uint32_t totalBytesNbr;
public:
	Tachymeter();

	uint32_t speed();
	uint32_t avgSpeed();
	void addSample(uint32_t addedBytes);
	void start();
	void stop();
};