#include "esphome.h"
#include "mrx_frame.h"

using namespace esphome;

static const char *TAG = "mr24hpc1";

class MR24HPC1 : public Component, public UARTDevice
{
public:
    MR24HPC1(UARTComponent *parent) : UARTDevice(parent) {}
    MRX_Frame frame;

    Sensor *presence_sensor = new Sensor();
    Sensor *motion_sensor = new Sensor();
    Sensor *proximity_sensor = new Sensor();
    Sensor *body_sensor = new Sensor();
    Sensor *scene_sensor = new Sensor();
    Sensor *sensitivity_sensor = new Sensor();
    Sensor *unoccupied_time_sensor = new Sensor();
    Sensor *status_sensor = new Sensor();

    void setup()
    {
        ESP_LOGI(TAG, "setup");
        
        write_array(FRAME_PRESENCE_INQUIRY, 10);
        flush();
    }

    void loop()
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

    void process()
    {
        int value = frame.get_value();
        std::string msg = frame.to_string();

        switch (frame.type)
        {
        case HEARTBEAT:
            ESP_LOGD(TAG, "heartbeat");
            break;

        case RESET_RESPONSE:
            ESP_LOGD(TAG, "resetting");
            break;

        case INIT_COMPLETE:
            ESP_LOGD(TAG, "init complete");
            break;

        case STATUS_INQUIRY:
            ESP_LOGD(TAG, "status %i", value);
            status_sensor->publish_state(value);
            break;

        case PRESENCE:
        case PRESENCE_INQUIRY:
            ESP_LOGD(TAG, "presence %i", value);
            presence_sensor->publish_state(value);
            break;

        case BODY_MOVEMENT:
        case BODY_MOVEMENT_INQUIRY:
            ESP_LOGD(TAG, "body movement %i", value);
            body_sensor->publish_state(value);
            break;

        case MOTION:
        case MOTION_INQUIRY:
            ESP_LOGD(TAG, "motion %i", value);
            motion_sensor->publish_state(value);
            break;

        case PROXIMITY:
        case PROXIMITY_INQUIRY:
            ESP_LOGD(TAG, "proximity %i", value);
            proximity_sensor->publish_state(value);
            break;

        case SCENE_RESPONSE:
        case SCENE_INQUIRY:
            ESP_LOGD(TAG, "scene mode %i", value);
            scene_sensor->publish_state(value);
            break;

        case SENSITIVITY_RESPONSE:
        case SENSITIVITY_INQUIRY:
            ESP_LOGD(TAG, "sensitivity %i", value);
            sensitivity_sensor->publish_state(value);
            break;

        case TIME_TO_NO_PERSON_STATE_RESPONSE:
        case TIME_TO_NO_PERSON_STATE_INQUIRY:
            ESP_LOGD(TAG, "unoccupied time %i", value);
            unoccupied_time_sensor->publish_state(value);
            break;

        case MOTION_TO_STILL_INQUIRY:
        case MOTION_TO_STILL_RESPONSE:
            ESP_LOGD(TAG, "motion to still time");
            ESP_LOGD(TAG, msg.c_str());
            break;

        case NO_PERSON_STATE_TIME_INQUIRY:
        case NO_PERSON_STATE_TIME_RESPONSE:
            ESP_LOGD(TAG, "no person time");
            ESP_LOGD(TAG, msg.c_str());
            break;

        case OPEN_OUTPUT_INQUIRY:
        case OPEN_OUTPUT_RESPONSE:
            ESP_LOGD(TAG, "open function output %i", value);
            break;

        case OPEN_SENSOR_REPORT:
            ESP_LOGD(TAG, "open report");

            ESP_LOGD(TAG, msg.c_str());
            break;

        default:
            ESP_LOGD(TAG, msg.c_str());
        }
    }

    float get_setup_priority() const
    {
        return esphome::setup_priority::AFTER_CONNECTION;
    }

    void dump_config()
    {
        ESP_LOGCONFIG(TAG, "MR24HPC1: ");
        LOG_SENSOR("  ", "Presence", presence_sensor);
        LOG_SENSOR("  ", "Motion", motion_sensor);
        LOG_SENSOR("  ", "Body", body_sensor);
        LOG_SENSOR("  ", "Proximity", proximity_sensor);
    }
};
