#ifndef RX_FRAME_H_
#define RX_FRAME_H_

#define UNDEFINED -1
#define HEARTBEAT 0
#define PRESENCE 1
#define MOTION 2
#define PROXIMITY 3
#define SCENE_RESPONSE 4
#define BODY_MOVEMENT 5
#define INIT_COMPLETE 6
#define SENSITIVITY_RESPONSE 7
#define TIME_TO_NO_PERSON_STATE_RESPONSE 8
#define MOTION_INQUIRY 9
#define PRESENCE_INQUIRY 10
#define PROXIMITY_INQUIRY 11
#define RESET_RESPONSE 12
#define SCENE_INQUIRY 13
#define STATUS_INQUIRY 14
#define DISTANCE 15
#define ORIENTATION 16
#define BODY_MOVEMENT_INQUIRY 17
#define DISTANCE_INQUIRY 18
#define ORIENTATION_INQUIRY 19
#define HEARTRATE 20
#define HEARTRATE_WAVEFORM 21
#define HEARTRATE_INQUIRY 22
#define HEARTRATE_WAVEFORM_INQUIRY 23
#define OUT_OF_BOUNDS 24
#define OUT_OF_BOUNDS_INQUIRY 25
#define BREATHING 26
#define BREATHING_INQUIRY 27
#define BREATHING_INFO 28
#define BREATHING_INFO_INQUIRY 29
#define BREATHING_WAVEFORM 30
#define BREATHING_WAVEFORM_INQUIRY 31
#define SENSITIVITY_INQUIRY 32
#define TIME_TO_NO_PERSON_STATE_INQUIRY 33

struct MRX_Frame
{
    char hexmap[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    std::vector<int> bytes;
    int type = UNDEFINED;
    bool complete = false;

    void push(int byte_token)
    {
        bytes.push_back(byte_token);

        if (has_start() && has_end())
        {
            complete = true;
            type = get_type();
        }
    }

    // index 2 and 3 of the byte vector define the message type
    // 2: control word
    // 3: command word
    int get_type()
    {
        switch (bytes[2])
        {
        case 0x01:
            return get_system_functions_type();
        case 0x05:
            return get_work_status_type();
        case 0x07:
            return get_radar_detection_functions_type();
        case 0x80:
            return get_human_presence_functions_type();
        case 0x81:
            return get_respiratory_monitor_functions_type();
        case 0x85:
            return get_heartrate_monitor_functions_type();
        default:
            return UNDEFINED;
        }
    }

    // 0x53 0x59 0x01 [...]
    int get_system_functions_type()
    {
        switch (bytes[3])
        {
        case 0x01:
            return HEARTBEAT;
        case 0x02:
            return RESET_RESPONSE;
        default:
            return UNDEFINED;
        }
    }

    // 0x53 0x59 0x05 [...]
    int get_work_status_type()
    {
        switch (bytes[3])
        {
        case 0x01:
            return INIT_COMPLETE;
        case 0x07:
            return SCENE_RESPONSE;
        case 0x08:
            return SENSITIVITY_RESPONSE;
        case 0x81:
            return STATUS_INQUIRY;
        case 0x87:
            return SCENE_INQUIRY;
        case 0x88:
            return SENSITIVITY_INQUIRY;
        default:
            return UNDEFINED;
        }
    }

    int get_radar_detection_functions_type()
    {
        switch (bytes[3])
        {
        case 0x07:
            return OUT_OF_BOUNDS;
        case 0x87:
            return OUT_OF_BOUNDS_INQUIRY;
        default:
            return UNDEFINED;
        }
    }

    // 0x53 0x59 0x80 [...]
    int get_human_presence_functions_type()
    {
        switch (bytes[3])
        {
        case 0x01:
            return PRESENCE;
        case 0x02:
            return MOTION;
        case 0x03:
            return BODY_MOVEMENT;
        case 0x04:
            return DISTANCE;
        case 0x05:
            return ORIENTATION;
        case 0x0A:
            return TIME_TO_NO_PERSON_STATE_RESPONSE;
        case 0x0B:
            return PROXIMITY;
        case 0x81:
            return PRESENCE_INQUIRY;
        case 0x82:
            return MOTION_INQUIRY;
        case 0x83:
            return BODY_MOVEMENT_INQUIRY;
        case 0x84:
            return DISTANCE_INQUIRY;
        case 0x85:
            return ORIENTATION_INQUIRY;
        case 0x8A:
            return TIME_TO_NO_PERSON_STATE_INQUIRY;
        case 0x8B:
            return PROXIMITY_INQUIRY;
        default:
            return UNDEFINED;
        }
    }

    int get_respiratory_monitor_functions_type()
    {
        switch (bytes[3])
        {
        case 0x01:
            return BREATHING_INFO;
        case 0x02:
            return BREATHING;
        case 0x05:
            return BREATHING_WAVEFORM;
        case 0x81:
            return BREATHING_INFO_INQUIRY;
        case 0x82:
            return BREATHING_INQUIRY;
        case 0x85:
            return BREATHING_WAVEFORM_INQUIRY;
        default:
            return UNDEFINED;
        }
    }

    int get_heartrate_monitor_functions_type()
    {
        switch (bytes[3])
        {
        case 0x02:
            return HEARTRATE;
        case 0x05:
            return HEARTRATE_WAVEFORM;
        case 0x82:
            return HEARTRATE_INQUIRY;
        case 0x85:
            return HEARTRATE_WAVEFORM_INQUIRY;
        default:
            return UNDEFINED;
        }
    }

    // warning
    // product information message contain more than one byte of data!
    // these messages are not implemented so far
    int get_value()
    {
        switch (type)
        {
        case MOTION:
        case MOTION_INQUIRY:
            return bytes[6] == 0x02 ? 1 : 0;
        case DISTANCE:
        case DISTANCE_INQUIRY:
            return bytes[7] | bytes[6] << 8;
        default:
            return bytes[6];
        }
    }

    int *get_xyz_value()
    {
        static int xyz[3] = {0, 0, 0};

        switch (type)
        {
        case ORIENTATION:
        case ORIENTATION_INQUIRY:
            xyz[0] = int(bytes[6]) + int(bytes[7]);
            xyz[1] = int(bytes[8]) + int(bytes[9]);
            xyz[2] = int(bytes[10]) + int(bytes[11]);
        }

        return xyz;
    }

    void clear()
    {
        type = UNDEFINED;
        complete = false;
        bytes.clear();
    }

    // 0x53 0x59 [...]
    bool has_start()
    {
        return bytes[0] == 0x53 && bytes[1] == 0x59;
    }

    // [...] 0x54 0x43
    bool has_end()
    {
        return bytes[bytes.size() - 2] == 0x54 && bytes[bytes.size() - 1] == 0x43;
    }

    std::string to_string()
    {
        std::string str = "";
        for (int i = 0; i < bytes.size(); i++)
        {
            if (i != 0)
            {
                str += ", ";
            }

            str += "0x";
            str += hexmap[(bytes[i] & 0xF0) >> 4];
            str += hexmap[bytes[i] & 0x0F];
        }

        return str;
    }
};

#endif