#include "mqtt_communicator.hpp"
#include "serializable.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

// Inherit from fast::Serializable
struct Data : fast::Serializable
{
	std::string task;
	unsigned int id;
	std::vector<std::string> vms;

	// Override these two methods to state which members should be serialized
	YAML::Node emit() const override;
	void load(const YAML::Node &node) override;
};

int main(int argc, char *argv[])
{
	(void)argc;(void)argv;
	try {
		mosqpp::lib_init();

		std::string id = "test-id";
		std::string subscribe_topic = "topic1";
		std::string publish_topic = "topic1";
		std::string host = "localhost";
		int port = 1883;
		int keepalive = 60;

		MQTT_communicator comm(id, subscribe_topic, publish_topic, host, port, keepalive);
		{
			Data d;
			d.task = "greet";
			d.id = 42;
			d.vms = {"vm-name-1", "vm-name-2"};

			comm.send_message(d.to_string());
		}
		{
			Data d;
			d.from_string(comm.get_message());
			std::cout << "Message received." << std::endl;
			std::cout << d.to_string() << std::endl;
		}
	} catch (const std::exception &e) {
		std::cout << "Exception: " << e.what();
	}
	mosqpp::lib_cleanup();
	return EXIT_SUCCESS;
}

YAML::Node Data::emit() const
{
	YAML::Node node;
	node["task"] = task;
	node["data-id"] = id;
	node["vms"] = vms;
	return node;
}

void Data::load(const YAML::Node &node)
{
	task = node["task"].as<std::string>("idle"); // "idle" is the default value if yaml does not contain the node "task"
	id = node["data-id"].as<unsigned int>();
	vms = node["vms"].as<decltype(vms)>(); // decltype: same type as vms
}
