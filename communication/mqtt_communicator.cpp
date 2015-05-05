#include "mqtt_communicator.hpp"

#include <stdexcept>
#include <cstdlib>

MQTT_communicator::MQTT_communicator(const std::string &id, 
				     const std::string &subscribe_topic,
				     const std::string &publish_topic,
				     const std::string &host, 
				     int port,
				     int keepalive) :
	mosqpp::mosquittopp(id.c_str()),
	id(id), 
	subscribe_topic(subscribe_topic),
	publish_topic(publish_topic),
	host(host), 
	port(port),
	keepalive(keepalive)
{
	loop_start();
	connect_async(host.c_str(), port, keepalive);
	subscribe(nullptr, subscribe_topic.c_str(), 2);
}

MQTT_communicator::~MQTT_communicator()
{
	disconnect();
	loop_stop();
}

void MQTT_communicator::on_connect(int rc)
{
	if (rc == 0) {
//		std::cout << "Connection established to " << host << ":" << port;
	} else {
//		std::cout << "Error on connect: Code " << rc;
	}
}

void MQTT_communicator::on_disconnect(int rc)
{
	if (rc == 0) {
//		std::cout << "Disconnected from  " << host << ":" << port;
	} else {
//		std::cout << "Unexpected disconnect: Code " << rc;
	}
}

void MQTT_communicator::on_message(const mosquitto_message *msg)
{
	mosquitto_message* buf = static_cast<mosquitto_message*>(malloc(sizeof(mosquitto_message)));
	if (!buf) {
//		std::cout << "malloc failed allocating mosquitto_message.");
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
		throw std::runtime_error("Error sending message: Code " + std::to_string(ret));
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
