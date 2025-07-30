#include <check.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#include "s21_decimal.h"

static s21_decimal random_decimal() {
  s21_decimal d;
  s21_init_decimal(&d);
  for (int i = 0; i < 3; i++) {
    d.bits[i] = rand() % 0xFFFFFFFF;
  }
  s21_set_scale(&d, rand() % 29);
  s21_set_sign(&d, rand() % 2);
  return d;
}

START_TEST(test_add) {
  s21_decimal a, b, result;
  int values[][3] = {
      {5, 10, 15},
      {-5, -10, -15},
      {0, 0, 0},
      {123456, -123456, 0},
  };

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
    s21_from_int_to_decimal(values[i][0], &a);
    s21_from_int_to_decimal(values[i][1], &b);
    int res = s21_add(a, b, &result);

    if (res == S21_OK) {
      int expected;
      s21_from_decimal_to_int(result, &expected);
      ck_assert_int_eq(expected, values[i][2]);
    }
  }

  s21_decimal max = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal one = {{1, 0, 0, 0}};
  ck_assert_int_eq(s21_add(max, one, &result), S21_ERROR_BIG);
}
END_TEST

START_TEST(test_sub) {
  s21_decimal a, b, result;
  int values[][3] = {
      {10, 5, 5},
      {-10, -5, -5},
      {0, 0, 0},
      {100, 100, 0},
  };

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
    s21_from_int_to_decimal(values[i][0], &a);
    s21_from_int_to_decimal(values[i][1], &b);
    int res = s21_sub(a, b, &result);

    if (res == S21_OK) {
      int expected;
      s21_from_decimal_to_int(result, &expected);
      ck_assert_int_eq(expected, values[i][2]);
    }
  }

  s21_decimal min = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal one = {{1, 0, 0, 0}};
  ck_assert_int_eq(s21_sub(min, one, &result), S21_ERROR_SMALL);
}
END_TEST

START_TEST(test_mul) {
  s21_decimal a, b, result;
  int values[][3] = {
      {5, 10, 50},
      {-5, 10, -50},
      {0, 100, 0},
      {12345, 6789, 12345 * 6789},
  };

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
    s21_from_int_to_decimal(values[i][0], &a);
    s21_from_int_to_decimal(values[i][1], &b);
    int res = s21_mul(a, b, &result);

    if (res == S21_OK) {
      int expected;
      s21_from_decimal_to_int(result, &expected);
      ck_assert_int_eq(expected, values[i][2]);
    }
  }

  s21_decimal max = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal two = {{2, 0, 0, 0}};
  ck_assert_int_eq(s21_mul(max, two, &result), S21_ERROR_BIG);
}
END_TEST

START_TEST(test_div) {
  s21_decimal a, b, result;
  int values[][3] = {
      {10, 2, 5},
      {-10, 2, -5},
      {0, 5, 0},
      {7, 3, 2},
  };

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
    s21_from_int_to_decimal(values[i][0], &a);
    s21_from_int_to_decimal(values[i][1], &b);
    s21_div(a, b, &result);

    int expected;
    s21_from_decimal_to_int(result, &expected);
    ck_assert_int_eq(expected, values[i][2]);
  }

  int ivalues[][3] = {
      {10, 2, 5},
      {100, 4, 25},
  };

  for (size_t i = 0; i < sizeof(ivalues) / sizeof(ivalues[0]); i++) {
    s21_from_int_to_decimal(ivalues[i][0], &a);
    s21_from_int_to_decimal(ivalues[i][1], &b);
    int res = s21_div(a, b, &result);
    ck_assert_int_eq(res, S21_OK);

    int expected;
    s21_from_decimal_to_int(result, &expected);
    ck_assert_int_eq(expected, ivalues[i][2]);
  }
}
END_TEST

