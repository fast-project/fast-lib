#ifndef FAST_MESSAGE_MIGFRA_DHCP_INFO_HPP
#define FAST_MESSAGE_MIGFRA_DHCP_INFO_HPP

#include <fast-lib/serializable.hpp>

#include <ostream>
#include <string>

namespace fast {
namespace msg {
namespace migfra {

// Contains the vendor id and device id to identify a PCI device type.
struct DHCP_info :
	public fast::Serializable
{
	DHCP_info(std::string hostname, std::string mac);
	// Default (copy-)constructor and assignment for use in std::vector.
	DHCP_info() = default;
	DHCP_info(const DHCP_info &) = default;
	DHCP_info & operator=(const DHCP_info &) = default;

	std::string str() const;

	YAML::Node emit() const override;
	void load(const YAML::Node &node) override;

	std::string hostname;
	std::string mac;
};

std::ostream & operator<<(std::ostream &os, const DHCP_info &rhs);

}
}
}

YAML_CONVERT_IMPL(fast::msg::migfra::DHCP_info)

#endif
