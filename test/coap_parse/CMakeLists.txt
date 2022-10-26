add_executable(coap_parse_complete_app
    coap_parse_complete.c
)

target_link_libraries(coap_parse_complete_app
    microcoap_ed
    Unity
)

add_test(coap_parse_complete coap_parse_complete_app)