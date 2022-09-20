add_executable(coap_order_options_app
    coap_order_options.c
)

target_link_libraries(coap_order_options_app
    microcoap_ed
    Unity
)

add_test(coap_order_options coap_order_options_app)