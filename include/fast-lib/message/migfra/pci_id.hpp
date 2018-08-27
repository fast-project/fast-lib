/*
 * This file is part of migration-framework.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#ifndef FAST_MESSAGE_MIGFRA_PCI_ID_HPP
#define FAST_MESSAGE_MIGFRA_PCI_ID_HPP

#include <fast-lib/serializable.hpp>

#include <ostream>
#include <string>

namespace fast {
namespace msg {
namespace migfra {

/**
 * \brief Contains the vendor id and device id to identify a PCI device type.
 */
struct PCI_id :
	public fast::Serializable
{
	using vendor_t = unsigned short;
	using device_t = unsigned short;

	/**
 	 * \brief Construct PCI_id.
 	 */
	PCI_id(vendor_t vendor, device_t device);
	/**
 	 * \brief Default constructor.
 	 */
	PCI_id() = default;
	/**
 	 * \brief Default copy constructor.
 	 */
	PCI_id(const PCI_id &) = default;
	/**
 	 * \brief Default assignment.
 	 */
	PCI_id & operator=(const PCI_id &) = default;

	/**
 	 * \brief Check if two PCI_ids are equal.
 	 */
	bool operator==(const PCI_id &rhs) const;
	/**
 	 * \brief Convert vendor part to hexadecimal format.
 	 */
	std::string vendor_hex() const;
	/**
 	 * \brief Convert device part to hexadecimal format.
 	 */
	std::string device_hex() const;
	/**
 	 * \brief Convert to formatted string.
 	 */
	std::string str() const;

	/**
 	 * \brief Serialize to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Initialize from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief vendor ID part.
 	 */
	vendor_t vendor;
	/**
 	 * \brief device ID part.
 	 */
	device_t  device;
};

std::ostream & operator<<(std::ostream &os, const PCI_id &rhs);

}
}
}

YAML_CONVERT_IMPL(fast::msg::migfra::PCI_id)

// std::hash specialization to make PCI_id a valid key for unordered_map.
namespace std
{
	template<> struct hash<fast::msg::migfra::PCI_id>
	{
		typedef fast::msg::migfra::PCI_id argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const
		{
			const result_type h1(std::hash<argument_type::vendor_t>()(s.vendor));
			const result_type h2(std::hash<argument_type::device_t>()(s.device));
			return h1 ^ (h2 << 1);
		}
	};
}

#endif