START_TEST(test_comparisons) {
  s21_decimal a, b;

  s21_from_int_to_decimal(5, &a);
  s21_from_int_to_decimal(5, &b);
  ck_assert(s21_is_equal(a, b));
  ck_assert(!s21_is_not_equal(a, b));
  ck_assert(s21_is_less_or_equal(a, b));
  ck_assert(s21_is_greater_or_equal(a, b));
  ck_assert(!s21_is_less(a, b));
  ck_assert(!s21_is_greater(a, b));

  s21_from_int_to_decimal(10, &a);
  s21_from_int_to_decimal(5, &b);
  ck_assert(!s21_is_equal(a, b));
  ck_assert(s21_is_not_equal(a, b));
  ck_assert(!s21_is_less(a, b));
  ck_assert(s21_is_greater(a, b));
  ck_assert(!s21_is_less_or_equal(a, b));
  ck_assert(s21_is_greater_or_equal(a, b));

  s21_from_int_to_decimal(5, &a);
  s21_from_int_to_decimal(10, &b);
  ck_assert(!s21_is_equal(a, b));
  ck_assert(s21_is_not_equal(a, b));
  ck_assert(s21_is_less(a, b));
  ck_assert(!s21_is_greater(a, b));
  ck_assert(s21_is_less_or_equal(a, b));
  ck_assert(!s21_is_greater_or_equal(a, b));

  s21_from_int_to_decimal(-5, &a);
  s21_from_int_to_decimal(5, &b);
  ck_assert(s21_is_less(a, b));
  ck_assert(s21_is_less_or_equal(a, b));
  ck_assert(!s21_is_greater(a, b));
  ck_assert(!s21_is_greater_or_equal(a, b));

  s21_from_float_to_decimal(5.5f, &a);
  s21_from_float_to_decimal(5.6f, &b);
  ck_assert(s21_is_less(a, b));

  s21_from_float_to_decimal(1.5f, &a);
  s21_from_float_to_decimal(1.6f, &b);
  ck_assert(s21_is_less(a, b));

  s21_decimal max = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal almost_max = {{0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  ck_assert(s21_is_less(almost_max, max));
}
END_TEST

START_TEST(test_converters) {
  for (int i = -1000; i <= 1000; i += 100) {
    s21_decimal d;
    s21_from_int_to_decimal(i, &d);

    int result;
    s21_from_decimal_to_int(d, &result);
    ck_assert_int_eq(i, result);
  }

  float fvalues[] = {0.0f,     1.0f,      -1.0f, 0.5f, -0.5f,
                     123.456f, -789.012f, 1e-7f, 1e7f};

  for (size_t i = 0; i < sizeof(fvalues) / sizeof(fvalues[0]); i++) {
    // if (fabsf(fvalues[i]) < 1e-28f || fabsf(fvalues[i]) > MAX_DECIMAL) continue;

    s21_decimal d;
    int conv_res = s21_from_float_to_decimal(fvalues[i], &d);
    ck_assert_int_eq(conv_res, S21_OK);

    float result;
    s21_from_decimal_to_float(d, &result);
    ck_assert_float_eq_tol(fvalues[i], result, 1e-3);
  }

  s21_decimal d;
  ck_assert_int_eq(s21_from_float_to_decimal(0.0f, &d), S21_OK);
  ck_assert(s21_is_zero(d));

  ck_assert_int_eq(s21_from_float_to_decimal(1e-29f, &d), S21_ERROR_CONV);
  ck_assert_int_eq(s21_from_float_to_decimal(8e28f, &d), S21_ERROR_CONV);
}
END_TEST

START_TEST(test_other_functions) {
  s21_decimal tests[][2] = {
      {{{5, 0, 0, 0x00010000}}, {{0, 0, 0, 0}}},
      {{{5, 0, 0, 0x80010000}}, {{1, 0, 0, 0x80000000}}},
      {{{15, 0, 0, 0x00010000}}, {{1, 0, 0, 0}}},
      {{{15, 0, 0, 0x80010000}}, {{2, 0, 0, 0x80000000}}},
  };

  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    s21_decimal result;
    s21_floor(tests[i][0], &result);
    ck_assert(s21_is_equal(result, tests[i][1]));
  }

  s21_decimal round_tests[][2] = {
      {{{15, 0, 0, 0x00010000}}, {{2, 0, 0, 0}}},
      {{{25, 0, 0, 0x00010000}}, {{3, 0, 0, 0}}},
      {{{15, 0, 0, 0x80010000}}, {{2, 0, 0, 0x80000000}}},
      {{{25, 0, 0, 0x80010000}}, {{3, 0, 0, 0x80000000}}},
  };

  for (size_t i = 0; i < sizeof(round_tests) / sizeof(round_tests[0]); i++) {
    s21_decimal result;
    s21_round(round_tests[i][0], &result);
    ck_assert(s21_is_equal(result, round_tests[i][1]));
  }

  s21_decimal trunc_tests[][2] = {
      {{{123456, 0, 0, 0x00030000}}, {{123, 0, 0, 0}}},
      {{{123456, 0, 0, 0x80030000}}, {{123, 0, 0, 0x80000000}}},
  };

  for (size_t i = 0; i < sizeof(trunc_tests) / sizeof(trunc_tests[0]); i++) {
    s21_decimal result;
    s21_truncate(trunc_tests[i][0], &result);
    ck_assert(s21_is_equal(result, trunc_tests[i][1]));
  }

  s21_decimal neg_tests[][2] = {
      {{{5, 0, 0, 0}}, {{5, 0, 0, 0x80000000}}},
      {{{5, 0, 0, 0x80000000}}, {{5, 0, 0, 0}}},
      {{{0, 0, 0, 0}}, {{0, 0, 0, 0}}},
  };

  for (size_t i = 0; i < sizeof(neg_tests) / sizeof(neg_tests[0]); i++) {
    s21_decimal result;
    s21_negate(neg_tests[i][0], &result);
    ck_assert(s21_is_equal(result, neg_tests[i][1]));
  }
}
END_TEST

START_TEST(test_random) {
  srand(time(NULL));
  for (int i = 0; i < 100; i++) {
    s21_decimal a = random_decimal();
    s21_decimal b = random_decimal();

    s21_decimal neg_a, neg_neg_a;
    s21_negate(a, &neg_a);
    s21_negate(neg_a, &neg_neg_a);
    ck_assert(s21_is_equal(a, neg_neg_a));

    s21_decimal add1, add2;
    int res1 = s21_add(a, b, &add1);
    int res2 = s21_add(b, a, &add2);
    if (res1 == S21_OK && res2 == S21_OK) {
      ck_assert(s21_is_equal(add1, add2));
    }

    s21_decimal sub, zero = {0};
    if (s21_sub(a, a, &sub) == S21_OK) {
      ck_assert(s21_is_equal(sub, zero));
    }
  }
}
END_TEST

START_TEST(test_float_rounding) {
  float src = 0.123456789f;
  s21_decimal dec;
  ck_assert_int_eq(s21_from_float_to_decimal(src, &dec), S21_OK);
  float res;
  s21_from_decimal_to_float(dec, &res);
  ck_assert_float_eq_tol(res, 0.1234568f, 1e-7f);
}
END_TEST

START_TEST(test_float_conversion) {
  float values[] = {0.0f,     1.0f,      -1.0f,     0.5f,      -0.5f,
                    123.456f, -789.012f, 0.000001f, 1000000.0f};

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
    s21_decimal dec;
    int conv_result = s21_from_float_to_decimal(values[i], &dec);
    ck_assert_int_eq(conv_result, S21_OK);

    float result;
    s21_from_decimal_to_float(dec, &result);
    ck_assert_float_eq_tol(values[i], result, 1e-3);
  }

  s21_decimal dec;
  ck_assert_int_eq(s21_from_float_to_decimal(1e-29f, &dec), S21_ERROR_CONV);
  ck_assert_int_eq(s21_from_float_to_decimal(8e28f, &dec), S21_ERROR_CONV);
}
END_TEST

