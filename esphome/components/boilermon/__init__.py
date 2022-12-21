import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import CONF_ID, CONF_SCL, CONF_SDA, CONF_ADDRESS, CONF_INPUT, CONF_OUTPUT

#DEPENDENCIES = 
AUTO_LOAD = ['sensor', 'text_sensor']

boilermon_ns = cg.esphome_ns.namespace('boilermon')
BoilerMonComponent = boilermon_ns.class_('BoilerMonComponent', cg.Component)
CONF_BOILERMON_ID = "boilermon_id"
CONF_BOILERMON_ADDRESS = "boilermon_address"
pin_with_input_and_output_support = cv.All(
    pins.internal_gpio_pin_number({CONF_INPUT: True}),
    pins.internal_gpio_pin_number({CONF_OUTPUT: True}),
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BoilerMonComponent),
            cv.Required(CONF_ADDRESS): cv.i2c_address,
            cv.Required(CONF_SDA): pins.internal_gpio_input_pin_schema,
            cv.Required(CONF_SCL): pins.internal_gpio_input_pin_schema,
        }
    )  .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    #address = await cg.get_variable(config[CONF_ADDRESS])
    cg.add(var.set_i2c_address(config[CONF_ADDRESS]))
    pin = await cg.gpio_pin_expression(config[CONF_SDA])
    cg.add(var.set_sda_pin(pin))
    pin = await cg.gpio_pin_expression(config[CONF_SCL])
    cg.add(var.set_scl_pin(pin))