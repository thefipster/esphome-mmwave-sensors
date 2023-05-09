#include "mr24hpc1.h"

namespace esphome
{
    namespace mr24hpc1
    {
        static const char *TAG = "mr24hpc1";

        void MR24HPC1::setup()
        {
            ESP_LOGI(TAG, "setup");
        }

        void MR24HPC1::loop()
        {
            while (available())
            {
                int data = read();
                frame.push(data);
                if (frame.complete)
                {
                    process();
                    frame.clear();
                }
            }
        }

        void MR24HPC1::update() {
        
        }

        float MR24HPC1::get_setup_priority() const
        {
            return esphome::setup_priority::AFTER_CONNECTION;
        }

        void MR24HPC1::dump_config()
        {
            ESP_LOGCONFIG(TAG, "MR24HPC1:");
            LOG_SENSOR("  ", "Presence", this->presence_sensor_);
        }

        void MR24HPC1::process()
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
                    this->presence_sensor_->publish_state(value);
                break;

            case BODY_MOVEMENT:
                //body_sensor->publish_state(value);
                break;

            case MOTION:
            case MOTION_INQUIRY:
                if (this->motion_sensor_ != nullptr)
                    this->motion_sensor_->publish_state(value);
                break;

            case PROXIMITY:
            case PROXIMITY_INQUIRY:
                //proximity_sensor->publish_state(value);
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