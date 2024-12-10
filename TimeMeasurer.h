#ifndef SRC_TIMEMEASURER_H_
#define SRC_TIMEMEASURER_H_

#include <chrono>

class TimeMeasurer
{
public:
	TimeMeasurer();
	double reset();
	double getSec();
	double getMsec();         // миллисекунды
	double getUsec();         // микросекунды
	uint64_t getNanoSec();

private:
	std::chrono::time_point<std::chrono::steady_clock> startPoint;
};

#endif /* SRC_TIMEMEASURER_H_ */
