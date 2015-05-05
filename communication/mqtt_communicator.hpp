#ifndef MQTT_COMMUNICATOR_HPP
#define MQTT_COMMUNICATOR_HPP

#include "communicator.hpp"

#include <mosquittopp.h>

#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace fast {

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
	 * \param id
	 * \param subscribe_topic The topic to subscribe to.
	 * \param publish_topic The topic to publish messages to.
	 * \param host The host to connect to.
	 * \param port The port to connect to.
	 * \param keepalive The number of seconds the broker sends periodically ping messages to test if client is still alive.
	 */
	MQTT_communicator(const std::string &id, 
			  const std::string &subscribe_topic,
			  const std::string &publish_topic,
			  const std::string &host, 
			  int port,
			  int keepalive);
	/**
	 * \brief Destructor for MQTT_communicator.
	 *
	 * Stops async mosquitto loop and disconnects.
	 */
	~MQTT_communicator();
	/**
	 * \brief Send a message.
	 */
	void send_message(const std::string &message) override;
	/**
	 * \brief Send a message.
	 */
	void send_message(const std::string &message, const std::string &topic);
	/**
	 * \brief Get a message (blocking).
	 */
	std::string get_message() override;

	/**
	 * \brief Get a message.
	 *
	 * \param duration The time in milliseconds until timeout.
	 */
	std::string get_message(const std::chrono::duration<double> &duration);
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

	std::string id;
	std::string subscribe_topic;
	std::string publish_topic;
	std::string host;
	int port;
	int keepalive;
	std::mutex msg_queue_mutex;
	std::condition_variable msg_queue_empty_cv;
	std::queue<mosquitto_message*> messages; /// \todo Consider using unique_ptr.

};

} // namespace fast
#endif
