#include "sensor_reader.hpp"
#include <functional>
#include <chrono>

using namespace std::chrono_literals;

SensorReader::SensorReader() : Node("sensor_read_pub") {
	publisher_ = this->create_publisher<std_msgs::msg::Int32>("sensor", 10);

	timer_ = this->create_wall_timer(
			500ms,
			std::bind(&SensorReader::publish, this)
			);
}

int SensorReader::read_from_sensor() {
	return 88;
}

void SensorReader::publish() {
	std_msgs::msg::Int32 msg;
	msg.data = read_from_sensor();

	RCLCPP_INFO(this->get_logger(), "Terbaca: '%i', bersiap mengirim", msg.data);
	publisher_->publish(msg);
}