START_TEST(test_bank_rounding) {
  s21_decimal val, res;
  int ires = 0;

  s21_from_float_to_decimal(2.5f, &val);
  s21_round(val, &res);
  s21_from_decimal_to_int(res, &ires);
  ck_assert_int_eq(ires, 3);

  s21_from_float_to_decimal(3.5f, &val);
  s21_round(val, &res);
  s21_from_decimal_to_int(res, &ires);
  ck_assert_int_eq(ires, 4);

  s21_from_float_to_decimal(-2.5f, &val);
  s21_round(val, &res);
  s21_from_decimal_to_int(res, &ires);
  ck_assert_int_eq(ires, -3);
}
END_TEST

START_TEST(test_boundaries) {
  s21_decimal max = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal min = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal one = {{1, 0, 0, 0}};

  s21_decimal result;
  ck_assert_int_eq(s21_add(max, one, &result), S21_ERROR_BIG);
  ck_assert_int_eq(s21_sub(min, one, &result), S21_ERROR_SMALL);
}
END_TEST

START_TEST(test_div_edge_cases) {
  s21_decimal a, b, result;

  s21_from_int_to_decimal(10, &a);
  s21_from_int_to_decimal(3, &b);
  ck_assert_int_eq(s21_div(a, b, &result), S21_OK);
  float res;
  s21_from_decimal_to_float(result, &res);
  ck_assert_float_eq_tol(res, 3.333333f, 1e-6f);

  s21_decimal small;
  s21_init_decimal(&small);
  small.bits[0] = 1;
  s21_set_scale(&small, 7);
  s21_decimal hundred;
  s21_from_int_to_decimal(100, &hundred);
  ck_assert_int_eq(s21_div(small, hundred, &result), S21_OK);
  float small_res;
  s21_from_decimal_to_float(result, &small_res);
  ck_assert_float_eq_tol(small_res, 0.000000001f, 1e-9f);
}
END_TEST

