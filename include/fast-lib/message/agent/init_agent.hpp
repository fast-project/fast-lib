/*
 * This file is part of fast-lib.
 * Copyright (C) 2015 Technische Universität München - LRR
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#ifndef FAST_LIB_MESSAGE_AGENT_INIT_AGENT
#define FAST_LIB_MESSAGE_AGENT_INIT_AGENT

#include <fast-lib/serializable.hpp>

#include <map>

namespace fast {
namespace message {
namespace agent {

/**
 * topic: fast/agent/<hostname>/task
 * Payload
 * task: init agent
 * KPI:
 *  categories:
 *   - energy consumption: <energy>
 *   - compute intensity: <high,medium,low>
 *   - IO intensity: <high,medium,low>
 *   - communication intensity (network): <high,medium,low>
 *   - expected runtime: <high,medium,low>
 *  repeat: <number in seconds how often the KPIs are reported>
 */

struct kpis : public fast::Serializable
{
	kpis() = default;
	kpis(std::map<std::string, std::string> categories, unsigned int kpi_repeat);

	YAML::Node emit() const override;
	void load(const YAML::Node &node) override;

	std::map<std::string, std::string> categories;
	unsigned int kpi_repeat;
};

struct init_agent : public fast::Serializable
{
	init_agent() = default;
	init_agent(std::map<std::string, std::string> categories, unsigned int kpi_repeat);

	YAML::Node emit() const override;
	void load(const YAML::Node &node) override;

	kpis KPIs;
};

}
}
}

#endif
