#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <limits.h>
#include <math.h>  // fabs() pow()
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define S21_OK 0
#define MAX_DECIMAL 79228162514264337593543950335.0
#define S21_ERROR_CONV 1
#define S21_ERROR_BIG 1
#define S21_ERROR_SMALL 2
#define S21_ERROR_CALC 1
#define S21_NULL ((void*)0)

typedef struct {
  int bits[4];
} s21_decimal;

typedef struct {
  unsigned int bits[8];
} s21_big_decimal;

// База Талгата
void s21_shift_left(s21_big_decimal* num, int shift_value);
void s21_bitwise_addition(s21_big_decimal* value_1, s21_big_decimal* value_2,
                          s21_big_decimal* result);
void s21_big_set_bit(s21_big_decimal* value, int bit_number, int bit_value);
int s21_big_get_bit(s21_big_decimal* value, int bit_number);
void s21_set_sign(s21_decimal* az, int sign_value);
int s21_get_sign(s21_decimal* az);
void s21_set_scale(s21_decimal* az, int scale_value);
int s21_get_scale(s21_decimal* az);
void s21_from_decimal_to_big(s21_decimal az, s21_big_decimal* big);
int s21_from_big_to_decimal(s21_big_decimal big, s21_decimal* dec);
int s21_is_out_of_96_bits(s21_big_decimal d);
void s21_big_subtraction(s21_big_decimal* a, s21_big_decimal* b,
                         s21_big_decimal* res);
void s21_big_negate(s21_big_decimal* num, s21_big_decimal* result);
int s21_big_is_greater_or_equal(s21_big_decimal a, s21_big_decimal b);
int s21_reduce_and_round(s21_big_decimal* big, int* scale);
void s21_mul_ten(s21_big_decimal* t);
void s21_normalize_big_decimals(s21_big_decimal* a, s21_big_decimal* b,
                                int* scale_a, int* scale_b);
int last_digit(s21_big_decimal* big, int z);
void s21_div_big_10(s21_big_decimal* a, s21_big_decimal* result);
int s21_find_oldest_positive_bit(s21_big_decimal* a);
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_big_is_zero(s21_big_decimal* val);

// Азовы функции
int s21_get_bit(s21_decimal dec, int bit);
int s21_from_int_to_decimal(int src, s21_decimal* dst);
int s21_from_float_to_decimal(float src, s21_decimal* dst);
int s21_from_decimal_to_int(s21_decimal src, int* dst);
int s21_from_decimal_to_float(s21_decimal src, float* dst);

// Ивангелие от Yana
int s21_is_less(s21_decimal a, s21_decimal b);
int s21_abs_compare(s21_big_decimal a, s21_big_decimal b);
int s21_is_equal(s21_decimal, s21_decimal);
int s21_is_zero(s21_decimal dec);
int s21_is_less_or_equal(s21_decimal a, s21_decimal b);
int s21_is_greater(s21_decimal a, s21_decimal b);
int s21_is_not_equal(s21_decimal a, s21_decimal b);
int s21_is_greater_or_equal(s21_decimal a, s21_decimal b);

// Damir theme
int s21_floor(s21_decimal value, s21_decimal* result);
int s21_round(s21_decimal value, s21_decimal* result);
int s21_truncate(s21_decimal value, s21_decimal* result);
int s21_negate(s21_decimal value, s21_decimal* result);

// Help functions
void s21_init_decimal(s21_decimal* dec);
int s21_div_by_10(s21_decimal* value);

#endif