START_TEST(test_mul_edge_cases) {
  s21_decimal a, b, result;

  s21_decimal max = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal ten = {{10, 0, 0, 0}};
  ck_assert_int_eq(s21_mul(max, ten, &result), S21_ERROR_BIG);

  s21_decimal zero = {0};
  s21_mul(max, zero, &result);
  ck_assert(s21_is_zero(result));

  s21_from_float_to_decimal(0.5f, &a);
  s21_from_float_to_decimal(0.5f, &b);
  ck_assert_int_eq(s21_mul(a, b, &result), S21_OK);
  float res;
  s21_from_decimal_to_float(result, &res);
  ck_assert_float_eq_tol(res, 0.25f, 1e-6f);
}
END_TEST

// START_TEST(test_normalization) {
//   s21_decimal a, b;
//   s21_from_float_to_decimal(1.5f, &a);
//   s21_from_float_to_decimal(2.25f, &b);

//   s21_normalize_exponents(&a, &b);
//   ck_assert_int_eq(s21_get_scale(&a), s21_get_scale(&b));
// }
// END_TEST

START_TEST(test_add_overflow_positive) {
  s21_decimal max = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_add(max, one, &result), S21_ERROR_BIG);
}
END_TEST

START_TEST(test_add_overflow_negative) {
  s21_decimal min = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal minus_one = {{1, 0, 0, 0x80000000}};
  s21_decimal result;
  ck_assert_int_eq(s21_add(min, minus_one, &result), S21_ERROR_SMALL);
}
END_TEST

START_TEST(test_sub_underflow) {
  s21_decimal min = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_sub(min, one, &result), S21_ERROR_SMALL);
}
END_TEST

START_TEST(test_mul_overflow) {
  s21_decimal max = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal two = {{2, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_mul(max, two, &result), S21_ERROR_BIG);
}
END_TEST

START_TEST(test_div_zero) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal zero = {{0, 0, 0, 0}};
  s21_decimal result;
  ck_assert_int_eq(s21_div(a, zero, &result), S21_ERROR_ZERO_DIV);
}
END_TEST

// START_TEST(test_normalization_extreme) {
//   s21_decimal a = {{123, 0, 0, 0x00020000}};
//   s21_decimal b = {{456, 0, 0, 0}};
//   s21_normalize_exponents(&a, &b);
//   ck_assert_int_eq(s21_get_scale(&a), s21_get_scale(&b));
//   ck_assert_uint_eq(b.bits[0], 45600);
// }
// END_TEST

