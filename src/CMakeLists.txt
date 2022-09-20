add_library(microcoap_ed STATIC
    coap.c
)

target_include_directories(microcoap_ed PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)