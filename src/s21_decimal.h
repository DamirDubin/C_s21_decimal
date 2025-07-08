#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <math.h>
#include <stdint.h>

typedef struct {
  int bits[4];
} s21_decimal;

typedef union {
  float f;
  struct {
    uint32_t mantissa : 23;
    uint32_t exponent : 8;
    uint32_t sign : 1;
  } parts;
} float_parser;

#define S21_OK 0
#define S21_ERROR_BIG 1
#define S21_ERROR_SMALL 2
#define S21_ERROR_ZERO_DIV 3
#define S21_ERROR_CONV 1

#define S21_SIGN_MASK 0b10000000000000000000000000000000
#define S21_EXP_MASK 0b00000000111111110000000000000000
#define S21_EXP_SHIFT 16
#define S21_MIN_EXP 0
#define S21_MAX_EXP 28
#define S21_LAST_BIT 95
#define S21_MAX_UINT 0b11111111111111111111111111111111
#define MAX_DECIMAL 79228162514264337593543950335.0

#define s21_set_sign(dec, sign)                             \
  ((dec)->bits[3] = (sign) ? (dec)->bits[3] | S21_SIGN_MASK \
                           : (dec)->bits[3] & ~S21_SIGN_MASK)
#define s21_get_sign(dec) (!!((dec)->bits[3] & S21_SIGN_MASK))

#define s21_set_exp(dec, exp) \
  ((dec)->bits[3] = ((dec)->bits[3] & ~S21_EXP_MASK) | ((exp) << S21_EXP_SHIFT))
#define s21_get_exp(dec) (((dec)->bits[3] & S21_EXP_MASK) >> S21_EXP_SHIFT)

#define s21_is_zero(dec) (!(dec)->bits[0] && !(dec)->bits[1] && !(dec)->bits[2])

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal dividend, s21_decimal divisor, s21_decimal *result);

int s21_is_less(s21_decimal a, s21_decimal b);
int s21_is_less_or_equal(s21_decimal a, s21_decimal b);
int s21_is_greater(s21_decimal a, s21_decimal b);
int s21_is_greater_or_equal(s21_decimal a, s21_decimal b);
int s21_is_equal(s21_decimal s, s21_decimal b);
int s21_is_not_equal(s21_decimal a, s21_decimal b);

int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

void s21_init_decimal(s21_decimal *dec);
void s21_copy(s21_decimal src, s21_decimal *dst);
int s21_get_bit(s21_decimal dec, int bit);
int s21_shift_left(s21_decimal *dec);
int s21_shift_right(s21_decimal *dec);
int s21_mantissa_less(s21_decimal a, s21_decimal b);
int s21_mul_by_10(s21_decimal *value);
int s21_div_by_10(s21_decimal *value);
void s21_normalize_exponents(s21_decimal *a, s21_decimal *b);
void s21_integer_division(s21_decimal dividend, s21_decimal divisor,
                          s21_decimal *quotient, s21_decimal *remainder);
void s21_handle_division_remainder(s21_decimal divisor, s21_decimal *remainder,
                                   s21_decimal *fractional,
                                   int *fractional_digits, int max_scale);

#endif