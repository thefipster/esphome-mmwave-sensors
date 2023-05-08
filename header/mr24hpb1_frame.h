#ifndef MRX_FRAME_H_
#define MRX_FRAME_H_

#define UNDEFINED -1
#define SOFTWARE_VERSION 0
#define ENVIRONMENTAL 1
#define BODY_MOVEMENT 2
#define PROXIMITY 3
#define HEARTBEAT 4
#define ABNORMAL_RESET 5
#define INIT_SUCCESS 6

struct MR24HPB1_Frame
{
    /*
        frame definition

        header      | data length         | function | address_1 | address_2 | data    | crc16
        1 byte 0x55 | 2 bytes (low, high) | 1 byte   | 1 byte    | 1 byte    | n bytes | 2 bytes (low, high)
    */

    typedef union
    {
        unsigned char Byte[4];
        float Float;
    } FloatByte;

    char hexmap[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    std::vector<int> bytes;
    int type = UNDEFINED;

    void push(int byte_token)
    {
        bytes.push_back(byte_token);
        
        if (has_start() && is_complete())
        {
            type = get_type();
        }
    }

    // index 2 and 3 of the byte vector define the message type
    // 2: control word
    // 3: command word
    int get_type()
    {
        switch (bytes[3])
        {
        case 0x04:
            return get_proactive_report_type();
        default:
            return -1;
        }
    }

    // 0x53 0x59 0x01 [...]
    int get_proactive_report_type()
    {
        switch (bytes[4])
        {
        case 0x01:
            return get_module_type();
        case 0x03:
            return get_sensor_type();
        case 0x05:
            return get_other_type();
        default:
            return -2;
        }
    }

    int get_module_type()
    {
        switch (bytes[5])
        {
        case 0x02:
            return SOFTWARE_VERSION;
        default:
            return -3;
        }
    }

    int get_sensor_type()
    {
        switch (bytes[5])
        {
        case 0x05:
            return ENVIRONMENTAL;
        case 0x06:
            return BODY_MOVEMENT;
        case 0x07:
            return PROXIMITY;
        default:
            return -4;
        }
    }

    int get_other_type()
    {
        switch (bytes[5])
        {
        case 0x01:
            return HEARTBEAT;
        case 0x02:
            return ABNORMAL_RESET;
        case 0x0A:
            return INIT_SUCCESS;
        default:
            return -5;
        }
    }

    // warning
    // product information message contain more than one byte of data!
    // these messages are not implemented so far
    int get_value()
    {
        int value = 0;

        if (is_complete())
        {
            switch (type)
            {
            case HEARTBEAT:
            case ENVIRONMENTAL:
                value = bytes[7];
                break;
            case BODY_MOVEMENT:
                FloatByte fb;
                fb.Byte[0] = bytes[6];
                fb.Byte[1] = bytes[7];
                fb.Byte[2] = bytes[8];
                fb.Byte[3] = bytes[9];
                value = fb.Float;
                break;
            case PROXIMITY:
                value = bytes[8];
                break;
            }
        }

        return value;
    }

    void clear()
    {
        type = UNDEFINED;
        bytes.clear();
    }

    // 0x55 [...]
    bool has_start()
    {
        return bytes[0] == 0x55;
    }

    // index 1 and 2 of the byte vector define the data length
    // total length = 1 byte header + n byte data length
    // 1: low byte
    // 2: high byte
    int get_msg_length()
    {
        if (bytes.size() < 3)
            return -1;

        return 1 + (bytes[1] | bytes[2] << 8);
    }

    bool is_complete()
    {
        return get_msg_length() == bytes.size();
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