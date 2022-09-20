#include "unity.h"
#include "coap.h"

void setUp(void) {}
void tearDown(void) {}

void one_option_returns_index_0(void)
{
    coap_option_t o ={1, NULL};
    uint8_t index;

    coap_order_options(&o, 1, &index);
    TEST_ASSERT_EQUAL_UINT8(0, index);
}

void two_options_in_order_return_indices_in_order(void)
{
    coap_option_t o[2] ={{1, NULL}, {2, NULL}};
    uint8_t indices[2];
    coap_order_options(o, 2, indices);
    TEST_ASSERT_EQUAL_UINT8(0, indices[0]);
    TEST_ASSERT_EQUAL_UINT8(1, indices[1]);
}

void two_options_in_reverse_order_return_indices_in_order(void)
{
    coap_option_t o[2] ={{2, NULL}, {1, NULL}};
    uint8_t indices[2];
    coap_order_options(o, 2, indices);
    TEST_ASSERT_EQUAL_UINT8(1, indices[0]);
    TEST_ASSERT_EQUAL_UINT8(0, indices[1]);    
}

void two_options_with_same_order_return_indices_in_order(void)
{
    coap_option_t o[2] ={{2, NULL}, {2, NULL}};
    uint8_t indices[2];
    coap_order_options(o, 2, indices);
    TEST_ASSERT_EQUAL_UINT8(0, indices[0]);
    TEST_ASSERT_EQUAL_UINT8(1, indices[1]);    
}

void three_options_in_order_return_indices_in_order(void)
{
    coap_option_t o[3] ={{1, NULL}, {2, NULL}, {3, NULL}};
    uint8_t indices[3];
    coap_order_options(o, 3, indices);
    TEST_ASSERT_EQUAL_UINT8(0, indices[0]);
    TEST_ASSERT_EQUAL_UINT8(1, indices[1]);
    TEST_ASSERT_EQUAL_UINT8(2, indices[2]);
}

void three_options_in_order_with_two_same_numbers_return_indices_in_order(void)
{
    coap_option_t o[3] ={{1, NULL}, {1, NULL}, {3, NULL}};
    uint8_t indices[3];
    coap_order_options(o, 3, indices);
    TEST_ASSERT_EQUAL_UINT8(2, indices[2]);
    //because first and second option has the same number, both sort outcomes are acceptable
    TEST_ASSERT((0 == indices[0] && 1 == indices[1] ) || (1 == indices[0] && 0 == indices[1]));
}

void three_options_in_reverse_order_return_indices_in_order(void)
{
    coap_option_t o[3] ={{3, NULL}, {2, NULL}, {1, NULL}};
    uint8_t indices[3];
    coap_order_options(o, 3, indices);
    TEST_ASSERT_EQUAL_UINT8(2, indices[0]);
    TEST_ASSERT_EQUAL_UINT8(1, indices[1]);
    TEST_ASSERT_EQUAL_UINT8(0, indices[2]);
}

void three_options_in_reverse_order_with_two_same_numbers_return_indices_in_order(void)
{
    coap_option_t o[3] ={{3, NULL}, {1, NULL}, {1, NULL}};
    uint8_t indices[3];
    coap_order_options(o, 3, indices);
    TEST_ASSERT_EQUAL_UINT8(0, indices[2]);
    //because second and third option has the same number, both sort outcomes are acceptable
    TEST_ASSERT((1 == indices[1] && 2 == indices[0]) || (2 == indices[1] && 1 == indices[0]));
}

void ten_random_option_numbers_with_duplicates(void)
{
    coap_option_t o[10] ={{3, NULL}, {1, NULL}, {8, NULL}, {15, NULL}, {8, NULL}, {2, NULL}, {9, NULL}, {1, NULL}, {19, NULL}, {4, NULL}};
    uint8_t indices[10];
    coap_order_options(o, 10, indices);
    TEST_ASSERT_EQUAL_UINT8(5, indices[2]);
    TEST_ASSERT_EQUAL_UINT8(0, indices[5]);
    TEST_ASSERT_EQUAL_UINT8(9, indices[4]);
    TEST_ASSERT_EQUAL_UINT8(6, indices[7]);
    TEST_ASSERT_EQUAL_UINT8(3, indices[8]);
    TEST_ASSERT_EQUAL_UINT8(8, indices[9]);
    //because second and third option has the same number, both sort outcomes are acceptable
    TEST_ASSERT((1 == indices[0] && 7 == indices[1]) || (1 == indices[1] && 7 == indices[0]));
    TEST_ASSERT((2 == indices[5] && 4 == indices[6]) || (2 == indices[6] && 4 == indices[5]));    
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(one_option_returns_index_0);
  RUN_TEST(two_options_in_order_return_indices_in_order);
  RUN_TEST(two_options_in_reverse_order_return_indices_in_order);
  RUN_TEST(two_options_with_same_order_return_indices_in_order);
  RUN_TEST(three_options_in_order_return_indices_in_order);
  RUN_TEST(three_options_in_order_with_two_same_numbers_return_indices_in_order);
  RUN_TEST(three_options_in_reverse_order_return_indices_in_order);
  RUN_TEST(three_options_in_reverse_order_with_two_same_numbers_return_indices_in_order);
  return UNITY_END();
}