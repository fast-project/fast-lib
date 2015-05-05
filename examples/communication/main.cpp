#include "mqtt_communicator.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

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
		comm.send_message("Hallo Welt");
		std::string msg = comm.get_message();
		std::cout << "Message received:" << msg << std::endl;
	} catch (const std::exception &e) {
		std::cout << "Exception: " << e.what();
	}
	mosqpp::lib_cleanup();
	return EXIT_SUCCESS;
}
