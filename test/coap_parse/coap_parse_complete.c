#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "coap.h"

/* This array is a result to a coap GET request. It has the following characteristics:
Version: 1d
Type: Acknowledgement (2 decimal)
Token Length: 2d
Code: 2.05 Content(69 decimal)
Message ID: 1
Token: 0x559D
Option 1:   Type: 12 (Content format)
            Length: 0
Payload: "world"
*/
uint8_t response_data[] = {0x62, 0x45, 0x00, 0x01, 0x55, 0x9D, 0xC0, 0xFF, 0x77, 0x6F, 0x72, 0x6C, 0x64};

void setUp(void) {}
void tearDown(void) {}

void parse_coap_response_header(void)
{
    coap_packet_t parsed_response = {0};
    coap_parse(&parsed_response, response_data, sizeof(response_data) / sizeof(response_data[0]));

    TEST_ASSERT_EQUAL_UINT8(1, parsed_response.hdr.ver);
    TEST_ASSERT_EQUAL_UINT8(2, parsed_response.hdr.t);
    TEST_ASSERT_EQUAL_UINT8(2, parsed_response.hdr.tkl);
    TEST_ASSERT_EQUAL_UINT8(COAP_CONTENT, parsed_response.hdr.code);
    TEST_ASSERT_EQUAL_UINT8(1, parsed_response.hdr.id);
}

void parse_coap_response_token(void)
{
    coap_packet_t parsed_response = {0};
    coap_parse(&parsed_response, response_data, sizeof(response_data) / sizeof(response_data[0]));

    TEST_ASSERT_EQUAL_UINT8(2, parsed_response.hdr.tkl);
    TEST_ASSERT_EQUAL_size_t(2, parsed_response.tok.len);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(response_data+4, parsed_response.tok.p, 2);
}

void parse_coap_response_options(void)
{
    coap_packet_t parsed_response = {0};
    coap_parse(&parsed_response, response_data, sizeof(response_data) / sizeof(response_data[0]));

    TEST_ASSERT_EQUAL_UINT8(1, parsed_response.numopts);
    TEST_ASSERT_EQUAL_UINT8(COAP_OPTION_CONTENT_FORMAT, parsed_response.opts[0].num);
    TEST_ASSERT_EQUAL_size_t(0, parsed_response.opts[0].buf.len);
}

void parse_coap_response_payload(void)
{
    coap_packet_t parsed_response = {0};
    coap_parse(&parsed_response, response_data, sizeof(response_data) / sizeof(response_data[0]));
    char expected_payload[] = "world";
    size_t expected_payload_len = strlen(expected_payload);

    TEST_ASSERT_EQUAL_size_t(expected_payload_len, parsed_response.payload.len);
    //never compare more characters than actually present in the coap payload
    TEST_ASSERT_EQUAL_STRING_LEN(expected_payload, parsed_response.payload.p, parsed_response.payload.len);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(parse_coap_response_header);
    RUN_TEST(parse_coap_response_token);
    RUN_TEST(parse_coap_response_options);
    RUN_TEST(parse_coap_response_payload);
    return UNITY_END();
}