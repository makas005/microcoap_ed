add_executable(coap_parse_complete_app
    coap_parse_complete.c
)

target_link_libraries(coap_parse_complete_app
    microcoap_ed
    Unity
)

add_test(coap_parse_complete coap_parse_complete_app)

add_executable(coap_get_blockwise_option_information_app
    coap_get_blockwise_option_information.c
)

target_link_libraries(coap_get_blockwise_option_information_app
    microcoap_ed
    Unity
)

add_test(coap_get_blockwise_option_information coap_get_blockwise_option_information_app)