#include "unity.h"
#include "coap.h"

void setUp(void) {}
void tearDown(void) {}

void blockwise_zero_byte(void)
{
    uint8_t option_buffer[3] = {0};
    uint8_t option_size = 0;

    option_size = coap_make_option_blockwise(option_buffer, 0, false, 0);

    TEST_ASSERT_EQUAL_UINT8(0, 0);
    uint8_t expected_array[3] = {0,0,0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_array, option_buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(1, option_size);
}

void only_size_set(void)
{
    uint8_t option_buffer[3] = {0};
    uint8_t option_size = 0;

    option_size = coap_make_option_blockwise(option_buffer, COAP_BLOCKSIZE_256, false, 0);

    TEST_ASSERT_EQUAL_UINT8(0, 0);
    uint8_t expected_array[3] = {0x04,0x00,0x00};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_array, option_buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(1, option_size);
}

void size_and_more_flag_set(void)
{
    uint8_t option_buffer[3] = {0};
    uint8_t option_size = 0;

    option_size = coap_make_option_blockwise(option_buffer, COAP_BLOCKSIZE_256, true, 0);

    TEST_ASSERT_EQUAL_UINT8(0, 0);
    uint8_t expected_array[3] = {0x0C,0x00,0x00};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_array, option_buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(1, option_size);
}

void four_byte_num_and_size_and_more_flag_set(void)
{
    uint8_t option_buffer[3] = {0};
    uint8_t option_size = 0;

    option_size = coap_make_option_blockwise(option_buffer, COAP_BLOCKSIZE_256, true, 6);

    TEST_ASSERT_EQUAL_UINT8(0, 0);
    uint8_t expected_array[3] = {0x6C,0x00,0x00};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_array, option_buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(1, option_size);
}


void twelve_byte_num_and_size_and_more_flag_set(void)
{
    uint8_t option_buffer[3] = {0};
    uint8_t option_size = 0;

    option_size = coap_make_option_blockwise(option_buffer, COAP_BLOCKSIZE_256, true, 1337);
    //1337d = 539h

    TEST_ASSERT_EQUAL_UINT8(0, 0);
    uint8_t expected_array[3] = {0x53,0x9C,0x00};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_array, option_buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(2, option_size);
}

void twenty_byte_num_and_size_and_more_flag_set(void)
{
    uint8_t option_buffer[3] = {0};
    uint8_t option_size = 0;

    option_size = coap_make_option_blockwise(option_buffer, COAP_BLOCKSIZE_256, true, 987654); 
    //987654d = hF1206

    TEST_ASSERT_EQUAL_UINT8(0, 0);
    uint8_t expected_array[3] = {0xF1,0x20,0x6C};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_array, option_buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(3, option_size);
}



int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(blockwise_zero_byte);
  RUN_TEST(only_size_set);
  return UNITY_END();
}