#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "coap.h"

static coap_packet_t packet = {};
static uint8_t obuf[256] = {0};
static size_t obuf_size = sizeof(obuf) / sizeof(obuf[0]);

void setUp(void) {
    //Prepare coap PDU
    coap_header_init(&packet, COAP_TYPE_CON, COAP_GET, 5);
}

void tearDown(void) {
    //Override packet with zero packet, therefore "deleting" it
    const coap_packet_t zero_packet = {};
    packet = zero_packet;

    //Delete output buffer
    memset(obuf, 0, sizeof(obuf));
    //Reset output buffer size
    obuf_size = sizeof(obuf) / sizeof(obuf[0]);
}

void block2_is_only_option_get_information(void)
{
    uint8_t block2[3] = {0};
    uint8_t block2_size = 0;
    block2_size = coap_make_option_blockwise(block2, COAP_BLOCKSIZE_16, true, 6);
    coap_add_option(&packet, COAP_OPTION_BLOCK_2, block2, block2_size);
    coap_error_t build_result = coap_build(obuf, &obuf_size, &packet);
    TEST_ASSERT_EQUAL_MESSAGE(COAP_ERR_NONE, build_result, "Coap build failed!");

    coap_packet_t read_packet = {};
    coap_parse(&read_packet, obuf, obuf_size);
    uint8_t count = 0;
    const coap_option_t *block_option = coap_findOptions(&read_packet, COAP_OPTION_BLOCK_2, &count);
    coap_blocksize_t szx = coap_option_blockwise_get_szx(block_option);
    TEST_ASSERT_EQUAL(COAP_BLOCKSIZE_16, szx);
    bool m = coap_option_blockwise_get_m(block_option);
    TEST_ASSERT_EQUAL(true, m);
    uint32_t num = coap_option_blockwise_get_num(block_option);
    TEST_ASSERT_EQUAL(6, num);
}

void block2_is_not_only_option_get_information(void)
{
    char path[] = "path";
    coap_add_option(&packet, COAP_OPTION_URI_PATH, (uint8_t*)path, strlen(path));

    char query[] = "query=123455";
    coap_add_option(&packet, COAP_OPTION_URI_QUERY, (uint8_t*)query, strlen(query));

    uint8_t block2[3] = {0};
    uint8_t block2_size = 0;
    block2_size = coap_make_option_blockwise(block2, COAP_BLOCKSIZE_16, true, 6);
    coap_add_option(&packet, COAP_OPTION_BLOCK_2, block2, block2_size);
    coap_error_t build_result = coap_build(obuf, &obuf_size, &packet);
    TEST_ASSERT_EQUAL_MESSAGE(COAP_ERR_NONE, build_result, "Coap build failed!");

    coap_packet_t read_packet = {};
    coap_parse(&read_packet, obuf, obuf_size);
    uint8_t count = 0;
    const coap_option_t *block_option = coap_findOptions(&read_packet, COAP_OPTION_BLOCK_2, &count);
    coap_blocksize_t szx = coap_option_blockwise_get_szx(block_option);
    TEST_ASSERT_EQUAL(COAP_BLOCKSIZE_16, szx);
    bool m = coap_option_blockwise_get_m(block_option);
    TEST_ASSERT_EQUAL(true, m);
    uint32_t num = coap_option_blockwise_get_num(block_option);
    TEST_ASSERT_EQUAL(6, num);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(block2_is_only_option_get_information);
    RUN_TEST(block2_is_not_only_option_get_information);
    return UNITY_END();
}