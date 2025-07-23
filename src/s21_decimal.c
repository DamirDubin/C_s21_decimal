#include "s21_decimal.h"

void s21_init_decimal(s21_decimal* dec) {
  for (int i = 0; i < 4; i++) dec->bits[i] = 0;
}

int s21_div_by_10(s21_decimal* value) {
  unsigned long long remainder = 0;
  unsigned long long digit = 0;

  for (int i = 2; i >= 0; i--) {
    digit = (((unsigned long long)value->bits[i]) & 0xFFFFFFFFULL) |
            (remainder << 32);  // формируем 64-бит значение
    value->bits[i] = (unsigned int)(digit / 10) & 0xFFFFFFFF;  // целая часть
    remainder = digit % 10;                                    // остаток
  }

  return (int)remainder;  // возвращаем остаток от деления
}

int s21_mul_by_10(s21_decimal* value) {
  unsigned long long carry = 0;
  for (int i = 0; i < 3; i++) {  // для каждого слова мантиссы
    unsigned long long digit = (unsigned int)value->bits[i];
    digit = digit * 10 + carry;
    value->bits[i] = (unsigned int)(digit & 0xFFFFFFFF);  // сохраняем 32 бита
    carry = digit >> 32;
  }
  return carry ? 1 : 0;  // 1 если переполнение
}

