#include <fast-lib/message/migfra/time_measurement.hpp>
#include <fast-lib/log.hpp>

#include <stdexcept>
#include <chrono>

FASTLIB_LOG_INIT(tm_log, "Time_measurement")

namespace fast {
namespace msg {
namespace migfra {

void Times::clear()
{
	wall = 0LL;
	start_timestamp = 0;
	stop_timestamp = 0;
}

// Returns wall since epoch or last_wall using high_resolution_clock and timestamp using system_clock
std::pair<nanosecond_type, std::time_t> get_time(nanosecond_type last_wall = 0)
{
	return std::make_pair<nanosecond_type, std::time_t>(
		std::chrono::duration<nanosecond_type, std::nano>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() - last_wall,
		std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
	);
}

Timer::Timer()
{
	start();
}

bool Timer::is_stopped() const noexcept
{
	return stopped;
}

Times Timer::elapsed() const noexcept
{
	if (is_stopped())
		return times;
	Times current(times);
	std::tie(current.wall, current.stop_timestamp) = get_time(current.wall);
	return current;
}

std::string Timer::format(const std::string &format) const
{
	const double sec = 1000000000.0L;
	double wall_sec = static_cast<double>(times.wall) / sec;
	if (format == "timestamps") {
		std::string start_timestamp(std::ctime(&times.start_timestamp));
		std::string stop_timestamp(std::ctime(&times.stop_timestamp));
		return "wall: " + std::to_string(wall_sec) + " started: " + start_timestamp + " stopped: " + stop_timestamp;
	} else {
		return std::to_string(wall_sec);
	}
}

void Timer::start() noexcept
{
	stopped = false;
	std::tie(times.wall, times.start_timestamp) = get_time();
}

void Timer::stop() noexcept
{
	if (is_stopped())
		return;
	stopped = true;
	std::tie(times.wall, times.stop_timestamp) = get_time(times.wall);
}

void Timer::resume() noexcept
{
	Times current(times);
	start();
	times.wall -= current.wall;
}

Time_measurement::Time_measurement(bool enable_time_measurement, std::string format) :
	enabled(enable_time_measurement),
	format(format)
{
}

Time_measurement::~Time_measurement()
{
	for (auto &timer : timers) {
		if (!timer.second.is_stopped()) {
			FASTLIB_LOG(tm_log, warn) << "Timer with name \"" + timer.first + "\" has not been stopped, but task is finished. Search for a tick without subsequent tock or ignore if there was a preceding error.";
		}
	}
}

void Time_measurement::tick(const std::string &timer_name)
{
	if (enabled) {
		if (timers.find(timer_name) != timers.end())
			throw std::runtime_error("Timer with name \"" + timer_name + "\" already exists.");
		timers[timer_name];
	}
}

void Time_measurement::tock(const std::string &timer_name)
{
	if (enabled) {
		try {
			timers.at(timer_name).stop();
		} catch (const std::out_of_range &e) {
			throw std::runtime_error("Timer with name \"" + timer_name + "\" not found. Search for a tock without preceding tick.");
		}
	}
}

bool Time_measurement::empty() const
{
	return timers.empty();
}

YAML::Node Time_measurement::emit() const
{
	YAML::Node node;
	for (auto &timer : timers) {
		node[timer.first] = timer.second.format(format);
	}
	return node;
}

void Time_measurement::load(const YAML::Node &node)
{
	(void) node;
	const std::string err_str("Class Time_measurement does not support loading from YAML.");
	FASTLIB_LOG(tm_log, error) << err_str;
	throw std::runtime_error(err_str);
}

}
}
}
