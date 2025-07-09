#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  int bits[4];
} s21_decimal;

#define S21_OK 0
#define S21_ERROR_BIG 1
#define S21_ERROR_SMALL 2
#define S21_ERROR_ZERO_DIV 3
#define S21_ERROR_CONV 1

#define S21_SIGN_MASK (1U << 31)   // 0x80000000
#define S21_EXP_MASK (255U << 16)  // 0x00FF0000
#define S21_EXP_SHIFT 16
#define S21_MIN_EXP 0
#define S21_MAX_EXP 28
#define S21_LAST_BIT 95
#define S21_MAX_DECIMAL 79228162514264337593543950335.0

#define s21_set_sign(dec, sign)                             \
  ((dec)->bits[3] = (sign) ? (dec)->bits[3] | S21_SIGN_MASK \
                           : (dec)->bits[3] & ~S21_SIGN_MASK)
#define s21_get_sign(dec) (!!((dec)->bits[3] & S21_SIGN_MASK))

#define s21_set_exp(dec, exp) \
  ((dec)->bits[3] = ((dec)->bits[3] & ~S21_EXP_MASK) | ((exp) << S21_EXP_SHIFT))
#define s21_get_exp(dec) (((dec)->bits[3] & S21_EXP_MASK) >> S21_EXP_SHIFT)

#define s21_is_zero(dec) (!(dec)->bits[0] && !(dec)->bits[1] && !(dec)->bits[2])

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

void s21_init_decimal(s21_decimal *dec) {
  for (int i = 0; i < 4; i++) dec->bits[i] = 0;
}

int s21_shift_left(s21_decimal *dec) {
  int carry = 0;
  for (int i = 0; i < 3; i++) {  // только мантисса (0-2)
    uint32_t value = (uint32_t)dec->bits[i];
    uint64_t extended = (uint64_t)value << 1 | carry;
    dec->bits[i] = (int)(extended & UINT32_MAX);
    carry = extended >> 32;
  }
  return carry;
}

int s21_div_by_10(s21_decimal *value) {
  uint64_t remainder = 0;
  for (int i = 2; i >= 0; i--) {
    uint64_t digit = (remainder << 32) |
                     (uint32_t)value->bits[i];  // формируем 64-бит значение
    value->bits[i] = (uint32_t)(digit / 10);  // целая часть
    remainder = digit % 10;                   // остаток
  }
  return (int)remainder;  // возвращаем остаток от деления
}

void s21_mul_by_10(s21_decimal *value) {
  s21_decimal a, b;
  a = *value;
  b = *value;
  for (int i = 0; i < 3; i++) s21_shift_left(&a);
  s21_shift_left(&b);
  s21_add(a, b, value);
}

void s21_normalize_exponents(s21_decimal *a, s21_decimal *b) {
  int exp_a = s21_get_exp(a);
  int exp_b = s21_get_exp(b);

  while (exp_a != exp_b) {
    if (exp_a < exp_b) {  // для а
      s21_mul_by_10(a);   // увеличиваем а
      exp_a++;
      s21_set_exp(a, exp_a);
    } else {  // для б
      s21_mul_by_10(b);
      exp_b++;
      s21_set_exp(b, exp_b);
    }
  }
}

int s21_mantissa_less(s21_decimal a, s21_decimal b) {
  for (int i = 2; i >= 0; i--) {  // сравниваем от старших слов к младшим
    if (a.bits[i] != b.bits[i]) {
      return a.bits[i] < b.bits[i];  // нашли различие - возвращаем результат
    }
  }
  return 0;  // равны
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = 0;
  s21_init_decimal(result);

  if (s21_is_zero(&value_1) && s21_is_zero(&value_2)) return status;

  s21_normalize_exponents(&value_1, &value_2);
  int exp = s21_get_exp(&value_1);
  s21_set_exp(result, exp);

  int sign1 = s21_get_sign(&value_1);
  int sign2 = s21_get_sign(&value_2);
  int result_sign = 0;

  if (sign1 == sign2) {  // одинаковые знаки значит плюсуем
    uint64_t carry = 0;
    for (int i = 0; i < 3; i++) {  // сложение мантисс с переносом
      uint64_t ua = (uint32_t)value_1.bits[i];
      uint64_t ub = (uint32_t)value_2.bits[i];
      uint64_t sum = ua + ub + carry;
      result->bits[i] = (uint32_t)(sum & UINT32_MAX);
      carry = sum >> 32;
    }
    result_sign = sign1;

    if (carry) {
      status = result_sign ? 1 : 2;
    }
  } else {  // разные знаки значит вычитаем
    if (s21_mantissa_less(value_1, value_2)) {  // меняем местами
      s21_decimal tmp = value_1;
      value_1 = value_2;
      value_2 = tmp;
      result_sign = sign2;
    } else {
      result_sign = sign1;
    }

    uint64_t borrow = 0;
    for (int i = 0; i < 3; i++) {  // вычитаем из большего меньшее
      uint64_t ua = (uint32_t)value_1.bits[i];
      uint64_t ub = (uint32_t)value_2.bits[i];
      uint64_t diff = ua - ub - borrow;  // вычитаем заем
      borrow = (diff > ua) ? 1 : 0;  // занимаем 1 из старшего разряда
      result->bits[i] = (uint32_t)diff;
    }
    if (s21_is_zero(result)) result_sign = 0;
  }

  if (status == 0) s21_set_sign(result, result_sign);
  return status;
}

void binary_print(s21_decimal x, char *buf) {
  int pos = 0;
  for (int i = 2; i >= 0; i--) {
    for (int j = 31; j >= 0; j--) {
      buf[pos++] = (x.bits[i] & (1U << j)) ? '1' : '0';
    }
    buf[pos++] = ' ';  // Добавляем пробел между словами
  }
  buf[pos - 1] = '\0';  // Заменяем последний пробел на \0
}

int main() {
  // unsigned int a = pow(2, 32) - 1;
  // char *res;
  // binary_print(a, res);
  // printf("%s", res);

  // unsigned char a = 2;
  // unsigned char b = (a << 3) + (a << 1);

  // printf("%d\n", b);
  char res[129];
  s21_decimal a = {5, 0, 0, 0};
  s21_mul_by_10(&a);
  binary_print(a, res);
  printf("%s\n", res);
}
