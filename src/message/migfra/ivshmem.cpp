/*
 * This file is part of migration-framework.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#include <fast-lib/message/migfra/ivshmem.hpp>

namespace fast {
namespace msg {
namespace migfra {

Device_ivshmem::Device_ivshmem() :
	size(0),
	path("path")
{
}

YAML::Node Device_ivshmem::emit() const
{
	YAML::Node node;
	node["id"] = id;
	fast::yaml::merge_node(node, path.emit());
	node["size"] = size;
	return node;
}

void Device_ivshmem::load(const YAML::Node &node)
{
	fast::load(id, node["id"]);
	path.load(node);
	fast::load(size, node["size"]);
}

}
}
}
