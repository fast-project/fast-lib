/*
 * This file is part of migration-framework.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#ifndef FAST_LIB_MESSAGE_MIGFRA_RESULT_HPP
#define FAST_LIB_MESSAGE_MIGFRA_RESULT_HPP

#include <fast-lib/message/migfra/time_measurement.hpp>
#include <fast-lib/serializable.hpp>

#include <vector>

namespace fast {
namespace msg {
namespace migfra {

/**
 * \brief Represents the result of a Task.
 *
 * Results are sent back packed in a vector representing all results of a Task.
 */
struct Result : public fast::Serializable
{
	/**
 	 * \brief Default construct Result task.
 	 */
	Result() = default;
	/**
 	 * \brief Construct Result task.
 	 * 
 	 * \param vm_name Name of the domain the result concerns.
 	 * \param status String of the result status.
 	 * \param details String containing further information especially on failure.
 	 */
	Result(std::string vm_name, std::string status, std::string details = "");
	/**
 	 * \brief Construct Result task with time measurement.
 	 *
 	 * \param vm_name Name of the domain the result concerns.
 	 * \param status String of the result status.
 	 * \param time_measurement Pass Time_measurement object containing timing of phases.
 	 * \param details String containing further information especially on failure.
 	 */
	Result(std::string vm_name, std::string status, Time_measurement time_measurement, std::string details = "");

	/**
 	 * \brief Serialize Result to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Initialize Result from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief Name of the domain the result concerns.
 	 */
	std::string vm_name;
	/**
 	 * String of the result status.
 	 */
	std::string status;
	/**
 	 * \brief String containing further information especially on failure.
 	 */
	std::string details;
	/**
 	 * \brief Time_measurement object containing timing of phases.
 	 */
	Time_measurement time_measurement;
};

/**
 * \brief Contains a vector of results and enables proper YAML conversion.
 *
 * A Result_container is neccesary to convert results to YAML in the right format.
 * Represents result of a Task_container.
 */
struct Result_container : public fast::Serializable
{
	/**
 	 * \brief Default construct Result_container.
 	 */
	Result_container() = default;
	/**
 	 * \brief Construct Result_container from YAML string.
 	 *
 	 * \param yaml_str String using YAML to describe Result.
 	 */
	Result_container(const std::string &yaml_str);
	/**
 	 * \brief Construct Result_container.
 	 *
 	 * \param title Type of tasks the results are from.
 	 * \param results Vector of results.
 	 * \param id Used to track tasks and corresponding results.
 	 */
	Result_container(std::string title, std::vector<Result> results, std::string id = "");

	/**
 	 * \brief Serialize all Results to YAML.
 	 */
	YAML::Node emit() const override;
	/**
 	 * \brief Initialize Results from YAML.
 	 */
	void load(const YAML::Node &node) override;

	/**
 	 * \brief Type of tasks the results are from.
 	 */
	std::string title;
	/**
 	 * \brief Vector of results.
 	 */
	std::vector<Result> results;
	/**
 	 * \brief Used to track tasks and corresponding results.
 	 */
	std::string id;
};

}
}
}

YAML_CONVERT_IMPL(fast::msg::migfra::Result)
YAML_CONVERT_IMPL(fast::msg::migfra::Result_container)

#endif
