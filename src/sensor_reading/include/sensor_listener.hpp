#pragma once
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

class SensorListener : public rclcpp::Node
{
public:
  SensorListener();

private:
  void subscribe(const std_msgs::msg::Int32 & msg) const;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_;
};
