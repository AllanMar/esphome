import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, text_sensor
from esphome.const import CONF_ID, UNIT_VOLT, ICON_FLASH, UNIT_AMPERE, UNIT_WATT, \
    UNIT_WATT_HOURS, ICON_POWER, ICON_CURRENT_AC, ICON_COUNTER, UNIT_EMPTY, ICON_EMPTY, \
    DEVICE_CLASS_TEMPERATURE, DEVICE_CLASS_EMPTY
from . import boilermon_ns, BoilerMonComponent, CONF_BOILERMON_ID

DEPENDENCIES = ["boilermon"]

# Text Sensors
CONF_STATUS_TEXT = "boiler_status"

#Sensors
CONF_VALUE = "boiler_value"


CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_BOILERMON_ID): cv.use_id(BoilerMonComponent),
    cv.Optional(CONF_STATUS_TEXT): text_sensor.TEXT_SENSOR_SCHEMA.extend({cv.GenerateID(): cv.declare_id(text_sensor.TextSensor)}),
    cv.Optional(CONF_VALUE): sensor.sensor_schema(
    unit_of_measurement="°F",
    icon=ICON_EMPTY,
    accuracy_decimals=0,
    device_class=DEVICE_CLASS_TEMPERATURE)
})

async def to_code(config):
    boilermon = await cg.get_variable(config[CONF_BOILERMON_ID])

    if CONF_VALUE in config:
        sens = await sensor.new_sensor(config[CONF_VALUE])
        cg.add(boilermon.set_value_sensor(sens))

    if CONF_STATUS_TEXT in config:
        conf = config[CONF_STATUS_TEXT]
        sens = cg.new_Pvariable(conf[CONF_ID])
        await text_sensor.register_text_sensor(sens, conf)
        cg.add(boilermon.set_status_text_sensor(sens))