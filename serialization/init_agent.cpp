#include "init_agent.hpp"

namespace fast {
namespace message {

YAML::Node kpis::emit() const {
	YAML::Node node;
	YAML::Node cat;
	for (auto &p : categories) {
		cat[p.first] = p.second;
	}
	node["categories"] = cat;
	node["repeat"] = kpi_repeat;
	return node;
}

void kpis::load(const YAML::Node &node) {
	fast::load(kpi_repeat, node["repeat"]);
	fast::load(categories, node["categories"]);
}

YAML::Node init_agent::emit() const {
	YAML::Node node;
	node["task"] = "init agent";
	node["KPI"] = KPIs.emit();
	return node;
}

void init_agent::load(const YAML::Node &node) {
	fast::load(KPIs.kpi_repeat, node["KPI"]["repeat"]);
	fast::load(KPIs.categories, node["KPI"]["categories"]);
}
}
}
