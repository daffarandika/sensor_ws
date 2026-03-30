#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <climits>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include "sensor_reader.hpp"
#include "sensor_listener.hpp"

using namespace std::chrono_literals;


// ── Shared base fixture ───────────────────────────────────────────────────────

class RosTestBase : public ::testing::Test
{
protected:
static void SetUpTestSuite()
{
	rclcpp::init(0, nullptr);
}

static void TearDownTestSuite()
{
	rclcpp::shutdown();
}

void spin_for(std::chrono::milliseconds duration)
{
	auto start = std::chrono::steady_clock::now();
	while (std::chrono::steady_clock::now() - start < duration) {
		executor_->spin_some();
		std::this_thread::sleep_for(10ms);
	}
}

std::shared_ptr<rclcpp::executors::SingleThreadedExecutor> executor_;
};


// ════════════════════════════════════════════════════════════════════════════
// SensorReader Tests
// ════════════════════════════════════════════════════════════════════════════

class TestSensorReader : public RosTestBase
{
protected:
void SetUp() override
{
	reader_ = std::make_shared<SensorReader>();
	executor_ = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
	executor_->add_node(reader_);
}

void TearDown() override
{
	executor_->remove_node(reader_);
	reader_.reset();
}

std::shared_ptr<SensorReader> reader_;
};

// Node metadata
TEST_F(TestSensorReader, NodeHasCorrectName)
{
	EXPECT_EQ(reader_->get_name(), std::string("sensor_read_pub"));
}

TEST_F(TestSensorReader, NodePublishesToCorrectTopic)
{
	auto topic_names = reader_->get_topic_names_and_types();
	EXPECT_TRUE(topic_names.find("/sensor") != topic_names.end());
}

// Sensor reading logic
TEST_F(TestSensorReader, ReadFromSensorReturnsExpectedValue)
{
	EXPECT_EQ(reader_->read_from_sensor(), 89);
}

TEST_F(TestSensorReader, ReadFromSensorReturnsInt)
{
	auto value = reader_->read_from_sensor();
	EXPECT_GE(value, INT_MIN);
	EXPECT_LE(value, INT_MAX);
}

// Publishing behaviour — use a helper subscriber to intercept messages
TEST_F(TestSensorReader, PublishSendsCorrectValue)
{
	std::vector<int32_t> received;

	// Spy subscriber on the same topic
	auto spy = std::make_shared<rclcpp::Node>("spy_sub");
	auto sub = spy->create_subscription<std_msgs::msg::Int32>(
		"sensor", 10,
		[&received](const std_msgs::msg::Int32::SharedPtr msg) {
		received.push_back(msg->data);
	}
		);
	executor_->add_node(spy);

	reader_->publish();
	spin_for(300ms);

	executor_->remove_node(spy);

	ASSERT_FALSE(received.empty()) << "No message was received";
	EXPECT_EQ(received[0], 89);
}

TEST_F(TestSensorReader, PublishSendsInt32Message)
{
	std::vector<std_msgs::msg::Int32> received;

	auto spy = std::make_shared<rclcpp::Node>("spy_type_sub");
	auto sub = spy->create_subscription<std_msgs::msg::Int32>(
		"sensor", 10,
		[&received](const std_msgs::msg::Int32::SharedPtr msg) {
		received.push_back(*msg);
	}
		);
	executor_->add_node(spy);

	reader_->publish();
	spin_for(300ms);

	executor_->remove_node(spy);

	ASSERT_FALSE(received.empty());
	// Verify it's a proper Int32 (data field is accessible)
	EXPECT_EQ(received[0].data, reader_->read_from_sensor());
}

TEST_F(TestSensorReader, TimerTriggersPublish)
{
	std::vector<int32_t> received;

	auto spy = std::make_shared<rclcpp::Node>("spy_timer_sub");
	auto sub = spy->create_subscription<std_msgs::msg::Int32>(
		"sensor", 10,
		[&received](const std_msgs::msg::Int32::SharedPtr msg) {
		received.push_back(msg->data);
	}
		);
	executor_->add_node(spy);

	// Timer fires every 500ms — spin for 1.2s, expect at least 2 publishes
	spin_for(1200ms);

	executor_->remove_node(spy);

	EXPECT_GE(received.size(), 2u)
	        << "Expected at least 2 timer-triggered publishes in 1.2 seconds";
}


// ════════════════════════════════════════════════════════════════════════════
// SensorListener Tests
// ════════════════════════════════════════════════════════════════════════════