void s21_normalize_exponents(s21_decimal* a, s21_decimal* b) {
  int exp_a = s21_get_scale(a);
  int exp_b = s21_get_scale(b);

  while (exp_a != exp_b) {
    if (exp_a < exp_b) {            // для а
      if (s21_mul_by_10(a) == 0) {  // увеличиваем а
        exp_a++;
        s21_set_scale(a, exp_a);
      } else {  // если переполнение уменьшаем б
        s21_div_by_10(b);
        exp_b--;
        s21_set_scale(b, exp_b);
      }
    } else {  // для б
      if (s21_mul_by_10(b) == 0) {
        exp_b++;
        s21_set_scale(b, exp_b);
      } else {
        s21_div_by_10(a);
        exp_a--;
        s21_set_scale(a, exp_a);
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------------

void s21_shift_left(s21_big_decimal* num, int shift_value) {
  if (shift_value >= 0 && shift_value < 32) {
    unsigned memory = 0;
    for (int i = 0; i < 7; ++i) {
      unsigned temp = num->bits[i];
      num->bits[i] <<= shift_value;
      num->bits[i] |= memory;
      memory = temp >> (32 - shift_value);
    }
  }
}

void s21_bitwise_addition(s21_big_decimal* value_1, s21_big_decimal* value_2,
                          s21_big_decimal* result) {
  unsigned int memo = 0;
  for (int i = 0; i < 32 * 7; i++) {
    unsigned int result_bit =
        s21_big_get_bit(value_1, i) + s21_big_get_bit(value_2, i) + memo;
    memo = result_bit / 2;
    result_bit %= 2;

    s21_big_set_bit(result, i, result_bit);
  }
}

void s21_big_set_bit(s21_big_decimal* value, int bit_number, int bit_value) {
  int int_index = bit_number / 32;
  int bit_index = bit_number % 32;

  if (int_index >= 0 && int_index < 7) {
    if (bit_value == 1) {
      value->bits[int_index] |= (1 << bit_index);
    } else {
      value->bits[int_index] &= ~(1 << bit_index);
    }
  }
}

int s21_big_get_bit(s21_big_decimal* value, int bit_number) {
  return (value->bits[bit_number / 32] >> (bit_number % 32)) & 1;
}

void s21_set_sign(s21_decimal* az, int sign_value) {
  if (sign_value == 0 || sign_value == 1) {
    if (sign_value) {
      az->bits[3] |= 1 << 31;
    } else {
      az->bits[3] &= ~(1 << 31);
    }
  }
}

int s21_get_sign(s21_decimal* az) { return (az->bits[3] >> 31) & 1; }

void s21_set_scale(s21_decimal* az, int scale_value) {
  if (scale_value >= 0 && scale_value <= 28) {
    int a = scale_value << 16;
    int b = az->bits[3] & (1 << 31);
    az->bits[3] = a | b;
  }
}

int s21_get_scale(s21_decimal* az) {
  return (az->bits[3] >> 16) & 0b00000000000000000000000011111111;
}

void s21_from_decimal_to_big(s21_decimal az, s21_big_decimal* big) {
  for (int i = 0; i < 3; i++) {
    big->bits[i] = az.bits[i];
  }
  for (int i = 3; i < 7; i++) {
    big->bits[i] = 0;
  }
}

int s21_from_big_to_decimal(s21_big_decimal big, s21_decimal* dec) {
  if (big.bits[3] || big.bits[4] || big.bits[5] || big.bits[6]) {
    return 1;
  }

  for (int i = 0; i < 3; i++) {
    dec->bits[i] = big.bits[i];
  }
  return 0;
}

void s21_mul_ten(s21_big_decimal* t) {
  s21_big_decimal temp = *t;
  s21_shift_left(t, 3);
  s21_shift_left(&temp, 1);
  s21_bitwise_addition(t, &temp, t);
}

int s21_normalize_big_decimals(s21_big_decimal* a, s21_big_decimal* b,
                               int* scale_a, int* scale_b) {
  while (*scale_a < *scale_b) {
    s21_big_decimal a_copy = *a;
    s21_mul_ten(&a_copy);

    if (s21_is_out_of_96_bits(a_copy)) {
      if (*scale_b > 0) {
        s21_big_decimal b_copy = *b;
        unsigned long long remainder = 0;

        for (int i = 6; i >= 0; i--) {
          unsigned long long value =
              ((unsigned long long)remainder << 32) | b_copy.bits[i];
          b_copy.bits[i] = (unsigned int)(value / 10);
          remainder = value % 10;
        }
        // if (remainder >= 5) {
        //     s21_big_decimal one = {{1, 0, 0, 0, 0, 0, 0, 0}};
        //     unsigned long long carry = 0;

        //     for (int i = 0; i < 7; i++) {
        //         unsigned long long sum = (unsigned long long)b_copy.bits[i] +
        //         carry; if (i == 0) sum += 1; b_copy.bits[i] = sum &
        //         0xFFFFFFFFU; carry = sum >> 32;
        //     }

        //     if (carry) {
        //         return 1;
        //     }
        // }

        if (s21_is_out_of_96_bits(b_copy)) {
          return 1;
        }

        *b = b_copy;
        (*scale_b)--;
      } else {
        return 1;
      }
    } else {
      *a = a_copy;
      (*scale_a)++;
    }
  }

  while (*scale_b < *scale_a) {
    s21_big_decimal b_copy = *b;
    s21_mul_ten(&b_copy);

    if (s21_is_out_of_96_bits(b_copy)) {
      return 1;
    }
    *b = b_copy;
    (*scale_b)++;
  }
  return 0;
}

int s21_is_out_of_96_bits(s21_big_decimal big) {
  return big.bits[3] || big.bits[4] || big.bits[5] || big.bits[6];
}

void s21_big_subtraction(s21_big_decimal* a, s21_big_decimal* b,
                         s21_big_decimal* res) {
  s21_big_decimal neg_b;
  s21_big_negate(b, &neg_b);

  s21_bitwise_addition(a, &neg_b, res);
}

void s21_big_negate(s21_big_decimal* num, s21_big_decimal* result) {
  for (int i = 0; i < 7; i++) {
    result->bits[i] = ~num->bits[i];
  }

  s21_big_decimal one = {{1, 0, 0, 0, 0, 0, 0}};
  s21_bitwise_addition(result, &one, result);
}

int s21_big_is_greater_or_equal(s21_big_decimal a, s21_big_decimal b) {
  for (int i = 6; i >= 0; i--) {
    if (a.bits[i] > b.bits[i]) return 1;
    if (a.bits[i] < b.bits[i]) return 0;
  }
  return 1;
}

int last_digit(s21_big_decimal* big, int z) {
  int x = 1;
  int a = 0;

  for (int i = 0; i < 32 * 7; i++) {
    if ((big->bits[i / 32] >> i % 32) & 1) {
      a = (a + x) % z;
    }
    x = (x * 2) % z;
  }
  return a;
}

int s21_reduce_and_round(s21_big_decimal* big, int* scale) {
  int error = 0;

  while (s21_is_out_of_96_bits(*big) && *scale < 28) {
    unsigned long long remainder = 0;
    s21_big_decimal old_big = *big;

    for (int i = 6; i >= 0; i--) {
      unsigned long long value =
          ((unsigned long long)remainder << 32) | big->bits[i];
      big->bits[i] = value / 10;
      remainder = value % 10;
    }

    (*scale)++;

    int last_digit_before_division = last_digit(&old_big, 10);

    if (remainder > 5 ||
        (remainder == 5 && last_digit_before_division % 2 != 0)) {
      // s21_big_decimal one = {{1, 0, 0, 0, 0, 0, 0}};
      unsigned carry = 0;

      for (int i = 0; i < 7; i++) {
        unsigned long long sum = (unsigned long long)big->bits[i] + carry;
        if (i == 0) sum += 1;
        big->bits[i] = sum & 0xFFFFFFFF;
        carry = sum >> 32;
      }

      if (carry) {
        error = 1;
        break;
      }
    }
  }

  if (s21_is_out_of_96_bits(*big)) {
    error = 1;
  }
  return error;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  // if (!result) return 1;

  //*result = (s21_decimal){{0}};

  if (result == S21_NULL) {
    return S21_ERROR;
  }

  s21_init_decimal(result);

  s21_big_decimal a, b, res = {0};
  s21_from_decimal_to_big(value_1, &a);
  s21_from_decimal_to_big(value_2, &b);

  int scale_a = s21_get_scale(&value_1);
  int scale_b = s21_get_scale(&value_2);
  int sign_a = s21_get_sign(&value_1);
  int sign_b = s21_get_sign(&value_2);
  int result_sign = sign_a;

  if (s21_normalize_big_decimals(&a, &b, &scale_a, &scale_b)) {
    return 1;
  }

  if (sign_a == sign_b) {
    s21_bitwise_addition(&a, &b, &res);
  } else {
    if (s21_big_is_greater_or_equal(a, b)) {
      s21_big_subtraction(&a, &b, &res);
      result_sign = sign_a;
    } else {
      s21_big_subtraction(&b, &a, &res);
      result_sign = sign_b;
    }
  }
  if (scale_a == 0 && s21_is_out_of_96_bits(res)) {
    return 1;  // Возвращаем ошибку для целых чисел
  }

  if (s21_reduce_and_round(&res, &scale_a)) {
    return 1;
  }

  if (s21_from_big_to_decimal(res, result)) {
    return 1;
  }

  s21_set_scale(result, scale_a);
  s21_set_sign(result, result_sign);

  return 0;
}

//--------------------------------------------------------------------------------------------------------------------------

int s21_get_bit(s21_decimal dec, int bit) {
  int word = bit / 32;  // определяем в каком слове находится бит (0-3)
  int offset = bit % 32;  // позиция бита внутри слова
  return !!(dec.bits[word] & (1U << offset));  // извлекаем и возвращаем бит
}

int s21_from_int_to_decimal(int src, s21_decimal* dst) {
  s21_init_decimal(dst);
  if (src < 0) {
    s21_set_sign(dst, 1);
    src = -src;
  }
  dst->bits[0] = src;  // младшее слово
  return S21_OK;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  s21_truncate(src, &src);
  *dst = src.bits[0];
  if (s21_get_sign(&src)) *dst = -*dst;
  return S21_OK;
}

int s21_from_float_to_decimal(float src, s21_decimal* dst) {
  s21_init_decimal(dst);
  if (src == 0.0f) return S21_OK;
  return S21_OK;
}

int s21_from_decimal_to_float(s21_decimal src, float* dst) {
  double result = 0.0;
  for (int i = 0; i < 96; i++) {
    if (s21_get_bit(src, i)) {
      result += pow(2.0, i);  // суммирование битов
    }
  }

  int exp = s21_get_scale(&src);  //
  if (exp > 0) {
    result /= pow(10.0, exp);  // учет экспо
  }

  *dst = (float)result;
  if (s21_get_sign(&src)) {
    *dst = -*dst;
  }

  return S21_OK;
}

//--------------------------------------------------------------------------------------------------------------------------

int s21_is_less(s21_decimal a, s21_decimal b) {
  int result = 0;

  s21_normalize_exponents(&a, &b);  // выравниваем экспоненты

  int sign_a = s21_get_sign(&a);  // получаем знак а
  int sign_b = s21_get_sign(&b);  // получаем знак b
  int abs_cmp = s21_abs_compare(a, b);

  // Если знаки разные, отрицательное всегда меньше
  if (sign_a != sign_b) {
    result = sign_a;  // если а отриц, то а < b и мы возвращаем знак а(1, а это
                      // true),если а полож, то a > b, это false и мы возвращаем
                      // знак 0 (то, что и нужно)
  }
  // 2. Если оба отрицательные
  else if (sign_a == 1) {
    // Для отрицательных: больше модуль → меньше число
    result = (abs_cmp == 1);
  }
  // 3. Если оба положительные
  else {
    // Для положительных: меньше модуль → меньше число
    result = (abs_cmp == -1);
  }

  return result;
}

int s21_abs_compare(s21_decimal a, s21_decimal b) {
  int result = 0;  // По умолчанию считаем числа равными

  // Сравниваем биты от старших к младшим
  for (int i = 2; i >= 0 && result == 0; i--) {
    if (a.bits[i] > b.bits[i]) {
      result = 1;
    } else if (a.bits[i] < b.bits[i]) {
      result = -1;
    }
  }

  return result;
}

int s21_is_equal(s21_decimal a, s21_decimal b) {
  int result = 1;

  s21_normalize_exponents(&a, &b);

  int sign_a = s21_get_sign(&a);  // получаем знак а
  int sign_b = s21_get_sign(&b);  // получаем знак b

  if (sign_a != sign_b) {
    result = (s21_is_zero(a) && s21_is_zero(b));
  } else {
    for (int i = 0; i < 3; i++) {
      if (a.bits[i] != b.bits[i]) {
        result = 0;
      }
    }
  }
  return result;
}

int s21_is_zero(s21_decimal dec) {
  return dec.bits[0] == 0 && dec.bits[1] == 0 && dec.bits[2] == 0;
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  return s21_is_less(a, b) || s21_is_equal(a, b);
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
  return !s21_is_less_or_equal(a, b);
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  return !s21_is_equal(a, b);
}

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  return !s21_is_less(a, b);
}

//--------------------------------------------------------------------------------------------------------------------------

// Округляет указанное Decimal число до ближайшего целого числа в сторону
// отрицательной бесконечности.
int s21_floor(s21_decimal value, s21_decimal* result) {
  int status_result = S21_OK;

  if (result == S21_NULL) {
    return S21_ERROR_CALC;
  }

  s21_init_decimal(result);

  if (s21_truncate(value, result) == S21_OK) {
    if (s21_get_sign(&value) && !s21_is_equal(value, *result)) {
      s21_decimal one = {{1, 0, 0, 0}};
      s21_set_sign(&one, 1);
      if (s21_add(*result, one, result) != S21_OK)
        status_result = S21_ERROR_CALC;
    }

  } else {
    status_result = S21_ERROR_CALC;
  }

  return status_result;
}

// Округляет Decimal до ближайшего целого числа.
int s21_round(s21_decimal value, s21_decimal* result) {
  if (result == S21_NULL) {
    return S21_ERROR_CALC;
  }

  s21_init_decimal(result);

  if (s21_is_zero(value)) return S21_OK;

  int sign = s21_get_sign(&value);
  s21_set_sign(&value, 0);

  s21_decimal truncated;
  s21_truncate(value, &truncated);  // целая часть

  s21_decimal negate_truncated;
  s21_negate(truncated, &negate_truncated);  // negate_truncated = -truncated

  s21_decimal fractional;
  s21_add(value, negate_truncated, &fractional);  // value - truncated

  s21_decimal half = {{5, 0, 0, 0}};  // проверка >= 0.5
  s21_set_scale(&half, 1);

  *result = truncated;

  s21_decimal max_dec = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000}  // 2^96 - 1, scale = 0
  };
  if ((s21_is_greater_or_equal(fractional, half)) &&
      (!s21_is_equal(truncated, max_dec))) {
    s21_decimal one = {{1, 0, 0, 0}};
    s21_add(*result, one, result);  // Округляем вверх
  }

  s21_set_sign(result, sign);
  return S21_OK;
}

// Возвращает целые цифры указанного Decimal числа; любые дробные цифры
// отбрасываются, включая конечные нули.
int s21_truncate(s21_decimal value, s21_decimal* result) {
  if (result == S21_NULL) {
    return S21_ERROR_CALC;
  }

  int status_result = S21_OK;

  // Валидация экспоненты
  int exp = s21_get_scale(&value);
  if (exp > 28 || exp < 0) {
    status_result = S21_ERROR_CALC;
  }

  if (status_result == S21_OK) {
    while (exp-- > 0) {
      s21_div_by_10(&value);  // деление на 10^эксп
    }

    *result = value;
    s21_set_scale(result, 0);  // сброс экспо
  }

  return status_result;
}

// Возвращает результат умножения указанного Decimal на -1.
int s21_negate(s21_decimal value, s21_decimal* result) {
  // Проверка указателя
  if (result == S21_NULL) {
    return S21_ERROR_CALC;
  }

  *result = value;

  s21_set_sign(result, !s21_get_sign(&value));
  return S21_OK;
}