#include "stop_monitor.hpp"

namespace fast {
namespace message {

YAML::Node job_description::emit() const {
	YAML::Node node;
	node["job-id"] = job_id;
	node["process-id"] = process_id;
	return node;
}

void job_description::load(const YAML::Node &node) {
	fast::load(job_id, node["job-id"]);
	fast::load(process_id, node["process-id"]);
}

YAML::Node stop_monitoring::emit() const {
	YAML::Node node;
	node["task"] = "stop monitoring";
	node["job-description"] = job_description.emit();
	return node;
}

void stop_monitoring::load(const YAML::Node &node) {
	fast::load(job_description.job_id, node["job-description"]["job-id"]);
	fast::load(job_description.process_id, node["job-description"]["process-id"]);
}
}
}
