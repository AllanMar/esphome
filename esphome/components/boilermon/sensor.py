import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, text_sensor
from esphome.const import (
    CONF_ID,
    ICON_THERMOMETER,
    DEVICE_CLASS_TEMPERATURE,
)
from . import BoilerMonComponent, CONF_BOILERMON_ID

DEPENDENCIES = ["boilermon"]

# Text Sensors
CONF_STATUS_TEXT = "boiler_status"

# Sensors
CONF_VALUE = "boiler_value"
CONF_SUPPLY_TEMP = "supply_temp"
CONF_RETURN_TEMP = "return_temp"
CONF_DHW_TEMP = "dhw_temp"
CONF_OUTDOOR_TEMP = "outdoor_temp"
CONF_FLUE_TEMP = "flue_temp"
CONF_SETPOINT = "boiler_setpoint"


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_BOILERMON_ID): cv.use_id(BoilerMonComponent),
        cv.Optional(CONF_STATUS_TEXT): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {cv.GenerateID(): cv.declare_id(text_sensor.TextSensor)}
        ),
        cv.Optional(CONF_VALUE): sensor.sensor_schema(
            unit_of_measurement="°F",
            icon=ICON_THERMOMETER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
        ),
        cv.Optional(CONF_SUPPLY_TEMP): sensor.sensor_schema(
            unit_of_measurement="°F",
            icon=ICON_THERMOMETER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
        ),
        cv.Optional(CONF_RETURN_TEMP): sensor.sensor_schema(
            unit_of_measurement="°F",
            icon=ICON_THERMOMETER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
        ),
        cv.Optional(CONF_DHW_TEMP): sensor.sensor_schema(
            unit_of_measurement="°F",
            icon=ICON_THERMOMETER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
        ),
        cv.Optional(CONF_OUTDOOR_TEMP): sensor.sensor_schema(
            unit_of_measurement="°F",
            icon=ICON_THERMOMETER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
        ),
        cv.Optional(CONF_FLUE_TEMP): sensor.sensor_schema(
            unit_of_measurement="°F",
            icon=ICON_THERMOMETER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
        ),
        cv.Optional(CONF_SETPOINT): sensor.sensor_schema(
            unit_of_measurement="°F",
            icon=ICON_THERMOMETER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
        ),
    }
)


async def to_code(config):
    boilermon = await cg.get_variable(config[CONF_BOILERMON_ID])
    if CONF_STATUS_TEXT in config:
        conf = config[CONF_STATUS_TEXT]
        sens = cg.new_Pvariable(conf[CONF_ID])
        await text_sensor.register_text_sensor(sens, conf)
        cg.add(boilermon.set_status_text_sensor(sens))
    if CONF_VALUE in config:
        sens = await sensor.new_sensor(config[CONF_VALUE])
        cg.add(boilermon.set_value_sensor(sens))
    if CONF_SUPPLY_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_SUPPLY_TEMP])
        cg.add(boilermon.set_supply_temp_sensor(sens))
    if CONF_RETURN_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_RETURN_TEMP])
        cg.add(boilermon.set_return_temp_sensor(sens))
    if CONF_DHW_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_DHW_TEMP])
        cg.add(boilermon.set_dhw_temp_sensor(sens))
    if CONF_OUTDOOR_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_OUTDOOR_TEMP])
        cg.add(boilermon.set_outdoor_temp_sensor(sens))
    if CONF_FLUE_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_FLUE_TEMP])
        cg.add(boilermon.set_flue_temp_sensor(sens))
    if CONF_SETPOINT in config:
        sens = await sensor.new_sensor(config[CONF_SETPOINT])
        cg.add(boilermon.set_setpoint_temp_sensor(sens))
