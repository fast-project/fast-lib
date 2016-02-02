#ifndef FAST_LIB_MESSAGE_MIGFRA_TIME_MEASUREMENT_HPP
#define FAST_LIB_MESSAGE_MIGFRA_TIME_MEASUREMENT_HPP

#include <fast-lib/serializable.hpp>
#include <boost/timer/timer.hpp>

#include <string>
#include <unordered_map>

namespace fast {
namespace msg {
namespace migfra {

// TODO: Add timer guard.
// TODO: drop boost dependency by implementing timers using std::high_resolution_clock
// TODO: Split in serialization and implementation part so that the namespace fits
class Time_measurement :
	public fast::Serializable
{
public:
	explicit Time_measurement(bool enable_time_measurement = false);
	~Time_measurement();

	void tick(const std::string &timer_name);
	void tock(const std::string &timer_name);

	bool empty() const;

	YAML::Node emit() const override;
	void load(const YAML::Node &node) override;
private:
	bool enabled;
	std::unordered_map<std::string, boost::timer::cpu_timer> timers;
};

}
}
}

YAML_CONVERT_IMPL(fast::msg::migfra::Time_measurement)

#endif
