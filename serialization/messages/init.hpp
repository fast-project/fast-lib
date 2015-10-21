/*
 * This file is part of fast-lib.
 * Copyright (C) 2015 Technische Universität München - LRR
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#pragma once

#include <string>
#include "../serializable.hpp"

namespace fast {
namespace message {

/**
 * * topic: fast/agent/<hostname>/status
 * * payload:
 *   task: init
 *   source: <hostname>
 */

struct init : public fast::Serializable {
	std::string hostname;

	init() = default;
	init(const std::string _hostname) : hostname(_hostname){};

	YAML::Node emit() const override;
	void load(const YAML::Node &node) override;
};
}
}
