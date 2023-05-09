import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import (
    uart,
    sensor
)
from esphome.const import (
    CONF_ID,
    UNIT_EMPTY,
    ICON_EMPTY,
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_MEASUREMENT
)

DEPENDENCIES = ['uart']

mr24hpc1_ns = cg.esphome_ns.namespace('mr24hpc1')
MR24HPC1 = mr24hpc1_ns.class_('MR24HPC1', cg.Component, uart.UARTDevice)

CONF_PRESENCE = "presence"
CONF_MOTION = "motion"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MR24HPC1),
    cv.Optional(CONF_PRESENCE):
        sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ).extend(),
    cv.Optional(CONF_MOTION):
        sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ).extend(),
}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)

    if CONF_PRESENCE in config:
        conf = config[CONF_PRESENCE]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_presence_sensor(sens))
        
    if CONF_MOTION in config:
        conf = config[CONF_MOTION]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_motion_sensor(sens))