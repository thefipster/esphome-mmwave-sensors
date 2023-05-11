#include "mr24hpc1_component.h"

namespace esphome
{
    namespace mr24hpc1
    {
        static const char *TAG = "mr24hpc1";

        void MR24HPC1Component::setup() {}

        void MR24HPC1Component::loop()
        {
            while (this->available())
            {
                int data = this->read();
                frame.push(data);
                if (frame.complete)
                {
                    process();
                    frame.clear();
                }
            }
        }

        float MR24HPC1Component::get_setup_priority() const
        {
            return esphome::setup_priority::AFTER_CONNECTION;
        }

        void MR24HPC1Component::dump_config()
        {
            ESP_LOGCONFIG(TAG, "MR24HPC1: ");
            LOG_SENSOR("  ", "Presence", this->presence_sensor_);
            LOG_SENSOR("  ", "Motion", this->motion_sensor_);
            LOG_SENSOR("  ", "Body", this->body_sensor_);
            LOG_SENSOR("  ", "Proximity", this->proximity_sensor_);
        }

        void MR24HPC1Component::process()
        {
            int value = frame.get_value();
            switch (frame.type)
            {
            case HEARTBEAT:
                ESP_LOGI(TAG, "heartbeat");
                break;

            case RESET_RESPONSE:
                ESP_LOGD(TAG, "resetting");
                break;

            case INIT_COMPLETE:
                ESP_LOGD(TAG, "init complete");
                break;

            case PRESENCE:
            case PRESENCE_INQUIRY:
                if (this->presence_sensor_ != nullptr)
                    this->presence_sensor_->publish_state(value == 1);
                break;

            case BODY_MOVEMENT:
                if (this->body_sensor_ != nullptr)
                    this->body_sensor_->publish_state(value);
                break;

            case MOTION:
            case MOTION_INQUIRY:
                if (this->motion_sensor_ != nullptr)
                    this->motion_sensor_->publish_state(value == 2);
                break;

            case PROXIMITY:
            case PROXIMITY_INQUIRY:
                if (this->proximity_sensor_ != nullptr)
                    this->proximity_sensor_->publish_state(value);
                break;

            case SCENE_RESPONSE:
                ESP_LOGD(TAG, "scene mode change acknowledgement: %i", value);
                break;

            case SENSITIVITY_RESPONSE:
                ESP_LOGD(TAG, "sensitivity change acknowledgement: %i", value);
                break;

            case TIME_TO_NO_PERSON_STATE_RESPONSE:
                ESP_LOGD(TAG, "time to no person state change acknowledgement: %i", value);
                break;

            default:
                std::string msg = frame.to_string();
                ESP_LOGD(TAG, msg.c_str());
            }
        }
    }
}