START_TEST(test_float_precision_loss) {
  float src = 0.1234567890123456789012345678f;
  s21_decimal dec;
  ck_assert_int_eq(s21_from_float_to_decimal(src, &dec), S21_OK);

  float res;
  s21_from_decimal_to_float(dec, &res);
  ck_assert_float_lt(fabsf(src - res), 1e-7f);
}
END_TEST

START_TEST(test_round_fractional) {
  s21_decimal tests[][2] = {
      {{{123456, 0, 0, 0x00030000}}, {{123, 0, 0, 0}}},
      {{{123456, 0, 0, 0x80030000}}, {{123, 0, 0, 0x80000000}}},
      {{{5000000, 0, 0, 0x00060000}}, {{5, 0, 0, 0}}},
      {{{4999999, 0, 0, 0x00060000}}, {{5, 0, 0, 0}}}};

  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    s21_decimal result;
    s21_round(tests[i][0], &result);
    ck_assert(s21_is_equal(result, tests[i][1]));
  }
}
END_TEST

START_TEST(test_int_conversion_edge) {
  int values[] = {INT_MAX, INT_MIN, 0, -1, 1};
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
    s21_decimal dec;
    s21_from_int_to_decimal(values[i], &dec);

    int res;
    s21_from_decimal_to_int(dec, &res);
    ck_assert_int_eq(values[i], res);
  }
}
END_TEST

START_TEST(test_zero_operations) {
  s21_decimal zero = {0};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal result;

  s21_add(zero, zero, &result);
  ck_assert(s21_is_zero(result));

  s21_sub(zero, zero, &result);
  ck_assert(s21_is_zero(result));

  s21_mul(zero, one, &result);
  ck_assert(s21_is_zero(result));

  s21_div(zero, one, &result);
  ck_assert(s21_is_zero(result));

  s21_negate(zero, &result);
  ck_assert(s21_is_zero(result));
}
END_TEST

START_TEST(test_negative_operations) {
  s21_decimal a, b, result;
  s21_from_int_to_decimal(-10, &a);
  s21_from_int_to_decimal(5, &b);

  s21_add(a, b, &result);
  int res;
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, -5);

  s21_sub(a, b, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, -15);

  s21_mul(a, b, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, -50);

  s21_div(a, b, &result);
  s21_from_decimal_to_int(result, &res);
  ck_assert_int_eq(res, -2);
}
END_TEST

START_TEST(test_rounding_negative) {
  s21_decimal tests[][2] = {{{{15, 0, 0, 0x80010000}}, {{2, 0, 0, 0x80000000}}},
                            {{{25, 0, 0, 0x80010000}}, {{3, 0, 0, 0x80000000}}},
                            {{{14, 0, 0, 0x80010000}}, {{1, 0, 0, 0x80000000}}},
                            {{{0, 0, 0, 0x80000000}}, {{0, 0, 0, 0}}}};

  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    s21_decimal result;
    s21_round(tests[i][0], &result);
    ck_assert(s21_is_equal(result, tests[i][1]));
  }
}
END_TEST

START_TEST(test_fractional_arithmetic) {
  s21_decimal a, b, result;
  s21_from_float_to_decimal(0.1f, &a);
  s21_from_float_to_decimal(0.2f, &b);

  s21_add(a, b, &result);
  float res;
  s21_from_decimal_to_float(result, &res);
  ck_assert_float_eq_tol(res, 0.3f, 1e-6f);

  s21_from_float_to_decimal(0.3f, &a);
  s21_sub(a, b, &result);
  s21_from_decimal_to_float(result, &res);
  ck_assert_float_eq_tol(res, 0.1f, 1e-6f);

  s21_mul(a, b, &result);
  s21_from_decimal_to_float(result, &res);
  ck_assert_float_eq_tol(res, 0.06f, 1e-6f);

  s21_from_float_to_decimal(0.06f, &a);
  s21_from_float_to_decimal(0.02f, &b);
  s21_div(a, b, &result);
  s21_from_decimal_to_float(result, &res);
  ck_assert_float_eq_tol(res, 3.0f, 1e-6f);
}
END_TEST

