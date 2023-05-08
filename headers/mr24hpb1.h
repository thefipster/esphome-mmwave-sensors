#include "esphome.h"
#include "mr24hpb1_frame.h"

using namespace esphome;

class MR24HPB1 : public Component, public UARTDevice
{
public:
    MR24HPB1(UARTComponent *parent) : UARTDevice(parent) {}

    MR24HPB1_Frame frame;

    Sensor *presence_sensor = new Sensor();
    Sensor *motion_sensor = new Sensor();
    Sensor *proximity_sensor = new Sensor();
    Sensor *body_sensor = new Sensor();

    void setup()
    {
        ESP_LOGI("mr24d", "setup");
    }

    void loop()
    {
        while (available())
        {
            int token = read();
            frame.push(token);

            if (frame.is_complete())
            {
                process();
                frame.clear();
            }
        }
    }

    void process()
    {
        std::string msg = frame.to_string();
        int value = frame.get_value();

        switch (frame.type)
        {
        case HEARTBEAT:
        case ENVIRONMENTAL:
            ESP_LOGD("mr24d", "environment %i", value);
            process_environment(value);
            break;
        case BODY_MOVEMENT:
            body_sensor->publish_state(value);
            break;
        case PROXIMITY:
            process_proximity(value);
            break;
        default:
            ESP_LOGD("mr24d", msg.c_str());
            break;
        }
    }

    void process_environment(int value)
    {
        switch (value)
        {
        case 0:
            presence_sensor->publish_state(1);
            motion_sensor->publish_state(0);
            break;
        case 1:
            presence_sensor->publish_state(1);
            motion_sensor->publish_state(1);
            break;
        default:
            presence_sensor->publish_state(0);
            motion_sensor->publish_state(0);
            break;
        }
    }

    void process_proximity(int value)
    {
        switch (value)
        {
        case 1:
            proximity_sensor->publish_state(0);
            break;
        case 2:
            proximity_sensor->publish_state(-1);
            break;
        case 4:
            proximity_sensor->publish_state(-2);
            break;
        case 3:
            proximity_sensor->publish_state(1);
            break;
        case 5:
            proximity_sensor->publish_state(2);
            break;
        }
    }
};
