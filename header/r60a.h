#include "esphome.h"
#include "rx_frame.h"

using namespace esphome;

class UARTSensor : public Component, public UARTDevice
{
public:
    UARTSensor(UARTComponent *parent) : UARTDevice(parent) {}

    RX_Frame frame;

    Sensor *presence_sensor = new Sensor();
    Sensor *motion_sensor = new Sensor();
    Sensor *distance_sensor = new Sensor();
    Sensor *x_sensor = new Sensor();
    Sensor *y_sensor = new Sensor();
    Sensor *z_sensor = new Sensor();
    Sensor *heartrate_sensor = new Sensor();
    Sensor *breathing_sensor = new Sensor();
    Sensor *breathing_info_sensor = new Sensor();
    Sensor *body_movement_sensor = new Sensor();
    Sensor *radar_oob_sensor = new Sensor();

    void setup()
    {
        ESP_LOGI("r60a", "setup");
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
        int *xyz = frame.get_xyz_value();
        std::string msg = frame.to_string();

        switch (frame.type)
        {
        case HEARTBEAT:
            ESP_LOGD("r60a", "heartbeat");
            break;

        case RESET_RESPONSE:
            ESP_LOGD("r60a", "resetting");
            break;

        case INIT_COMPLETE:
            ESP_LOGD("r60a", "init complete");
            break;

        case PRESENCE:
        case PRESENCE_INQUIRY:
            presence_sensor->publish_state(value);
            break;

        case OUT_OF_BOUNDS:
        case OUT_OF_BOUNDS_INQUIRY:
            radar_oob_sensor->publish_state(value);
            break;

        case BODY_MOVEMENT:
            body_movement_sensor->publish_state(value);
            break;

        case MOTION:
        case MOTION_INQUIRY:
            motion_sensor->publish_state(value);
            break;

        case BREATHING:
        case BREATHING_INQUIRY:
            breathing_sensor->publish_state(value);
            break;

        case BREATHING_INFO:
        case BREATHING_INFO_INQUIRY:
            breathing_info_sensor->publish_state(value);
            break;

        case BREATHING_WAVEFORM:
        case BREATHING_WAVEFORM_INQUIRY:
            break;

        case DISTANCE:
        case DISTANCE_INQUIRY:
            distance_sensor->publish_state(value);
            break;

        case ORIENTATION:
        case ORIENTATION_INQUIRY:
            x_sensor->publish_state(*(xyz));
            y_sensor->publish_state(*(xyz + 1));
            z_sensor->publish_state(*(xyz + 2));
            break;

        case HEARTRATE:
        case HEARTRATE_INQUIRY:
            heartrate_sensor->publish_state(value);
            break;

        case HEARTRATE_WAVEFORM:
        case HEARTRATE_WAVEFORM_INQUIRY:
            break;

        default:
            ESP_LOGD("r60a", msg.c_str());
        }
    }
};
