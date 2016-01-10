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
 *  task: stop monitoring
 *  job-description:
 *    job-id: <job id>
 *    process-id: <process id of the vm>
 */

struct job_description : public fast::Serializable {
	std::string job_id;
	unsigned int process_id;

	job_description() = default;
	job_description(const std::string &_job_id, unsigned int _process_id) : job_id(_job_id), process_id(_process_id) {}

	YAML::Node emit() const override;
	void load(const YAML::Node &node) override;
};

struct stop_monitoring : public fast::Serializable {

	job_description job_desc;

	stop_monitoring() = default;
	stop_monitoring(const std::string &_job_id, unsigned int _process_id) : job_desc(_job_id, _process_id){};

	YAML::Node emit() const override;
	void load(const YAML::Node &node) override;
};
}
}
