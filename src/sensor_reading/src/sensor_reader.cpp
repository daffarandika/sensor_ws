#include <functional>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include <cstdlib>
#include <ctime>

using namespace std::chrono_literals;

class SensorReader : public rclcpp::Node {
public: 
	SensorReader() : Node("sensor_read_pub") {
		publisher_ = this->create_publisher<std_msgs::msg::Int32>("sensor", 10);
		timer_ = this->create_wall_timer(
			500ms,
			std::bind(&SensorReader::publish, this)
		);
	}

private:
	rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;

	int read_from_sensor() {
		std::srand(static_cast<unsigned int>(time(nullptr)));
		return std::rand() % 50 + 1;
	}

	void publish() {
		std_msgs::msg::Int32 msg = std_msgs::msg::Int32();
		msg.data = read_from_sensor();
		RCLCPP_INFO(this->get_logger(), "Terbaca: '%i', bersiap mengirim", msg.data);
		publisher_->publish(msg);
	}
};

int main (int argc, char *argv[]) {
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<SensorReader>());
	rclcpp::shutdown();
	return 0;
}


