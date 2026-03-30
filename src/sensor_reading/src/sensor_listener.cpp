#include "sensor_listener.hpp"
#include <functional>

using std::placeholders::_1;

SensorListener::SensorListener() : Node("sensor_read_sub") {
	subscriber_ = this->create_subscription<std_msgs::msg::Int32>(
			"sensor",
			10,
			std::bind(&SensorListener::subscribe, this, _1)
			);
}

void SensorListener::subscribe(const std_msgs::msg::Int32& msg) const {
	RCLCPP_INFO(this->get_logger(), "Menerima pesan: '%i'", msg.data);
}

