#include "esphome.h"
#include "mrx_frame.h"

using namespace esphome;

class MR24HPC1 : public Component, public UARTDevice
{
public:
    MR24HPC1(UARTComponent *parent) : UARTDevice(parent) {}

    MRX_Frame frame;

    Sensor *presence_sensor = new Sensor();
    Sensor *motion_sensor = new Sensor();
    Sensor *proximity_sensor = new Sensor();
    Sensor *body_sensor = new Sensor();

    void setup()
    {
        ESP_LOGI("r24d", "setup");
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
        switch (frame.type)
        {
        case HEARTBEAT:
            ESP_LOGD("r24d", "heartbeat");
            break;

        case RESET_RESPONSE:
            ESP_LOGD("r24d", "resetting");
            break;

        case INIT_COMPLETE:
            ESP_LOGD("r24d", "init complete");
            break;

        case PRESENCE:
        case PRESENCE_INQUIRY:
            presence_sensor->publish_state(value);
            break;

        case BODY_MOVEMENT:
            body_sensor->publish_state(value);
            break;

        case MOTION:
        case MOTION_INQUIRY:
            motion_sensor->publish_state(value);
            break;

        case PROXIMITY:
        case PROXIMITY_INQUIRY:
            proximity_sensor->publish_state(value);
            break;

        case SCENE_RESPONSE:
            ESP_LOGD("r24d", "scene mode change acknowledgement: %i",  value);
            break;

        case SENSITIVITY_RESPONSE:
            ESP_LOGD("r24d", "sensitivity change acknowledgement: %i",  value);
            break;

        case TIME_TO_NO_PERSON_STATE_RESPONSE:
            ESP_LOGD("r24d", "time to no person state change acknowledgement: %i",  value);
            break;

        default:
            std::string msg = frame.to_string();
            ESP_LOGD("r24d", msg.c_str());
        }
    }
};
