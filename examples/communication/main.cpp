/*
 * This file is part of fast-lib.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#include "communication/mqtt_communicator.hpp"

#include <boost/log/trivial.hpp>

#include <cstdlib>
#include <exception>
#include <thread>

int main(int argc, char *argv[])
{
	(void)argc;(void)argv;
	try {
		std::string id = "test-id";
		std::string subscribe_topic = "topic1";
		std::string publish_topic = "topic1";
		std::string host = "localhost";
		int port = 1883;
		int keepalive = 60;

		BOOST_LOG_TRIVIAL(info) << "Creating a MQTT_communicator for use in test cases.";
		fast::MQTT_communicator comm(id, subscribe_topic, publish_topic, host, port, keepalive, std::chrono::seconds(5));

		BOOST_LOG_TRIVIAL(info) << "Test of a second MQTT_communicator instance with random id.";
		{
			fast::MQTT_communicator comm("", subscribe_topic, publish_topic, host, port, keepalive, std::chrono::seconds(5));
		}
		
		BOOST_LOG_TRIVIAL(info) << "Send and receive test.";
		{
			// Expected: success
			comm.send_message("Hallo Welt");
			std::string msg = comm.get_message();
			if (msg != "Hallo Welt") {
				BOOST_LOG_TRIVIAL(debug) << "Received unexpected message.";
				BOOST_LOG_TRIVIAL(debug) << "Message contents:" << std::endl << msg;
				return EXIT_FAILURE;
			}
		}
		BOOST_LOG_TRIVIAL(info) << "Receive with timeout test.";
		{
			try {
				// Expected: success
				comm.send_message("Hallo Welt");
				BOOST_LOG_TRIVIAL(debug) << "Waiting for message. (3s timeout)";
				std::string msg = comm.get_message(std::chrono::seconds(3));
				if (msg != "Hallo Welt") {
					BOOST_LOG_TRIVIAL(debug) << "Received unexpected message.";
					BOOST_LOG_TRIVIAL(debug) << "Message contents:" << std::endl << msg;
					return EXIT_FAILURE;
				}

				// Expected: timeout
				BOOST_LOG_TRIVIAL(debug) << "No message this time.";
				BOOST_LOG_TRIVIAL(debug) << "Waiting for message. (3s timeout)";
				msg = comm.get_message(std::chrono::seconds(3));
				BOOST_LOG_TRIVIAL(debug) << "This output should not be reached due to timeout exception.";
				return EXIT_FAILURE;
			} catch (const std::runtime_error &e) {
				if (e.what() != std::string("Timeout while waiting for message.")) {
					BOOST_LOG_TRIVIAL(debug) << "Expected timeout exception but another exception was thrown.";
					return EXIT_FAILURE;
				}
			}
		}
		BOOST_LOG_TRIVIAL(info) << "Add and remove subscription test.";
		{
			// Expected: success
			BOOST_LOG_TRIVIAL(debug) << "Add subscription on \"topic2\".";
			comm.add_subscription("topic2");
			BOOST_LOG_TRIVIAL(debug) << "Sending message on \"topic2\".";
			comm.send_message("Hallo Welt", "topic2");
			BOOST_LOG_TRIVIAL(debug) << "Waiting for message on \"topic2\".";
			std::string msg = comm.get_message("topic2");
			BOOST_LOG_TRIVIAL(debug) << "Message received on \"topic2\". ";
			if (msg != "Hallo Welt") {
				BOOST_LOG_TRIVIAL(debug) << "Received unexpected message.";
				BOOST_LOG_TRIVIAL(debug) << "Message contents:" << std::endl << msg;
				return EXIT_FAILURE;
			}

			// Expected: missing subscription
			BOOST_LOG_TRIVIAL(debug) << "Remove subscription on \"topic2\".";
			comm.remove_subscription("topic2");
			try {
				BOOST_LOG_TRIVIAL(debug) << "Sending message on \"topic2\".";
				comm.send_message("Hallo Welt", "topic2");
				BOOST_LOG_TRIVIAL(debug) << "Waiting for message on \"topic2\".";
				std::string msg = comm.get_message("topic2", std::chrono::seconds(1));
				BOOST_LOG_TRIVIAL(debug) << "This output should not be reached due to topic-not-found exception.";
				return EXIT_FAILURE;
			} catch (const std::out_of_range &e) {
				if (e.what() != std::string("Topic not found in subscriptions.")) {
					BOOST_LOG_TRIVIAL(debug) << "Expected topic-not-found exception but another exception was thrown.";
					return EXIT_FAILURE;
				}
			}
		}
		BOOST_LOG_TRIVIAL(info) << "Add subscription with callback test.";
		{
			// Expected: success
			std::string msg;
			BOOST_LOG_TRIVIAL(debug) << "Add subscription on \"topic3\" with callback.";
			comm.add_subscription("topic3", [&msg](std::string received_msg) {
					BOOST_LOG_TRIVIAL(debug) << "Received in callback: " << received_msg;
					msg = std::move(received_msg);
			});
			BOOST_LOG_TRIVIAL(debug) << "Sending message on \"topic3\".";
			comm.send_message("Hallo Welt", "topic3");
			std::this_thread::sleep_for(std::chrono::seconds(1));
			if (msg != "Hallo Welt") {
				BOOST_LOG_TRIVIAL(debug) << "Received unexpected message.";
				BOOST_LOG_TRIVIAL(debug) << "Message contents:" << std::endl << msg;
				return EXIT_FAILURE;
			}
			BOOST_LOG_TRIVIAL(debug) << "Message received on \"topic3\".";

			// Expected: cannot get_message from subscription with callback
			try {
				BOOST_LOG_TRIVIAL(debug) << "Try to get message from topic with callback.";
				comm.get_message("topic3");
				BOOST_LOG_TRIVIAL(debug) << "This output should not be reached due to wrong-subscription-type exception.";
				return EXIT_FAILURE;
			} catch (const std::runtime_error &e) {
				if (e.what() != std::string("Error in get_message: This topic is subscribed with callback.")) {
					BOOST_LOG_TRIVIAL(debug) << "Expected wrong-subscription-type exception but another exception was thrown.";
					return EXIT_FAILURE;
				}
			}
			BOOST_LOG_TRIVIAL(debug) << "Remove subscription on \"topic2\"";
			comm.remove_subscription("topic3");
		}
		BOOST_LOG_TRIVIAL(info) << "Wildcard + test.";
		{
			// Expected: success
			BOOST_LOG_TRIVIAL(debug) << "Add subscription on \"A/+/B\".";
			comm.add_subscription("A/+/B");
			BOOST_LOG_TRIVIAL(debug) << "Sending message on \"A/C/B\".";
			comm.send_message("Hallo Welt", "A/C/B");
			BOOST_LOG_TRIVIAL(debug) << "Waiting for message on \"A/+/B\".";
			auto msg = comm.get_message("A/+/B", std::chrono::seconds(3));
			BOOST_LOG_TRIVIAL(debug) << "Message received on \"A/+/B\".";
			if (msg != "Hallo Welt") {
				BOOST_LOG_TRIVIAL(debug) << "Received unexpected message.";
				BOOST_LOG_TRIVIAL(debug) << "Message contents:" << std::endl << msg;
				return EXIT_FAILURE;
			}
			comm.remove_subscription("A/+/B");
		}
		BOOST_LOG_TRIVIAL(info) << "Wildcard # test.";
		{
			// Expected: success
			BOOST_LOG_TRIVIAL(debug) << "Add subscription on \"A/#\".";
			comm.add_subscription("A/#");
			BOOST_LOG_TRIVIAL(debug) << "Sending message on \"A/C/B\".";
			comm.send_message("Hallo Welt", "A/C/B");
			BOOST_LOG_TRIVIAL(debug) << "Waiting for message on \"A/#\".";
			auto msg = comm.get_message("A/#", std::chrono::seconds(3));
			BOOST_LOG_TRIVIAL(debug) << "Message received on \"A/#\".";
			if (msg != "Hallo Welt") {
				BOOST_LOG_TRIVIAL(debug) << "Received unexpected message.";
				BOOST_LOG_TRIVIAL(debug) << "Message contents:" << std::endl << msg;
				return EXIT_FAILURE;
			}
			comm.remove_subscription("A/#");
		}
	} catch (const std::exception &e) {
		BOOST_LOG_TRIVIAL(debug) << "Exception: " << e.what();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
