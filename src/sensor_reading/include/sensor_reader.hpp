#pragma once
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

class SensorReader : public rclcpp::Node
{
public:
  SensorReader();
  int read_from_sensor();
  void publish();

private:
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};
