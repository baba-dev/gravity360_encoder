import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, i2c
from esphome.const import CONF_ID, CONF_NAME, CONF_ADDRESS, CONF_UPDATE_INTERVAL  # removed UNIT_NONE, ICON_COUNTER

# Define the ESPHome namespace and class for the encoder (from C++ code)
gravity360_ns = cg.esphome_ns.namespace("gravity360_encoder")
Gravity360Encoder = gravity360_ns.class_("Gravity360Encoder", cg.PollingComponent, i2c.I2CDevice, sensor.Sensor)

# Configuration schema for the sensor platform
CONFIG_SCHEMA = (
    sensor.sensor_schema(unit_of_measurement="", icon="mdi:counter", accuracy_decimals=0)  # use literal empty unit and counter icon
    .extend(
        {
            cv.GenerateID(): cv.declare_id(Gravity360Encoder),
            cv.Optional(CONF_ADDRESS, default=0x40): cv.i2c_address,      # default I2C address (modify if needed)
            cv.Optional(CONF_UPDATE_INTERVAL, default="50ms"): cv.update_interval,
        }
    )
    .extend(i2c.i2c_device_schema(CONF_ADDRESS))
)

# Set up the component in ESPHome
async def to_code(config):
    # Instantiate the Gravity360Encoder component
    var = cg.new_Pvariable(config[CONF_ID])

    # Initialize base classes (PollingComponent and I2C device)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    # Apply the polling update interval
    if CONF_UPDATE_INTERVAL in config:
        cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))

    # Register the sensor with ESPHome (assigns name, unit, icon, etc.)
    await sensor.register_sensor(var, config)
