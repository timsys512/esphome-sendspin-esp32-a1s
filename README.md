# esphome-sendspin-esp32-a1s

# The YAML expects the custom line_in component from a1s-line-in-package

/config/esphome/
├── esphome-web-267520-complete.yaml
└── components/
    └── line_in/
        ├── __init__.py
        └── media_source/
            ├── __init__.py
            ├── line_in_media_source.h
            └── line_in_media_source.cpp


# The YAML references it with:
external_components:
  - source: github://PxPert/a2dp_sink@master
    components: [a2dp_sink]

  - source:
      type: local
      path: components
    components: [line_in]
