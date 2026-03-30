#include "sensor_listener.hpp"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char * argv[])
{
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<SensorListener>());
	rclcpp::shutdown();
	return 0;
}
