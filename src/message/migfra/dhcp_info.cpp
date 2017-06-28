/*
 * This file is part of migration-framework.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#include <fast-lib/message/migfra/dhcp_info.hpp>

#include <sstream>
#include <iomanip>

namespace fast {
namespace msg {
namespace migfra {


//
// DHCP_info implementation
//

DHCP_info::DHCP_info(std::string hostname, std::string mac) :
	hostname(hostname), mac(mac)
{
}

std::string DHCP_info::str() const
{
	return "(" + hostname + "," + mac + ")";
}

YAML::Node DHCP_info::emit() const
{
	YAML::Node node;
	node["hostname"] = hostname;
	node["mac"] = mac;
	return node;
}

void DHCP_info::load(const YAML::Node &node)
{
	fast::load(hostname, node["hostname"]);
	fast::load(mac, node["mac"]);
}

std::ostream & operator<<(std::ostream &os, const DHCP_info &rhs)
{
	return os << rhs.str();
}

}
}
}
