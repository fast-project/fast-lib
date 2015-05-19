/*
 * This file is part of fast-lib.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

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

		std::cout << "Starting communicator." << std::endl;
		fast::MQTT_communicator comm(id, subscribe_topic, publish_topic, host, port, keepalive);
		std::cout << "Communicator started." << std::endl << std::endl;
		
		// Sending and receiving
		{
			std::cout << "Sending message." << std::endl;
			comm.send_message("Hallo Welt");
			std::cout << "Waiting for message." << std::endl;
			std::string msg = comm.get_message();
			std::cout << "Message received: " << msg << std::endl << std::endl;
		}

		// Receiving with timeout
		{
			try {
				std::cout << std::endl << "Sending message." << std::endl;
				comm.send_message("Hallo Welt");
				std::cout << "Waiting for message. (3s timeout)" << std::endl;
				std::string msg = comm.get_message(std::chrono::seconds(3));
				std::cout << "Message received: " << msg << std::endl << std::endl;

				std::cout << std::endl << "No message this time." << std::endl;
				std::cout << "Waiting for message. (3s timeout)" << std::endl;
				msg = comm.get_message(std::chrono::seconds(3));
				std::cout << "Message received: " << msg << std::endl;
			} catch (const std::runtime_error &e) {
				std::cout << "Exception: " << e.what() << std::endl;
			}
			std::cout << std::endl;
		}
	} catch (const std::exception &e) {
		std::cout << "Exception: " << e.what() << std::endl;
		mosqpp::lib_cleanup();
		return EXIT_FAILURE;
	}
	mosqpp::lib_cleanup();
	return EXIT_SUCCESS;
}
