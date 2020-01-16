/*
 * This file is part of migration-framework.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#include <fast-lib/message/migfra/pci_addr.hpp>

#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm> // std::count

namespace fast {
	namespace msg {
		namespace migfra {

			// Converts integer type numbers to string in hex format.
			static std::string to_hex_string(unsigned int integer, int digits, bool show_base = true)
			{
				std::stringstream ss;
				ss << (show_base ? "0x" : "") << std::hex << std::setfill('0') << std::setw(digits) << integer;
				return ss.str();
			}


			PCI_addr::PCI_addr(uint16_t domain, uint8_t bus , uint8_t device, uint8_t funct) :
				domain(domain), bus(bus), device(device), funct(funct)
			{
			}

			bool PCI_addr::operator==(const PCI_addr &rhs) const
			{
				return domain == rhs.domain && bus == rhs.bus
					&& device == rhs.device && funct == rhs.funct;
			}

			std::string PCI_addr::str() const
			{
				return to_hex_string(device, 4, false) + ":" + to_hex_string(bus, 2, false) + "." 
					+ to_hex_string(device, 2, false) + "." + to_hex_string(funct, 1, false);
			}

			YAML::Node PCI_addr::emit() const
			{
				YAML::Node node;
				node["addr"] = str();
				return node;
			}

			void PCI_addr::load(const YAML::Node &node)
			{
				std::string pci_str = node["pci-addr"].as<std::string>();
				size_t token_pos;
				switch (std::count(pci_str.begin(), pci_str.end(), ':')) {
					case 1: 
						domain = 0;
						break;
					case 2: 
						token_pos = pci_str.find(":");
						domain = std::stoul(pci_str.substr(0, token_pos), nullptr, 16);
						pci_str.erase(0, pci_str.find(":") + 1); // Remove Domain
						break;
					default:
						throw std::runtime_error("Invalid PCI Address");
				}

				token_pos = pci_str.find(":");
				bus = std::stoul(pci_str.substr(0, token_pos), nullptr, 16);
				pci_str.erase(0, token_pos + 1); // Remove Bus

				token_pos = pci_str.find(".");
				device = std::stoul(pci_str.substr(0, token_pos), nullptr, 16);
				pci_str.erase(0, token_pos + 1); // Remove Device

				funct = std::stoul(pci_str, nullptr, 16);
			}

			std::ostream & operator<<(std::ostream &os, const PCI_addr &rhs)
			{
				return os << rhs.str();
			}

		}
	}
}
