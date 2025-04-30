# This Python file defines the YAML configuration schema and ties it to the C++ implementation. It declares the component class and config options (I2C address, update interval, and sub-entities for sensor and button). It ensures the encoder value sensor and button binary sensor are set up and linked to the C++ component.

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_NAME, CONF_UPDATE_INTERVAL, CONF_ADDRESS
from esphome.components import i2c, sensor, binary_sensor

# Declare namespace and class
gravity360_ns = cg.esphome_ns.namespace('gravity360_encoder')
Gravity360Encoder = gravity360_ns.class_(
    'Gravity360Encoder', sensor.Sensor, cg.PollingComponent, i2c.I2CDevice
)

CONF_BUTTON = "button"
DEFAULT_I2C_ADDRESS = 0x54
DEFAULT_UPDATE_INTERVAL = "100ms"

# Configuration schema for YAML
CONFIG_SCHEMA = sensor.sensor_schema(unit_of_measurement=None, icon="mdi:rotate-360", accuracy_decimals=0).extend({
    cv.GenerateID(): cv.declare_id(Gravity360Encoder),
    cv.Optional(CONF_UPDATE_INTERVAL, default=DEFAULT_UPDATE_INTERVAL): cv.update_interval,
    cv.Optional(CONF_ADDRESS, default=DEFAULT_I2C_ADDRESS): cv.i2c_address,
    cv.Required(CONF_BUTTON): binary_sensor.binary_sensor_schema(),
}).extend(i2c.i2c_device_schema(DEFAULT_I2C_ADDRESS)).extend(cv.COMPONENT_SCHEMA)

# Code generation logic
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    await i2c.register_i2c_device(var, config)
    if CONF_UPDATE_INTERVAL in config:
        cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))
    # Set up the binary sensor for the button and link it
    btn = await binary_sensor.new_binary_sensor(config[CONF_BUTTON])
    cg.add(var.set_button_sensor(btn))
