#pragma once
#include <tachymeter.h>

/*
struct tachymeterSample
{
	uint32_t state = 0;
	time_t time = time();
};

class Tachymeter
{
protected:
	time_t startTime;
	time_t endTime;
	bool runing;
	std::queue <uint32_t> samples;
public:


	uint32_t speed();
	void addSample(uint32_t addedBytes);
	void start();
	void stop();
};*/

#define MAXDELTAT 1000

#define nowTime() std::chrono::steady_clock::now()
#define toMillis std::chrono::duration_cast<std::chrono::milliseconds>


Tachymeter::Tachymeter():runing(false),totalBytesNbr(0)
{
	startTime = std::chrono::high_resolution_clock::now();
	endTime = std::chrono::high_resolution_clock::now();
}

void Tachymeter::addSample(uint32_t addedBytes)
{
	//ancienne données retiré
	time_p actTime = nowTime();
	while (samples.size() > 0)
	{
		if (toMillis(actTime - samples.front().time).count() > MAXDELTAT) samples.pop();
		else break;
	}

	//nouvelle donnée ajouté
	totalBytesNbr += addedBytes;
	tachymeterSample sample;
	sample.time = actTime;
	sample.state = totalBytesNbr;
	samples.push(sample);
}

void Tachymeter::start()
{
	runing = true;
	totalBytesNbr = 0;
	startTime = nowTime();
}

void Tachymeter::stop()
{

	endTime = nowTime();
	runing = false;
}

uint32_t Tachymeter::speed()
{
	//aucune vitesse si non en marche
	if (!runing) return 0;

	//ancienne données retiré
	time_p actTime = nowTime();
	long delta = toMillis(actTime - samples.front().time).count();
	while (samples.size() > 0)
	{
		if (toMillis(actTime - samples.front().time).count() > MAXDELTAT)
		{
			samples.pop();
		}
		else break;
	}

	//analyse impossible
	if (samples.size() < 2) return 0;

	//analyse
	const tachymeterSample older = samples.front();
	const tachymeterSample newer = samples.back();
	uint32_t msec = toMillis(newer.time - older.time).count();
	
	//division par 0
	if (msec == 0) return 0;

	return (newer.state - older.state) / msec;
}

uint32_t Tachymeter::avgSpeed()
{
	time_p end = endTime;
	if (runing)end = nowTime();
	return totalBytesNbr / (uint32_t)(toMillis(end - startTime).count());
}