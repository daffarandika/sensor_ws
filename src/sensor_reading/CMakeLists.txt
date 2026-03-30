cmake_minimum_required(VERSION 3.8)
project(sensor_reading)

# ── Dependencies ──────────────────────────────────────────────────────────────
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# ── Shared libraries (no main()) ──────────────────────────────────────────────
add_library(sensor_reader_lib src/sensor_reader.cpp)
target_include_directories(sensor_reader_lib PUBLIC include)
ament_target_dependencies(sensor_reader_lib rclcpp std_msgs)

add_library(sensor_listener_lib src/sensor_listener.cpp)
target_include_directories(sensor_listener_lib PUBLIC include)
ament_target_dependencies(sensor_listener_lib rclcpp std_msgs)

# ── Executables (point to _main.cpp files, link against libs) ─────────────────
add_executable(sensor_reader src/sensor_reader_main.cpp)
target_link_libraries(sensor_reader sensor_reader_lib)
ament_target_dependencies(sensor_reader rclcpp std_msgs)

add_executable(sensor_listener src/sensor_listener_main.cpp)
target_link_libraries(sensor_listener sensor_listener_lib)
ament_target_dependencies(sensor_listener rclcpp std_msgs)

install(TARGETS
  sensor_reader
  sensor_listener
  DESTINATION lib/${PROJECT_NAME}
)

# ── Tests ─────────────────────────────────────────────────────────────────────
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_sensor_nodes
    test/test_sensor_nodes.cpp
  )

  target_include_directories(test_sensor_nodes PUBLIC include)

  target_link_libraries(test_sensor_nodes
    sensor_reader_lib
    sensor_listener_lib
  )

  ament_target_dependencies(test_sensor_nodes
    rclcpp
    std_msgs
  )
endif()

ament_package()
