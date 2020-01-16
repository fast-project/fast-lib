/*
 * This file is part of migration-framework.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#ifndef FAST_MESSAGE_MIGFRA_PCI_ADDR_HPP
#define FAST_MESSAGE_MIGFRA_PCI_ADDR_HPP

#include <fast-lib/serializable.hpp>

#include <ostream>
#include <string>

namespace fast {
	namespace msg {
		namespace migfra {

			/**
			 * \brief Contains the vendor id and device id to identify a PCI device type.
			 */
			struct PCI_addr :
				public fast::Serializable
			{
				uint16_t domain; //probably not needed
				uint8_t bus;
				uint8_t device;
				uint8_t funct;

				PCI_addr(uint16_t domain, uint8_t bus , uint8_t device, uint8_t funct);
				PCI_addr() = default;
				PCI_addr(const PCI_addr &) = default;
				PCI_addr & operator=(const PCI_addr &) = default;
				bool operator==(const PCI_addr &rhs) const;
				std::string str() const;

				YAML::Node emit() const override;
				void load(const YAML::Node &node) override;
			};

			std::ostream & operator<<(std::ostream &os, const PCI_addr &rhs);

		}
	}
}

YAML_CONVERT_IMPL(fast::msg::migfra::PCI_addr)

	// std::hash specialization to make PCI_id a valid key for unordered_map.
	namespace std
{
	template<> struct hash<fast::msg::migfra::PCI_addr>
	{
		typedef fast::msg::migfra::PCI_addr argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const
		{
			return std::hash<uint32_t>()((s.bus << 8) + (s.device << 5) + (s.funct));
		}
	};
}

#endif
