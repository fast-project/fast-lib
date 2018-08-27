/*
 * This file is part of migration-framework.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#ifndef FAST_LIB_MESSAGE_MIGFRA_TASK_HPP
#define FAST_LIB_MESSAGE_MIGFRA_TASK_HPP

#include <fast-lib/message/migfra/ivshmem.hpp>
#include <fast-lib/message/migfra/pci_id.hpp>
#include <fast-lib/message/migfra/time_measurement.hpp>
#include <fast-lib/optional.hpp>
#include <fast-lib/serializable.hpp>

#include <memory>
#include <string>
#include <vector>

namespace fast {
namespace msg {
namespace migfra {

/**
 * \brief An abstract struct to provide an interface for a Task.
 */
struct Task :
	public fast::Serializable
{
	/**
 	 * \brief Construct an empty Task.
 	 */
	Task();
	/**
	 * \brief Constructor for Task.
	 *
	 * \param concurrent_execution Execute Task in dedicated thread.
	 * \param time_measurement Measure execution time and send in Result.
	 */
	Task(bool concurrent_execution, bool time_measurement = false);
	/**
 	 * \brief Default destructor.
 	 */
	virtual ~Task() = default;

	/**
 	 * \brief Serialize this Task to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Deserialize this Task from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief Flag to enable threaded rather than serial execution of this task by the Migration Framework.
 	 */
	Optional<bool> concurrent_execution;
	/**
 	 * \brief Enables time measurement in the Migration Framework.
 	 */
	Optional<bool> time_measurement;
	/**
 	 * \brief Denotes the driver of the Hypervisor to be used (e.g. "qemu", "lxctools" for libvirt)
 	 */
	Optional<std::string> driver;
};

/**
 * \brief A struct containing tasks.
 *
 * Contains several Tasks and executes those.
 * Task_handler will call execute method to execute the task.
 */
struct Task_container :
	public fast::Serializable
{
	struct no_task_exception :
		public std::runtime_error
	{
		no_task_exception(const std::string &str);
	};

	/**
	 * \brief Generate trivial default constructor.
	 *
	 * Constructs a Task_container without tasks.
	 * The execute method will return immediatly on a such constructed Task_container.
	 */
	Task_container();
	/**
	 * \brief Constructor for Task_container.
	 *
	 * \param tasks The tasks to execute.
	 * \param concurrent_execution Create and wait on tasks to be finished in dedicated thread.
	 */
	Task_container(std::vector<std::shared_ptr<Task>> tasks, bool concurrent_execution, std::string id = "");

	/**
 	 * \brief Serialize all Tasks to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Initialize tasks from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief vector of tasks.
 	 */
	std::vector<std::shared_ptr<Task>> tasks;
	/**
 	 * \brief Denotes the execution of tasks in separate threads rather than sequential execution.
 	 */
	Optional<bool> concurrent_execution;
	/**
 	 * \brief The id may be used to track tasks/messages.
 	 */
	Optional<std::string> id;

	/**
	 * \brief Get readable type of tasks.
	 *
	 * Returned type is the same format as in YAML (task:/result:).
	 * \param enable_result_format Set to true if type should be stored in Result, else Task format is used.
	 */
	std::string type(bool enable_result_format = false) const;
};

/**
 * \brief Task to start a single domain.
 */
struct Start :
	public Task
{
	/**
 	 * \brief Construct empty Start task.
 	 */
	Start();
	/**
	 * \brief Constructor for Start task.
	 *
	 * \param vm_name The name of the domain to start.
	 * \param vcpus The number of virtual cpus to assign to the domain.
	 * \param memory The ram to assign to the domain in MiB.
 	 * \param pci_ids A vector of PCI IDs for which devices should be searched and attached.
	 * \param concurrent_execution Execute this Task in dedicated thread.
	 */
	Start(std::string vm_name, unsigned int vcpus, unsigned long memory, std::vector<PCI_id> pci_ids, bool concurrent_execution);
	/**
 	 * \brief Constructor for Start task using xml.
 	 *
 	 * \param xml A string containing the xml description of the vm/container.
 	 * \param pci_ids A vector of PCI IDs for which devices should be searched and attached.
	 * \param concurrent_execution Execute this Task in dedicated thread.
 	 */
	Start(std::string xml, std::vector<PCI_id> pci_ids, bool concurrent_execution);

	/**
 	 * \brief Serialize Start task to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Initialize Start task from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief Name of the domain.
 	 */
	Optional<std::string> vm_name;
	/**
 	 * \brief Number of CPUs assigned to the domain.
 	 */
	Optional<unsigned int> vcpus;
	/**
 	 * \brief Amount of memory assigned to the domain in MiB.
 	 */
	Optional<unsigned long> memory;
	/**
 	 * \brief Used by ponci hypervisor.
 	 */
	Optional<std::vector<std::vector<unsigned int>>> memnode_map;
	/**
 	 * \brief A vector of PCI IDs for which devices should be searched and attached.
 	 */
	std::vector<PCI_id> pci_ids;
	/**
 	 * \brief Description of the domain using XML.
 	 */
	Optional<std::string> xml;
	/**
 	 * \brief ivshmem device to assign to the domain.
 	 */
	Optional<Device_ivshmem> ivshmem;
	/**
 	 * \brief Create a transient domain which ceases to exist after shutdown.
 	 */
	Optional<bool> transient;
	/**
 	 * \brief Map of how vcpus are assigned to physical cpus.
 	 */
	Optional<std::vector<std::vector<unsigned int>>> vcpu_map;
	/**
 	 * \brief Enable to test if the domain finished starting by probing via ssh.
 	 */
	Optional<bool> probe_with_ssh;
	/**
 	 * \brief Used to provide a hostname to probe an ssh connection after startup.
 	 */
	Optional<std::string> probe_hostname;
};

