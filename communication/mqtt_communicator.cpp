/*
 * This file is part of fast-lib.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#include "mqtt_communicator.hpp"

#include <stdexcept>
#include <cstdlib>
#include <thread>
#include <iostream>

namespace fast {

/// Helper function to make error codes human readable.
std::string mosq_err_string(const std::string &str, int code)
{
	return str + mosqpp::strerror(code);
}

MQTT_communicator::MQTT_communicator(const std::string &id, 
				     const std::string &subscribe_topic,
				     const std::string &publish_topic,
				     const std::string &host, 
				     int port,
				     int keepalive,
				     const std::chrono::duration<double> &timeout) :
	mosqpp::mosquittopp(id.c_str()),
	publish_topic(publish_topic),
	connected(false)
{
	// Initialize mosquitto library if no other instance did
	if (ref_count++ == 0)
		mosqpp::lib_init();

	int ret;
	// Start threaded mosquitto loop
	if ((ret = loop_start()) != MOSQ_ERR_SUCCESS)
		throw std::runtime_error(mosq_err_string("Error starting mosquitto loop: ", ret));
	// Connect to MQTT broker. Uses condition variable that is set in on_connect, because
	// (re-)connect returning MOSQ_ERR_SUCCESS does not guarantee an fully established connection.
	{
		auto start = std::chrono::high_resolution_clock::now();
		ret = connect(host.c_str(), port, keepalive);
		while (ret != MOSQ_ERR_SUCCESS) {
			std::cout << mosq_err_string("Failed connecting to MQTT broker: ", ret) << std::endl;
			if (std::chrono::high_resolution_clock::now() - start > timeout)
				throw std::runtime_error("Timeout while trying to connect to MQTT broker.");
			std::this_thread::sleep_for(std::chrono::seconds(1));
			ret = reconnect();
		}
		std::unique_lock<std::mutex> lock(connected_mutex);
		auto time_left = timeout - (std::chrono::high_resolution_clock::now() - start);
		// Branch between wait and wait_for because if time_left is max wait_for does not work 
		// (waits until now + max -> overflow?).
		if (time_left != std::chrono::duration<double>::max()) {
			if (!connected_cv.wait_for(lock, time_left, [this]{return connected;}))
				throw std::runtime_error("Timeout while trying to connect to MQTT broker.");
		} else {
			connected_cv.wait(lock, [this]{return connected;});
		}
	}
	// Subscribe to default topic
	subscribe(nullptr, subscribe_topic.c_str(), 2);
}

MQTT_communicator::~MQTT_communicator()
{
	disconnect();
	loop_stop();
	if (--ref_count == 0)
		mosqpp::lib_cleanup();
}

void MQTT_communicator::on_connect(int rc)
{
	if (rc == 0) {
		{
			std::lock_guard<std::mutex> lock(connected_mutex);
			connected = true;
		}
		connected_cv.notify_one();
		std::cout << "Connection established." << std::endl;
	} else {
		std::cout << "Error on connect: " << mosqpp::connack_string(rc) << std::endl;
	}
}

void MQTT_communicator::on_disconnect(int rc)
{
	if (rc == 0) {
		std::cout << "Disconnected." << std::endl;
	} else {
		std::cout << mosq_err_string("Unexpected disconnect: ", rc) << std::endl;
	}
	std::lock_guard<std::mutex> lock(connected_mutex);
	connected = false;
}

void MQTT_communicator::on_message(const mosquitto_message *msg)
{
	mosquitto_message* buf = static_cast<mosquitto_message*>(std::malloc(sizeof(mosquitto_message)));
	if (!buf) {
		std::cout << "malloc failed allocating mosquitto_message." << std::endl;
	}
	std::lock_guard<std::mutex> lock(msg_queue_mutex);
	messages.push(buf);
	mosquitto_message_copy(messages.back(), msg);
	if (messages.size() == 1)
		msg_queue_empty_cv.notify_one();
}

void MQTT_communicator::send_message(const std::string &message)
{
	send_message(message, publish_topic);
}

void MQTT_communicator::send_message(const std::string &message, const std::string &topic)
{
	int ret = publish(nullptr, topic.c_str(), message.size(), message.c_str(), 2, false);
	if (ret != MOSQ_ERR_SUCCESS)
		throw std::runtime_error(mosq_err_string("Error sending message: ", ret));
}

std::string MQTT_communicator::get_message()
{
	std::unique_lock<std::mutex> lock(msg_queue_mutex);
	while (messages.empty())
		msg_queue_empty_cv.wait(lock);
	mosquitto_message *msg = messages.front();
	messages.pop();
	std::string buf(static_cast<char*>(msg->payload), msg->payloadlen);
	mosquitto_message_free(&msg);
	return buf;
}

std::string MQTT_communicator::get_message(const std::chrono::duration<double> &duration)
{
	std::unique_lock<std::mutex> lock(msg_queue_mutex);
	while (messages.empty())
		if (msg_queue_empty_cv.wait_for(lock, duration) == std::cv_status::timeout)
			throw std::runtime_error("Timeout while waiting for message.");
	mosquitto_message *msg = messages.front();
	messages.pop();
	std::string buf(static_cast<char*>(msg->payload), msg->payloadlen);
	mosquitto_message_free(&msg);
	return buf;
}

unsigned int MQTT_communicator::ref_count = 0;

} // namespace fast
