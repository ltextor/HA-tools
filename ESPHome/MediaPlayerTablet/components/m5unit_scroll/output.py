import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_CHANNEL, CONF_ID

from . import M5UnitScroll, m5unit_scroll_ns, CONF_M5UNIT_SCROLL_ID

CONF_LED_INDEX = "led_index"

M5UnitScrollLEDOutput = m5unit_scroll_ns.class_(
    "M5UnitScrollLEDOutput", output.FloatOutput
)

# Maps YAML channel name → uint8_t value used by set_led_channel()
LED_CHANNELS = {
    "red": 0,
    "green": 1,
    "blue": 2,
}

CONFIG_SCHEMA = output.FLOAT_OUTPUT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(M5UnitScrollLEDOutput),
        cv.Required(CONF_M5UNIT_SCROLL_ID): cv.use_id(M5UnitScroll),
        cv.Required(CONF_LED_INDEX): cv.int_range(min=0, max=1),
        cv.Required(CONF_CHANNEL): cv.enum(LED_CHANNELS),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_M5UNIT_SCROLL_ID])
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_led_index(config[CONF_LED_INDEX]))
    cg.add(var.set_channel(config[CONF_CHANNEL]))
    await output.register_output(var, config)