/**
 * \brief Task to stop a single virtual machine.
 */
struct Stop :
	public Task
{
	/**
 	 * \brief Construct empty Stop task.
 	 */
	Stop();
	/**
	 * \brief Constructor for Stop task.
	 *
	 * \param vm_name The name of the virtual machine to stop.
	 * \param force Force stopping the domain by using destroy instead of shutdown.
	 * \param undefine Undefine the domain after stopping.
	 * \param concurrent_execution Execute this Task in dedicated thread.
	 */
	Stop(std::string vm_name, bool force, bool undefine, bool concurrent_execution);

	/**
 	 * \brief Serialize Stop task to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Initialize Stop task from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief Name of the domain.
 	 */
	Optional<std::string> vm_name;
	/**
 	 * \brief Used to stop all domains which names are matched by the regex.
 	 */
	Optional<std::string> regex;
	/**
 	 * \brief Forced shutdown of domain.
 	 */
	Optional<bool> force;
	/**
 	 * \brief Undefine domain after shutdown.
 	 */
	Optional<bool> undefine;
};

/**
 * \brief Used for the swap-with option of the migrate task.
 */
struct Swap_with :
	public fast::Serializable
{
	/**
 	 * \brief Construct empty Swap_with object.
 	 */
	Swap_with();

	/**
 	 * \brief Serialize Swap_with object to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Initialize Swap_with object from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief Name of the domain to swap with.
 	 */
	std::string vm_name;
	/**
 	 * \brief Number of pscom processes to suspend.
 	 */
	Optional<std::string> pscom_hook_procs;
	/**
 	 * \brief Map of how vcpus are assigned to physical cpus.
 	 */
	Optional<std::vector<std::vector<unsigned int>>> vcpu_map;
};


/**
 * \brief Task to migrate a domain.
 */
struct Migrate :
	public Task
{
	/**
 	 * \brief Construct empty Migrate task.
 	 */
	Migrate();
	/**
	 * \brief Constructor for Migrate task.
	 *
	 * \param vm_name The name of the domain to migrate.
	 * \param dest_hostname The name of the host to migrate to.
	 * \param migration_type Option to enable live migration.
	 * \param rdma_migration Option to enable rdma migration.
	 * \param concurrent_execution Execute this Task in dedicated thread.
	 * \param pscom_hook_procs Number of processes to suspend during migration.
	 * \param time_measurement Measure execution time and send in Result.
	 */
	Migrate(std::string vm_name, std::string dest_hostname, std::string migration_type, bool rdma_migration, bool concurrent_execution, unsigned int pscom_hook_procs, bool time_measurement);
	/**
	 * \brief Constructor for Migrate task.
	 *
	 * \param vm_name The name of the domain to migrate.
	 * \param dest_hostname The name of the host to migrate to.
	 * \param migration_type Option to enable live migration.
	 * \param rdma_migration Option to enable rdma migration.
	 * \param concurrent_execution Execute this Task in dedicated thread.
	 * \param pscom_hook_procs Number of processes to suspend during migration as string or "auto" for auto detection.
	 * \param time_measurement Measure execution time and send in Result.
	 */
	Migrate(std::string vm_name, std::string dest_hostname, std::string migration_type, bool rdma_migration, bool concurrent_execution, std::string pscom_hook_procs, bool time_measurement);

	/**
 	 * \brief Serialize Migrate task to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Initialize Migrate task from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief Name of the domain to Migrate.
 	 */
	std::string vm_name;
	/**
 	 * \brief Name of the host to migrate to.
 	 */
	std::string dest_hostname;
	/**
 	 * \brief Number of times migration should be retried on failure.
 	 */
	Optional<unsigned int> retry_counter;
	/**
 	 * \brief Type of migration (e.g. cold/warm/live)
 	 */
	Optional<std::string> migration_type;
	/**
 	 * \brief Enable migration via RDMA.
 	 */
	Optional<bool> rdma_migration;
	/**
 	 * \brief Number of pscom processes to suspend during migration.
 	 */
	Optional<std::string> pscom_hook_procs;
	/**
 	 * \brief Type of transport (e.g. ssh or tcp).
 	 */
	Optional<std::string> transport;
	/**
 	 * \brief Swap with domain on destination.
 	 */
	Optional<Swap_with> swap_with;
	/**
 	 * \brief Map of how vcpus are assigned to physical cpus on destination.
 	 */
	Optional<std::vector<std::vector<unsigned int>>> vcpu_map;
};

