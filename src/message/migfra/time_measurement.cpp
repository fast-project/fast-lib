#include <fast-lib/message/migfra/time_measurement.hpp>

#include <boost/log/trivial.hpp>

#include <stdexcept>

namespace fast {
namespace msg {
namespace migfra {

Time_measurement::Time_measurement(bool enable_time_measurement) :
	enabled(enable_time_measurement)
{
}

Time_measurement::~Time_measurement()
{
	for (auto &timer : timers) {
		if (!timer.second.is_stopped()) {
			BOOST_LOG_TRIVIAL(error) << "Timer with name \"" + timer.first + "\" has not been stopped, but task is finished. Search for a tick without subsequent tock or ignore if there was a preceding error.";
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
		node[timer.first] = timer.second.format();
	}
	return node;
}

void Time_measurement::load(const YAML::Node &node)
{
	(void) node;
	const std::string err_str("Class Time_measurement does not support loading from YAML.");
	BOOST_LOG_TRIVIAL(error) << err_str;
	throw std::runtime_error(err_str);
}

}
}
}
