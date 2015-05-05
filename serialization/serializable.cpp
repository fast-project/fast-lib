#include "serializable.hpp"

namespace fast
{
	std::string Serializable::to_string() const
	{
		return "---\n" + YAML::Dump(emit()) + "\n---";
	}
	void Serializable::from_string(const std::string &str)
	{
		load(YAML::Node(str));
	}
}
