set(COMPONENT_SRCS "main.cpp" "ui.c" "fft.c" "wifi.c" "co2.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "." "./includes")

register_component()

target_add_binary_data(${COMPONENT_TARGET} "certs/aws-root-ca.pem" TEXT)