START_TEST(test_float_boundaries) {
  float valid_values[] = {1e-28f,     -1e-28f,    1e14f,    -1e14f,
                          1234567.0f, -987654.0f, 0.000001f};

  float invalid_values[] = {1e-29f, -1e-29f,  8e28f,     -8e28f, 1e29f,
                            -1e29f, INFINITY, -INFINITY, NAN};

  for (size_t i = 0; i < sizeof(valid_values) / sizeof(valid_values[0]); i++) {
    s21_decimal dec;
    int result = s21_from_float_to_decimal(valid_values[i], &dec);
    ck_assert_int_eq(result, S21_OK);

    float res;
    s21_from_decimal_to_float(dec, &res);
    ck_assert_float_eq_tol(valid_values[i], res, 1e-6f);
  }

  for (size_t i = 0; i < sizeof(invalid_values) / sizeof(invalid_values[0]);
       i++) {
    s21_decimal dec;
    int result = s21_from_float_to_decimal(invalid_values[i], &dec);
    ck_assert_int_eq(result, S21_ERROR_CONV);
  }
}
END_TEST

Suite *decimal_suite(void) {
  Suite *s = suite_create("s21_decimal");

  TCase *tc_arith = tcase_create("Arithmetic");
  tcase_add_test(tc_arith, test_add);
  tcase_add_test(tc_arith, test_sub);
  tcase_add_test(tc_arith, test_mul);
  tcase_add_test(tc_arith, test_div);
  suite_add_tcase(s, tc_arith);

  TCase *tc_compare = tcase_create("Comparison");
  tcase_add_test(tc_compare, test_comparisons);
  suite_add_tcase(s, tc_compare);

  TCase *tc_convert = tcase_create("Converters");
  tcase_add_test(tc_convert, test_converters);
  tcase_add_test(tc_convert, test_float_rounding);
  suite_add_tcase(s, tc_convert);

  TCase *tc_other = tcase_create("Other Functions");
  tcase_add_test(tc_other, test_other_functions);
  tcase_add_test(tc_other, test_bank_rounding);
  suite_add_tcase(s, tc_other);

  TCase *tc_random = tcase_create("Random");
  tcase_add_test(tc_random, test_random);
  suite_add_tcase(s, tc_random);

  TCase *tc_edge = tcase_create("Edge Cases");
  tcase_add_test(tc_edge, test_boundaries);
  tcase_add_test(tc_edge, test_div_edge_cases);
  tcase_add_test(tc_edge, test_mul_edge_cases);
  //tcase_add_test(tc_edge, test_normalization);
  suite_add_tcase(s, tc_edge);

  TCase *tc_convert_float = tcase_create("Converters");
  tcase_add_test(tc_convert_float, test_float_conversion);
  tcase_add_test(tc_convert_float, test_bank_rounding);
  suite_add_tcase(s, tc_convert_float);

  TCase *tc_extended = tcase_create("Extended");
  tcase_add_test(tc_extended, test_add_overflow_positive);
  tcase_add_test(tc_extended, test_add_overflow_negative);
  tcase_add_test(tc_extended, test_sub_underflow);
  tcase_add_test(tc_extended, test_mul_overflow);
  tcase_add_test(tc_extended, test_div_zero);
  //tcase_add_test(tc_extended, test_normalization_extreme);
  tcase_add_test(tc_extended, test_float_precision_loss);
  tcase_add_test(tc_extended, test_round_fractional);
  tcase_add_test(tc_extended, test_int_conversion_edge);
  tcase_add_test(tc_extended, test_zero_operations);
  tcase_add_test(tc_extended, test_negative_operations);
  tcase_add_test(tc_extended, test_rounding_negative);
  tcase_add_test(tc_extended, test_fractional_arithmetic);
  tcase_add_test(tc_extended, test_float_boundaries);
  suite_add_tcase(s, tc_extended);

  return s;
}

int main() {
  Suite *s = decimal_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}