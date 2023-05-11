#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/core/log.h"
#include "mrx_frame.h"

namespace esphome
{
    namespace mr24hpc1
    {
        class MR24HPC1Component : public Component, public uart::UARTDevice
        {
        public:
            void setup() override;
            void loop() override;

            void dump_config() override;
            float get_setup_priority() const override;

            void set_presence_sensor(output::BinaryOutput *presence_sensor) { presence_sensor_ = presence_sensor; }
            void set_motion_sensor(output::BinaryOutput *motion_sensor) { motion_sensor_ = motion_sensor; }

            void set_proximity_sensor(sensor::Sensor *proximity_sensor) { proximity_sensor_ = proximity_sensor; }
            void set_body_sensor(sensor::Sensor *body_sensor) { body_sensor_ = body_sensor; }

        protected:
            MRX_Frame frame;

            binary_sensor::BinarySensor *motion_sensor_{nullptr};
            binary_sensor::BinarySensor *presence_sensor_{nullptr};

            sensor::Sensor *body_sensor_{nullptr};
            sensor::Sensor *proximity_sensor_{nullptr};

        private:
            void process();
        };
    }
}