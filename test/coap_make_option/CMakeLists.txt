add_executable(coap_make_option_blockwise_app
    coap_make_option_blockwise.c
)

target_link_libraries(coap_make_option_blockwise_app
    microcoap_ed
    Unity
)

add_test(coap_make_option_blockwise coap_make_option_blockwise_app)