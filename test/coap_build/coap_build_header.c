#include "unity.h"
#include "coap.h"
#include <string.h>

static uint8_t buf[128] = {};
static size_t buflen = 128;
static coap_packet_t pkt = {};
static const coap_packet_t pkt_zero = {};
static coap_error_t ret;

void setUp(void)
{
    memset(buf, 0, 128);
    buflen = 128;
    int s = sizeof(coap_packet_t);
    pkt = pkt_zero;
    ret = COAP_ERR_NONE;
}

void tearDown(void) {}

void header_has_correct_size(void) {
    pkt.hdr.ver = 1;
    pkt.hdr.t = COAP_TYPE_CON;
    pkt.hdr.tkl = 0;
    pkt.hdr.id = 0x0001;
    coap_build(buf, &buflen, &pkt);
    TEST_ASSERT_EQUAL_size_t(4, buflen);
}

void header_version_other_than_one_returns_error(void) {
    pkt.hdr.ver = 1;
    pkt.hdr.t = COAP_TYPE_CON;
    pkt.hdr.tkl = 0;
    pkt.hdr.id = 0x0001;
    ret = coap_build(buf, &buflen, &pkt);
    TEST_ASSERT_EQUAL_INT(COAP_ERR_NONE, ret);  

    memset(buf, 0, 128);
    pkt.hdr.ver = 2;
    ret = coap_build(buf, &buflen, &pkt);
    TEST_ASSERT_EQUAL_INT(COAP_ERR_VERSION_NOT_1, ret);

    memset(buf, 0, 128);
    pkt.hdr.ver = 10;
    ret = coap_build(buf, &buflen, &pkt);
    TEST_ASSERT_EQUAL_INT(COAP_ERR_VERSION_NOT_1, ret);
}

void specified_token_length_differs_from_actual_size_returns_error(void) {
    pkt.hdr.ver = 1;
    pkt.hdr.t = COAP_TYPE_CON;
    pkt.hdr.tkl = 1;
    pkt.hdr.id = 0x0001;
    ret = coap_build(buf, &buflen, &pkt);    
    TEST_ASSERT_EQUAL_INT(COAP_ERR_TOKEN_LENGTH_MISMATCH, ret);
}

void token_length_above_8_returns_error(void) {
    uint8_t token_buf[9] = {0};
    
    pkt.hdr.ver = 1;
    pkt.hdr.t = COAP_TYPE_CON;
    pkt.hdr.tkl = 9;
    pkt.tok.p = token_buf;
    pkt.tok.len = 9;
    pkt.hdr.id = 0x0001;
    ret = coap_build(buf, &buflen, &pkt);    
    TEST_ASSERT_EQUAL_INT(COAP_ERR_TOKEN_TOO_LONG, ret);
}

void one_byte_token_set_correct(void) {
    uint8_t token = 0x1A;

    pkt.hdr.ver = 1;
    pkt.hdr.t = COAP_TYPE_CON;
    pkt.hdr.tkl = 1;
    pkt.tok.p = &token;
    pkt.tok.len = 1;
    pkt.hdr.id = 0x0001;
    ret = coap_build(buf, &buflen, &pkt);
    TEST_ASSERT_EQUAL_HEX8(0x1A, buf[4]);
}

void two_byte_token_set_correct(void) {
    uint8_t token[2] = {0x1A, 0x2B};

    pkt.hdr.ver = 1;
    pkt.hdr.t = COAP_TYPE_CON;
    pkt.hdr.tkl = 2;
    pkt.tok.p = token;
    pkt.tok.len = 2;
    pkt.hdr.id = 0x0001;
    ret = coap_build(buf, &buflen, &pkt);
    TEST_ASSERT_EQUAL_HEX8(0x1A, buf[4]);
    TEST_ASSERT_EQUAL_HEX8(0x2B, buf[5]);
}

void three_byte_token_set_correct(void) {
    uint8_t token[3] = {0x1A, 0x2B, 0x3C};

    pkt.hdr.ver = 1;
    pkt.hdr.t = COAP_TYPE_CON;
    pkt.hdr.tkl = 3;
    pkt.tok.p = token;
    pkt.tok.len = 3;
    pkt.hdr.id = 0x0001;
    ret = coap_build(buf, &buflen, &pkt);
    TEST_ASSERT_EQUAL_HEX8(0x1A, buf[4]);
    TEST_ASSERT_EQUAL_HEX8(0x2B, buf[5]);
    TEST_ASSERT_EQUAL_HEX8(0x3C, buf[6]);
}

void eight_byte_token_set_correct(void) {
    uint8_t token[8] = {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F, 0xB0, 0x55};

    pkt.hdr.ver = 1;
    pkt.hdr.t = COAP_TYPE_CON;
    pkt.hdr.tkl = 8;
    pkt.tok.p = token;
    pkt.tok.len = 8;
    pkt.hdr.id = 0x0001;
    ret = coap_build(buf, &buflen, &pkt);
    TEST_ASSERT_EQUAL_HEX8(0x1A, buf[4]);
    TEST_ASSERT_EQUAL_HEX8(0x2B, buf[5]);
    TEST_ASSERT_EQUAL_HEX8(0x3C, buf[6]);   
    TEST_ASSERT_EQUAL_HEX8(0x4D, buf[7]);
    TEST_ASSERT_EQUAL_HEX8(0x5E, buf[8]); 
    TEST_ASSERT_EQUAL_HEX8(0x6F, buf[9]);
    TEST_ASSERT_EQUAL_HEX8(0xB0, buf[10]);
    TEST_ASSERT_EQUAL_HEX8(0x55, buf[11]);
}

void message_id_set_correct(void) {
    pkt.hdr.ver = 1;
    pkt.hdr.t = COAP_TYPE_CON;
    pkt.hdr.tkl = 0;
    pkt.hdr.id = 0xBEEF;
    ret = coap_build(buf, &buflen, &pkt);
    //Network byte order is big endian, device is little endian. Test if conversion is correct
    TEST_ASSERT_EQUAL_HEX8(0xBE, buf[2]);
    TEST_ASSERT_EQUAL_HEX8(0xEF, buf[3]);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(header_has_correct_size);
    RUN_TEST(header_version_other_than_one_returns_error);
    RUN_TEST(specified_token_length_differs_from_actual_size_returns_error);
    RUN_TEST(token_length_above_8_returns_error);
    RUN_TEST(one_byte_token_set_correct);
    RUN_TEST(two_byte_token_set_correct);
    RUN_TEST(eight_byte_token_set_correct);
    RUN_TEST(message_id_set_correct);
    return UNITY_END();
}