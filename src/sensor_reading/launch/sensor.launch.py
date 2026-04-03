from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    node_reader = Node(
            package='sensor_reading',
            executable='sensor_reader',
            name='sensor_reader_node',
            output='screen'
        ),
    node_listener = Node(
            package='sensor_reading',
            executable='sensor_listener',
            name='sensor_listener_node',
            output='screen'
        ),

    return LaunchDescription([
        Node(
            package='sensor_reading',
            executable='sensor_reader',
            name='sensor_reader_node',
            output='screen'
        ),
        Node(
            package='sensor_reading',
            executable='sensor_listener',
            name='sensor_listener_node',
            output='screen'
        )
    ])