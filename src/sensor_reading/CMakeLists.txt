cmake_minimum_required(VERSION 3.8)
project(sensor_reading)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(sensor_reader ./src/sensor_reader.cpp)
ament_target_dependencies(sensor_reader rclcpp std_msgs)

add_executable(sensor_listener ./src/sensor_listener.cpp)
ament_target_dependencies(sensor_listener rclcpp std_msgs)

install(TARGETS
  sensor_reader
  sensor_listener
  DESTINATION lib/${PROJECT_NAME})

ament_package()
