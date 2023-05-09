#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/log.h"
#include "mrx_frame.h"

namespace esphome
{
    namespace mr24hpc1
    {
        class MR24HPC1 : public sensor::Sensor, public uart::UARTDevice
        {
        public:
            MRX_Frame frame;

            void setup() override;
            void loop() override;
            void update() override;
            void dump_config() override;
            float get_setup_priority() const override;

            void set_presence_sensor(sensor::Sensor *presence_sensor) { presence_sensor_ = presence_sensor; }
            void set_motion_sensor(sensor::Sensor *motion_sensor) { motion_sensor_ = motion_sensor; }

        protected:
            sensor::Sensor *presence_sensor_{nullptr};
            sensor::Sensor *motion_sensor_{nullptr};

        private:
            void process();
        };
    }
}