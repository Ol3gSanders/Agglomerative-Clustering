#include "TimeMeasurer.h"

using std::chrono::steady_clock;
using std::chrono::duration;
using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::nanoseconds;

TimeMeasurer::TimeMeasurer() : startPoint( steady_clock::now() )
{

}

double TimeMeasurer::reset()
{
	const auto last_point = steady_clock::now();
	duration<double> elapsed = last_point - startPoint;
	startPoint = last_point;
	return elapsed.count();
}

double TimeMeasurer::getSec()
{
	return duration<double, seconds::period> ( steady_clock::now() - startPoint ).count();
}

double TimeMeasurer::getMsec()
{
	return duration<double, milliseconds::period>( steady_clock::now() - startPoint ).count();
}

double TimeMeasurer::getUsec()
{
	return duration<double, microseconds::period>( steady_clock::now() - startPoint ).count();
}

uint64_t TimeMeasurer::getNanoSec()
{
	return duration<uint64_t, nanoseconds::period>( steady_clock::now() - startPoint ).count();
}
