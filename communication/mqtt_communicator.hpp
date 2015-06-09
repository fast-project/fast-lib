/*
 * This file is part of fast-lib.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#ifndef FAST_LIB_MQTT_COMMUNICATOR_HPP
#define FAST_LIB_MQTT_COMMUNICATOR_HPP

#include "communicator.hpp"

#include <mosquittopp.h>

#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <chrono>

namespace fast {

class MQTT_subscription
{
public:
	virtual void add_message(const mosquitto_message *msg) = 0;
	virtual std::string get_message(const std::chrono::duration<double> &duration) = 0;
};

/**
 * \brief A specialized Communicator to provide communication using mqtt framework mosquitto.
 *
 * Initialize mosquitto before using this class (e.g. using mosqpp::lib_init() and mosqpp::lib_cleanup() in main)
 */
class MQTT_communicator : 
	public Communicator, 
	private mosqpp::mosquittopp
{
public:
	/**
	 * \brief Constructor for MQTT_communicator.
	 *
	 * Establishes a connection, starts async mosquitto loop and subscribes to topic.
	 * The async mosquitto loop runs in a seperate thread so callbacks and send_/get_message should be threadsafe.
	 * Initialize mosquitto before using this class (e.g. using mosqpp::lib_init() and mosqpp::lib_cleanup() in main)
	 * Establishing a connection is retried every second until success or timeout.
	 * \param id
	 * \param subscribe_topic The topic to subscribe to.
	 * \param publish_topic The topic to publish messages to by default.
	 * \param host The host to connect to.
	 * \param port The port to connect to.
	 * \param keepalive The number of seconds the broker sends periodically ping messages to test if client is still alive.
	 * \param timeout The timeout of establishing a connection to the MQTT broker e.g. std::chrono::seconds(10).
	 */
	MQTT_communicator(const std::string &id, 
			  const std::string &subscribe_topic,
			  const std::string &publish_topic,
			  const std::string &host, 
			  int port,
			  int keepalive,
			  const std::chrono::duration<double> &timeout = std::chrono::duration<double>::max());
	/**
	 * \brief Destructor for MQTT_communicator.
	 *
	 * Stops async mosquitto loop and disconnects.
	 */
	~MQTT_communicator();
	/**
	 * \brief Add a subscription to listen on for messages.
	 */
	void add_subscription(const std::string &topic, int qos = 2);
	/**
	 * \brief Add a subscription with a callback to retrieve messages.
	 */
	void add_subscription(const std::string &topic, std::function<void(std::string)> callback, int qos = 2);
	/**
	 * \brief Remove a subscription.
	 */
	void remove_subscription(const std::string &topic);
	/**
	 * \brief Send a message to a specific topic.
	 */
	void send_message(const std::string &message) override;
	/**
	 * \brief Send a message to a specific topic.
	 */
	void send_message(const std::string &message, const std::string &topic, int qos = 2);
	/**
	 * \brief Get a message from a default topic.
	 */
	std::string get_message() override;
	/**
	 * \brief Get a message from a specific topic.
	 *
	 * \param topic The topic to listen on for a message.
	 */
	std::string get_message(const std::string &topic);
	/**
	 * \brief Get a message from a default topic with timeout.
	 *
	 * \param duration The duration until timeout.
	 */
	std::string get_message(const std::chrono::duration<double> &duration);
	/**
	 * \brief Get a message from a specific topic with timeout.
	 *
	 * \param topic The topic to listen on for a message.
	 * \param duration The duration until timeout.
	 */
	std::string get_message(const std::string &topic, 
				const std::chrono::duration<double> &duration);
private:
	/**
	 * \brief Callback for established connections.
	 */
	void on_connect(int rc) override;
	/**
	 * \brief Callback for disconnected connections.
	 */
	void on_disconnect(int rc) override;
	/**
	 * \brief Callback for received messages.
	 */
	void on_message(const mosquitto_message *msg) override;

	std::string default_subscribe_topic;
	std::string default_publish_topic;

	std::unordered_map<std::string, std::shared_ptr<MQTT_subscription>> subscriptions;
	std::mutex subscriptions_mutex;

	std::mutex connected_mutex;
	std::condition_variable connected_cv;
	bool connected;
	static unsigned int ref_count;
};

class MQTT_subscription_get : public MQTT_subscription
{
public:
	void add_message(const mosquitto_message *msg) override;
	std::string get_message(const std::chrono::duration<double> &duration) override;
private:
	std::mutex msg_queue_mutex;
	std::condition_variable msg_queue_empty_cv;
	std::queue<mosquitto_message*> messages; /// \todo Consider using unique_ptr.
};

class MQTT_subscription_callback : public MQTT_subscription
{
public:
	MQTT_subscription_callback(std::function<void(std::string)> callback);
	void add_message(const mosquitto_message *msg) override;
	std::string get_message(const std::chrono::duration<double> &duration) override;
private:
	std::string message;
	std::function<void(std::string)> callback;
};

} // namespace fast
#endif