/**
 * \brief Task to evacuate all domains from a node.
 */
struct Evacuate :
	public Task
{
	/**
 	 * \brief Construct empty Evacuate task.
 	 */
	Evacuate();
	/**
	 * \brief Constructor for Evacuate task.
	 *
	 * \param destinations List of hosts to migrate to.
	 * \param mode Evacuation mode (auto/compact/scatter).
	 * \param overbooking Allows overbooking of destination nodes.
	 * \param migration_type Option to enable live migration.
	 * \param rdma_migration Option to enable rdma migration.
	 * \param concurrent_execution Execute this Task in dedicated thread.
	 * \param pscom_hook_procs Number of processes to suspend during migration as string or "auto" for auto detection.
	 * \param time_measurement Measure execution time and send in Result.
	 */
	Evacuate(std::vector<std::string> destinations, std::string mode, bool overbooking, std::string migration_type, bool rdma_migration, bool concurrent_execution, std::string pscom_hook_procs, bool time_measurement);

	YAML::Node emit() const override;
	void load(const YAML::Node &node) override;

	/**
	 * \brief List of hosts to migrate to.
	 */
	std::vector<std::string> destinations;
	/**
 	 * \brief Evacuation mode (auto/compact/scatter).
 	 */
	Optional<std::string> mode;
	/**
	 * \brief Allows overbooking of destination nodes.
	 */
	Optional<bool> overbooking;
	/**
 	 * \brief Number of retry attempts on failure.
 	 */
	Optional<unsigned int> retry_counter;
	/**
	 * \brief Option to enable live migration.
	 */
	Optional<std::string> migration_type;
	/**
	 * \brief Option to enable rdma migration.
	 */
	Optional<bool> rdma_migration;
	/**
	 * \brief Number of processes to suspend during migration as string or "auto" for auto detection.
	 */
	Optional<std::string> pscom_hook_procs;
	/**
 	 * \brief Type of transport (e.g. ssh or tcp).
 	 */
	Optional<std::string> transport;

	/**
 	 * \brief Used internally by migration framework
 	 */
	Optional<std::string> vm_name;
};

/**
 * \brief Task to repin vcpus of a domain
 */
struct Repin :
	public Task
{
	/**
 	 * \brief Construct empty Repin task.
 	 */
	Repin();
	/**
 	 * \brief Construct Repin task.
 	 */
	Repin(std::string vm_name, std::vector<std::vector<unsigned int>> vcpu_map, bool concurrent_execution = true);

	/**
 	 * \brief Serialize Repin task to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Initialize Repin task from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief Name of the domain to repin.
 	 */
	std::string vm_name;
	/**
 	 * \brief Map of how vcpus are assigned to physical cpus on destination.
 	 */
	std::vector<std::vector<unsigned int>> vcpu_map;
};

/**
 * \brief Task to suspend the execution of a single domain.
 */
struct Suspend :
	public Task
{
	/**
 	 * \brief Construct empty Suspend task.
 	 */
	Suspend();
	/**
	 * \brief Constructor for Suspend task.
	 *
	 * \param vm_name The name of the domain to stop.
	 * \param concurrent_execution Execute this Task in dedicated thread.
	 */
	Suspend(std::string vm_name, bool concurrent_execution);

	/**
 	 * \brief Serialize Suspend task to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Initialize Suspend task from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief Name of the domain to suspend.
 	 */
	std::string vm_name;
};

/**
 * \brief Task to resume the execution of a single domain.
 */
struct Resume :
	public Task
{
	/**
 	 * \brief Construct empty Resume task.
 	 */
	Resume();
	/**
	 * \brief Constructor for Suspend task.
	 *
	 * \param vm_name The name of the domain to stop.
	 * \param concurrent_execution Execute this Task in dedicated thread.
	 */
	Resume(std::string vm_name, bool concurrent_execution);

	/**
 	 * \brief Serialize Resume task to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Initialize Resume task from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief Name of the domain to resume.
 	 */
	std::string vm_name;
};

/**
 * \brief Task to quit migfra.
 */
struct Quit :
	public Task
{
};

}
}
}

// Adds support for direct yaml conversion, e.g., a vector of these types:
// 	std::vector<T> objs;
// 	fast::load(objs, node["list"]);
YAML_CONVERT_IMPL(fast::msg::migfra::Task)
YAML_CONVERT_IMPL(fast::msg::migfra::Task_container)
YAML_CONVERT_IMPL(fast::msg::migfra::Start)
YAML_CONVERT_IMPL(fast::msg::migfra::Stop)
YAML_CONVERT_IMPL(fast::msg::migfra::Swap_with)
YAML_CONVERT_IMPL(fast::msg::migfra::Migrate)
YAML_CONVERT_IMPL(fast::msg::migfra::Evacuate)
YAML_CONVERT_IMPL(fast::msg::migfra::Repin)
YAML_CONVERT_IMPL(fast::msg::migfra::Suspend)
YAML_CONVERT_IMPL(fast::msg::migfra::Resume)
YAML_CONVERT_IMPL(fast::msg::migfra::Quit)

#endif
