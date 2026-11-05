import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import media_source, microphone
from esphome.const import CONF_ID

CONF_MICROPHONE = "microphone"
CONF_SAMPLE_RATE = "sample_rate"
CONF_CHANNELS = "channels"
CONF_BUFFER_SIZE = "buffer_size"

DEPENDENCIES = ["microphone"]
AUTO_LOAD = ["audio", "media_source"]

line_in_ns = cg.esphome_ns.namespace("line_in")
LineInMediaSource = line_in_ns.class_(
    "LineInMediaSource", cg.Component, media_source.MediaSource
)

CONFIG_SCHEMA = media_source.media_source_schema(LineInMediaSource).extend(
    {
        cv.Required(CONF_MICROPHONE): cv.use_id(microphone.Microphone),
        cv.Optional(CONF_SAMPLE_RATE, default=48000): cv.int_range(min=8000, max=96000),
        cv.Optional(CONF_CHANNELS, default=2): cv.one_of(1, 2, int=True),
        cv.Optional(CONF_BUFFER_SIZE, default=32768): cv.int_range(min=4096, max=262144),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await media_source.register_media_source(var, config)
    mic = await cg.get_variable(config[CONF_MICROPHONE])
    cg.add(var.set_microphone(mic))
    cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))
    cg.add(var.set_channels(config[CONF_CHANNELS]))
    cg.add(var.set_buffer_size(config[CONF_BUFFER_SIZE]))
