#include "s21_decimal.h"

void s21_init_decimal(s21_decimal *dec) {
  for (int i = 0; i < 4; i++) dec->bits[i] = 0;
}

void s21_copy(s21_decimal src, s21_decimal *dst) {
  memcpy(dst->bits, src.bits, sizeof(src.bits));
}

int s21_get_bit(s21_decimal dec, int bit) {
  int word = bit / 32;  // определяем в каком слове находится бит (0-3)
  int offset = bit % 32;  // позиция бита внутри слова
  return !!(dec.bits[word] & (1U << offset));  // извлекаем и возвращаем бит
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

int s21_shift_right(s21_decimal *dec) {
  uint32_t carry = 0;
  for (int i = 2; i >= 0; i--) {
    uint32_t value = (uint32_t)dec->bits[i];
    uint32_t new_value = (value >> 1) | carry;
    carry = (value & 1) ? S21_SIGN_MASK : 0;
    dec->bits[i] = (int)new_value;
  }
  return carry ? 1 : 0;
}

int s21_mantissa_less(s21_decimal a, s21_decimal b) {
  for (int i = 2; i >= 0; i--) {  // сравниваем от старших слов к младшим
    if (a.bits[i] != b.bits[i]) {
      return a.bits[i] < b.bits[i];  // нашли различие - возвращаем результат
    }
  }
  return 0;  // равны
}

int s21_mul_by_10(s21_decimal *value) {
  uint64_t carry = 0;
  for (int i = 0; i < 3; i++) {  // для каждого слова мантиссы
    uint64_t digit = (uint32_t)value->bits[i];
    digit = digit * 10 + carry;
    value->bits[i] = (uint32_t)(digit & UINT32_MAX);  // сохраняем
                                                      // 32 бита
    carry = digit >> 32;
  }
  return carry ? 1 : 0;  // 1 если переполнение
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

void s21_normalize_exponents(s21_decimal *a, s21_decimal *b) {
  int exp_a = s21_get_exp(a);
  int exp_b = s21_get_exp(b);

  while (exp_a != exp_b) {
    if (exp_a < exp_b) {            // для а
      if (s21_mul_by_10(a) == 0) {  // увеличиваем а
        s21_set_exp(a, ++exp_a);
      } else {  // если переполнение уменьшаем б
        s21_div_by_10(b);
        s21_set_exp(b, --exp_b);
      }
    } else {  // для б
      if (s21_mul_by_10(b) == 0) {
        s21_set_exp(b, ++exp_b);
      } else {
        s21_div_by_10(a);
        s21_set_exp(a, --exp_a);
      }
    }
  }
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = S21_OK;
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
      status = result_sign ? S21_ERROR_SMALL : S21_ERROR_BIG;
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

  if (status == S21_OK) s21_set_sign(result, result_sign);
  return status;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_negate(value_2, &value_2);
  return s21_add(value_1, value_2, result);
}

void s21_integer_division(s21_decimal dividend, s21_decimal divisor,
                          s21_decimal *quotient, s21_decimal *remainder) {
  s21_init_decimal(quotient);
  s21_copy(dividend, remainder);

  if (s21_is_greater(divisor, dividend)) return;

  s21_decimal current;
  s21_copy(divisor, &current);
  s21_decimal multiple = {{1, 0, 0, 0}};
  int loop = 1;

  while (loop) {
    while (s21_is_less_or_equal(current,
                                *remainder) &&  // current <= remainder
           !(current.bits[2] & S21_SIGN_MASK)) {  // и нет переполнения
      s21_shift_left(&current);   // умножаем current на 2
      s21_shift_left(&multiple);  // умножаем multiple на 2
    }

    s21_shift_right(&current);
    s21_shift_right(&multiple);

    s21_sub(*remainder, current, remainder);
    s21_add(*quotient, multiple, quotient);

    if (s21_is_less(*remainder, divisor)) {  // остаток < делителя
      loop = 0;
    } else {
      s21_copy(divisor, &current);  // сброс рабочих переменных
      s21_init_decimal(&multiple);
      multiple.bits[0] = 1;
    }
  }
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int status = S21_OK;
  s21_init_decimal(result);
  if (s21_is_zero(&value_1) || s21_is_zero(&value_2)) return status;

  int sign1 = s21_get_sign(&value_1), sign2 = s21_get_sign(&value_2);
  int exp1 = s21_get_exp(&value_1), exp2 = s21_get_exp(&value_2);
  int result_sign = sign1 ^ sign2;
  int result_exp = exp1 + exp2;

  s21_decimal temp = {0};
  s21_decimal shifted = value_1;

  for (int i = 0; i < 96 && status == S21_OK; i++) {
    if (s21_get_bit(value_2, i)) {  // сдвиг на й позиций
      s21_decimal current = shifted;
      for (int j = 0; j < i; j++) {
        if (s21_shift_left(&current)) {
          status = S21_ERROR_BIG;
        }
      }
      s21_add(current, temp, &temp);  // добавляем к результату
    }
  }

  while (status == S21_OK && result_exp > S21_MAX_EXP) {  // корректируем экспо
    s21_div_by_10(&temp);
    result_exp--;
  }

  *result = temp;
  s21_set_sign(result, result_sign);
  s21_set_exp(result, result_exp);

  return status;
}

void s21_handle_division_remainder(s21_decimal divisor, s21_decimal *remainder,
                                   s21_decimal *fractional,
                                   int *fractional_digits, int max_scale) {
  while (!s21_is_zero(remainder) && *fractional_digits < max_scale) {
    s21_mul_by_10(remainder);
    s21_decimal digit;
    s21_decimal new_rem;
    s21_integer_division(*remainder, divisor, &digit, &new_rem);
    s21_mul_by_10(fractional);
    s21_add(*fractional, digit, fractional);
    (*fractional_digits)++;
    *remainder = new_rem;
  }
}

int s21_div(s21_decimal dividend, s21_decimal divisor, s21_decimal *result) {
  if (s21_is_zero(&divisor)) return S21_ERROR_ZERO_DIV;

  int status = S21_OK;
  s21_init_decimal(result);

  if (s21_is_zero(&dividend)) return status;

  int sign_dividend = s21_get_sign(&dividend);
  int sign_divisor = s21_get_sign(&divisor);
  int sign_result = sign_dividend ^ sign_divisor;

  s21_set_sign(&dividend, 0);
  s21_set_sign(&divisor, 0);

  int exp_dividend = s21_get_exp(&dividend);
  int exp_divisor = s21_get_exp(&divisor);
  s21_set_exp(&dividend, 0);
  s21_set_exp(&divisor, 0);

  int scale = 0;
  while (scale < 28 && s21_mantissa_less(dividend, divisor)) {  // нормализация
    s21_mul_by_10(&dividend);
    scale++;
  }

  s21_decimal quotient = {0};
  s21_decimal remainder = {0};
  s21_integer_division(dividend, divisor, &quotient,
                       &remainder);  // целочисленное деление

  int fractional_digits = 0;
  s21_decimal fractional = {0};
  s21_handle_division_remainder(divisor, &remainder, &fractional,
                                &fractional_digits,
                                28 - scale);  // обработка дробной части

  s21_decimal total;
  s21_copy(quotient, &total);

  for (int i = 0; i < fractional_digits;
       i++) {  // соединяем целую и дробную часть
    s21_mul_by_10(&total);
  }
  status = s21_add(total, fractional, &total);

  int total_exp = exp_dividend - exp_divisor + scale + fractional_digits;
  while (status == S21_OK && total_exp > 28 && !s21_is_zero(&total)) {
    s21_div_by_10(&total);
    total_exp--;
  }

  if (status == S21_OK) {
    *result = total;
    s21_set_sign(result, sign_result);
    s21_set_exp(result, total_exp > 28 ? 28 : (total_exp < 0 ? 0 : total_exp));
  }

  return status;
}

int s21_is_less(s21_decimal a, s21_decimal b) {
  if (s21_get_sign(&a) && !s21_get_sign(&b)) return 1;  // отриц < полож
  if (!s21_get_sign(&a) && s21_get_sign(&b)) return 0;  // полож > отриц

  s21_normalize_exponents(&a, &b);
  int sign = s21_get_sign(&a);

  for (int i = 2; i >= 0; i--) {  // побайтовое сравнение от старших к младшим
    if (a.bits[i] < b.bits[i]) return sign ? 0 : 1;
    if (a.bits[i] > b.bits[i]) return sign ? 1 : 0;
  }
  return 0;  // равны
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  return s21_is_less(a, b) || s21_is_equal(a, b);
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
  return !s21_is_less_or_equal(a, b);
}

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  return s21_is_greater(a, b) || s21_is_equal(a, b);
}

int s21_is_equal(s21_decimal a, s21_decimal b) {
  if (s21_is_zero(&a) && s21_is_zero(&b)) return 1;
  if (s21_get_sign(&a) != s21_get_sign(&b)) return 0;
  s21_normalize_exponents(&a, &b);

  for (int i = 0; i < 3; i++) {
    if (a.bits[i] != b.bits[i]) return 0;
  }
  return 1;
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  return !s21_is_equal(a, b);
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  s21_init_decimal(dst);

  if (src < 0) {
    s21_set_sign(dst, 1);
    src *= -1;
  }

  dst->bits[0] = src;

  return S21_OK;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  s21_init_decimal(dst);
  float_parser parser;
  parser.f = src;
  double abs_src = fabs(src);

  if (src == 0.0f) return S21_OK;
  if ((abs_src < 1e-28f && src != 0) || (abs_src > S21_MAX_DECIMAL) ||
      (parser.parts.exponent == 255U))
    return S21_ERROR_CONV;

  int exp = 0;
  double temp = abs_src;

  while (exp < 28 && temp < 1e7 &&
         temp * 10.0 <= (double)S21_MAX_DECIMAL) {  // нормализация
    temp *= 10.0;
    exp++;
  }

  uint64_t int_value = (uint64_t)round(temp);

  s21_decimal temp_dec = {0};
  temp_dec.bits[0] = int_value & UINT32_MAX;
  if (int_value > UINT32_MAX) {
    temp_dec.bits[1] = (int_value >> 32) & UINT32_MAX;
  }

  s21_set_exp(&temp_dec, exp);
  s21_set_sign(&temp_dec, parser.parts.sign);
  *dst = temp_dec;

  return S21_OK;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  s21_truncate(src, &src);

  *dst = src.bits[0];
  if (s21_get_sign(&src)) *dst *= -1;

  return S21_OK;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  double result = 0.0;

  for (int i = 0; i < 96; i++) {
    if (s21_get_bit(src, i)) {
      result += pow(2, i);  // суммирование битов
    }
  }

  int exp = s21_get_exp(&src);
  if (exp > 0) {
    result /= pow(10, exp);  // учет экспо
  }

  *dst = (float)result;
  if (s21_get_sign(&src)) {
    *dst *= -1;
  }

  return S21_OK;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  s21_truncate(value, result);

  if (s21_get_sign(&value) && !s21_is_equal(value, *result)) {
    s21_decimal one = {{1, 0, 0, 0}};
    s21_set_sign(&one, 1);
    s21_add(*result, one, result);
  }
  return S21_OK;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  s21_init_decimal(result);
  if (s21_is_zero(&value)) return S21_OK;

  int sign = s21_get_sign(&value);
  s21_set_sign(&value, 0);

  s21_decimal truncated;
  s21_truncate(value, &truncated);  // целая часть

  s21_decimal fractional;
  s21_sub(value, truncated, &fractional);  // дробная часть

  s21_decimal half = {{5, 0, 0, 0}};  // проверка >= 0.5
  s21_set_exp(&half, 1);

  *result = truncated;

  if (s21_is_greater(fractional, half) ||
      (s21_is_equal(fractional, half) && (truncated.bits[0] & 1))) {
    s21_decimal one = {{1, 0, 0, 0}};
    s21_add(*result, one, result);  // округление вверх
  }

  s21_set_sign(result, sign);
  return S21_OK;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int exp = s21_get_exp(&value);
  while (exp-- > 0) {
    s21_div_by_10(&value);  // деление на 10
  }
  *result = value;
  s21_set_exp(result, 0);  // сброс экспо
  return S21_OK;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  *result = value;
  s21_set_sign(result, !s21_get_sign(&value));
  return S21_OK;
}