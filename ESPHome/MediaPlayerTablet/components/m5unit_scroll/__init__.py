import esphome.codegen as cg
from esphome.components import i2c, sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@ltextor"]
DEPENDENCIES = ["i2c"]

CONF_M5UNIT_SCROLL_ID = "m5unit_scroll_id"

m5unit_scroll_ns = cg.esphome_ns.namespace("m5unit_scroll")
M5UnitScroll = m5unit_scroll_ns.class_(
    "M5UnitScroll", sensor.Sensor, cg.PollingComponent, i2c.I2CDevice
)