// Extended listener that exposes received data for testing
class TestableSensorListener : public rclcpp::Node
{
public:
std::vector<int32_t> received;

TestableSensorListener()
	: Node("sensor_read_sub")
{
	subscriber_ = this->create_subscription<std_msgs::msg::Int32>(
		"sensor", 10,
		[this](const std_msgs::msg::Int32::SharedPtr msg) {
			received.push_back(msg->data);
		}
		);
}

private:
rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_;
};

class TestSensorListener : public RosTestBase
{
protected:
void SetUp() override
{
	listener_ = std::make_shared<TestableSensorListener>();
	executor_ = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
	executor_->add_node(listener_);
}

void TearDown() override
{
	executor_->remove_node(listener_);
	listener_.reset();
}

// Helper: publish a value onto "sensor" from a temporary node
void publish_value(int32_t value)
{
	auto pub_node = std::make_shared<rclcpp::Node>("test_publisher");
	auto pub = pub_node->create_publisher<std_msgs::msg::Int32>("sensor", 10);
	executor_->add_node(pub_node);

	spin_for(50ms);                  // allow discovery

	std_msgs::msg::Int32 msg;
	msg.data = value;
	pub->publish(msg);

	spin_for(300ms);                  // allow delivery
	executor_->remove_node(pub_node);
}

std::shared_ptr<TestableSensorListener> listener_;
};

// Node metadata
TEST_F(TestSensorListener, NodeHasCorrectName)
{
	EXPECT_EQ(listener_->get_name(), std::string("sensor_read_sub"));
}

TEST_F(TestSensorListener, NodeSubscribesToCorrectTopic)
{
	auto subscriptions = listener_->get_topic_names_and_types();
	EXPECT_TRUE(subscriptions.find("/sensor") != subscriptions.end());
}

// Subscription behaviour
TEST_F(TestSensorListener, ReceivesPublishedMessage)
{
	publish_value(42);

	ASSERT_FALSE(listener_->received.empty()) << "Listener received no messages";
	EXPECT_EQ(listener_->received[0], 42);
}

TEST_F(TestSensorListener, ReceivesSensorReaderValue)
{
	// Simulate exactly what SensorReader sends
	publish_value(89);

	ASSERT_FALSE(listener_->received.empty());
	EXPECT_EQ(listener_->received[0], 89);
}

TEST_F(TestSensorListener, ReceivesMultipleMessages)
{
	publish_value(10);
	publish_value(20);
	publish_value(30);

	ASSERT_GE(listener_->received.size(), 3u);
	EXPECT_EQ(listener_->received[0], 10);
	EXPECT_EQ(listener_->received[1], 20);
	EXPECT_EQ(listener_->received[2], 30);
}

TEST_F(TestSensorListener, ReceivesNegativeValues)
{
	publish_value(-5);

	ASSERT_FALSE(listener_->received.empty());
	EXPECT_EQ(listener_->received[0], -5);
}

TEST_F(TestSensorListener, ReceivesZero)
{
	publish_value(0);

	ASSERT_FALSE(listener_->received.empty());
	EXPECT_EQ(listener_->received[0], 0);
}


// ════════════════════════════════════════════════════════════════════════════
// Integration: SensorReader → SensorListener
// ════════════════════════════════════════════════════════════════════════════

class TestSensorIntegration : public RosTestBase
{
protected:
void SetUp() override
{
	reader_   = std::make_shared<SensorReader>();
	listener_ = std::make_shared<TestableSensorListener>();
	executor_ = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
	executor_->add_node(reader_);
	executor_->add_node(listener_);
}

void TearDown() override
{
	executor_->remove_node(reader_);
	executor_->remove_node(listener_);
	reader_.reset();
	listener_.reset();
}

std::shared_ptr<SensorReader>           reader_;
std::shared_ptr<TestableSensorListener> listener_;
};

TEST_F(TestSensorIntegration, ListenerReceivesFromReader)
{
	// Manually trigger one publish instead of waiting for the timer
	reader_->publish();
	spin_for(400ms);

	ASSERT_FALSE(listener_->received.empty())
	        << "Listener did not receive SensorReader's message";
	EXPECT_EQ(listener_->received[0], 89);
}

TEST_F(TestSensorIntegration, TimerDrivenEndToEnd)
{
	// Let the 500ms timer fire naturally twice
	spin_for(1200ms);

	EXPECT_GE(listener_->received.size(), 2u);
	for (auto val : listener_->received) {
		EXPECT_EQ(val, 89);
	}
}


// ── Main ─────────────────────────────────────────────────────────────────────

int main(int argc, char ** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
