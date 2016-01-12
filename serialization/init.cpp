#include "init.hpp"

namespace fast {
namespace message {

YAML::Node init::emit() const {
	YAML::Node node;
	node["task"] = "init";
	node["hostname"] = hostname;
	return node;
}

void init::load(const YAML::Node &node) { fast::load(hostname, node["hostname"]); }
}
}
