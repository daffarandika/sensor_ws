#include <functional>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include <memory>

using std::placeholders::_1;

class SensorListener : public rclcpp::Node {
public:
	SensorListener() : Node("sensor_read_sub") {
		subscriber_ = this->create_subscription<std_msgs::msg::Int32>(
			"sensor", 
			10,
			std::bind(&SensorListener::subscribe, this, _1)
		);
	}
private:
	rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_;
	void subscribe(const std_msgs::msg::Int32& msg) const {
		RCLCPP_INFO(this->get_logger(), "Menerima pesan: '%i'", msg.data);
	}
};

int main (int argc, char *argv[]) {
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<SensorListener>());
	rclcpp::shutdown();
	return 0;
}

