/*
 * This file is part of fast-lib.
 * Copyright (C) 2015 Technische Universität München - LRR
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#pragma once

#include <string>
#include <map>
#include "serializable.hpp"

namespace fast {
namespace message {

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

struct kpis : public fast::Serializable {
	kpis() = default;
	kpis(const std::map<std::string, std::string> &_categories, unsigned int _kpi_repeat)
		: categories(_categories), kpi_repeat(_kpi_repeat) {}

	std::map<std::string, std::string> categories;
	unsigned int kpi_repeat;

	YAML::Node emit() const override;
	void load(const YAML::Node &node) override;
};

struct init_agent : public fast::Serializable {
	kpis KPIs;

	init_agent() = default;
	init_agent(const std::map<std::string, std::string> &_categories, unsigned int _kpi_repeat)
		: KPIs(_categories, _kpi_repeat){};

	YAML::Node emit() const override;
	void load(const YAML::Node &node) override;
};
}
}
