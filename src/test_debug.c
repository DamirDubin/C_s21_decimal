#include "s21_decimal.h"
#include "stdio.h"
#include <float.h>
#include <limits.h>
#include <string.h>

typedef struct {
    int src_int;             // Исходное целое число (для int → decimal)
    float src_float;         // Исходное число float (для float → decimal)
    s21_decimal src_decimal; // Исходное decimal (для decimal → int/float)
    int expected_int;        // Ожидаемое целое (для decimal → int)
    float expected_float;    // Ожидаемое float (для decimal → float)
    int expected_result;     // Ожидаемый код возврата (0 = OK, 1 = Ошибка)
} conversion_test_case;

typedef struct {
  s21_decimal a;
  s21_decimal b;
  int expected_less;  // Ожидаемый результат для s21_is_less(a, b)
  int expected_less_or_equal;  // Ожидаемый результат для
                               // s21_is_less_or_equal(a, b)
  int expected_greater;  // Ожидаемый результат для s21_is_greater(a, b)
  int expected_greater_or_equal;  // Ожидаемый результат для
                                  // s21_is_greater_or_equal(a, b)
  int expected_equal;  // Ожидаемый результат для s21_is_equal(a, b)
  int expected_not_equal;  // Ожидаемый результат для s21_is_not_equal(a, b)
} comparison_test_case;

typedef struct {
    s21_decimal a;
    s21_decimal b;
    s21_decimal expected_result;
    int expected_error_code;  // 0 (OK), 1 (INF), 2 (-INF), 3 (DIV_ZERO)
} add_test_case;

void run_conversion_tests() {
    conversion_test_case tests[] = {
        // ===================== int → decimal =====================
        // Простые положительные числа
        {
            .src_int = 42,
            .expected_result = 0,
            .src_decimal = {{42, 0, 0, 0}}  // Ожидаемое decimal для 42
        },
        // Отрицательные числа
        {
            .src_int = -100,
            .expected_result = 0,
            .src_decimal = {{100, 0, 0, 0x80000000}}  // -100 (знак в старшем бите)
        },
        // Ноль
        {
            .src_int = 0,
            .expected_result = 0,
            .src_decimal = {{0, 0, 0, 0}}
        },
        // MAX_INT
        {
            .src_int = INT_MAX,
            .expected_result = 0,
            .src_decimal = {{INT_MAX, 0, 0, 0}}
        },
        // MIN_INT
        {
            .src_int = INT_MIN,
            .expected_result = 0,
            .src_decimal = {{(unsigned)INT_MIN, 0, 0, 0x80000000}}
        },

        // ===================== float → decimal =====================
        // Простые положительные числа
        {
            .src_float = 123.456f,
            .expected_result = 0,
            .src_decimal = {{123456, 0, 0, 0x00030000}}  // 123.456 (scale=3)
        },
        // Отрицательные числа
        {
            .src_float = -0.001f,
            .expected_result = 0,
            .src_decimal = {{1, 0, 0, 0x80030000}}  // -0.001 (scale=3, знак)
        },
        // Ноль
        {
            .src_float = 0.0f,
            .expected_result = 0,
            .src_decimal = {{0, 0, 0, 0}}
        },
        // Слишком малое число (должно вернуть ошибку)
        {
            .src_float = 1e-29f,
            .expected_result = 1,
            .src_decimal = {{0, 0, 0, 0}}  // Ожидается 0 (ошибка)
        },
        // Слишком большое число (должно вернуть ошибку)
        {
            .src_float = 8e28f,
            .expected_result = 1,
            .src_decimal = {{0, 0, 0, 0}}  // Ожидается 0 (ошибка)
        },
        // Округление (больше 7 значащих цифр)
        {
            .src_float = 1.23456789f,
            .expected_result = 0,
            .src_decimal = {{1234568, 0, 0, 0x00060000}}  // Ожидается округление до 1.234568
        },

        // ===================== decimal → int =====================
        // Простые числа
        {
            .expected_int = 42,
            .expected_result = 0,
            .src_decimal = {{42, 0, 0, 0}}
        },
        // Отрицательные числа
        {
            .expected_int = -100,
            .expected_result = 0,
            .src_decimal = {{100, 0, 0, 0x80000000}}
        },
        // Дробная часть отбрасывается (0.9 → 0)
        {
            .expected_int = 0,
            .expected_result = 0,
            .src_decimal = {{9, 0, 0, 0x00010000}}  // 0.9 → 0
        },
        // Слишком большое число (должно вернуть ошибку)
        {
            .expected_int = INT_MAX,
            .expected_result = 0,
            .src_decimal = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}}  // MAX_DECIMAL > INT_MAX
        },

        // ===================== decimal → float =====================
        // Простые числа
        {
            .expected_float = 123.456f,
            .expected_result = 0,
            .src_decimal = {{123456, 0, 0, 0x00030000}}  // 123.456
        },
        // Отрицательные числа
        {
            .expected_float = -0.001f,
            .expected_result = 0,
            .src_decimal = {{1, 0, 0, 0x80030000}}  // -0.001
        },
        // Ноль
        {
            .expected_float = 0.0f,
            .expected_result = 0,
            .src_decimal = {{0, 0, 0, 0}}
        }
    };

    const char *test_types[] = {
        "int → decimal",
        "float → decimal",
        "decimal → int",
        "decimal → float"
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        conversion_test_case test = tests[i];
        int result;
        s21_decimal dst_dec;
        int dst_int;
        float dst_float;

        // Проверка int → decimal
        if (i < 5) {
            result = s21_from_int_to_decimal(test.src_int, &dst_dec);
            if (result == test.expected_result &&
                memcmp(&dst_dec, &test.src_decimal, sizeof(s21_decimal)) == 0) {
                printf("Тест %s - %ld пройден\n", test_types[0], i);
            } else {
                printf("Тест %s - %ld провален! (ожидалось %d, получено %d)\n",
                       test_types[0], i, test.expected_result, result);
            }
        }
        // Проверка float → decimal
        else if (i < 11) {
            result = s21_from_float_to_decimal(test.src_float, &dst_dec);
            if (result == test.expected_result &&
                memcmp(&dst_dec, &test.src_decimal, sizeof(s21_decimal)) == 0) {
                printf("Тест %s - %ld пройден\n", test_types[1], i);
            } else {
                printf("Тест %s - %ld провален! (ожидалось %d, получено %d)\n",
                       test_types[1], i, test.expected_result, result);
            }
        }
        // Проверка decimal → int
        else if (i < 15) {
            result = s21_from_decimal_to_int(test.src_decimal, &dst_int);
            if (result == test.expected_result && dst_int == test.expected_int) {
                printf("Тест %s - %ld пройден\n", test_types[2], i);
            } else {
                printf("Тест %s - %ld провален! (ожидалось %d, получено %d)\n",
                       test_types[2], i, test.expected_result, result);
            }
        }
        // Проверка decimal → float
        else {
            result = s21_from_decimal_to_float(test.src_decimal, &dst_float);
            if (result == test.expected_result &&
                fabs(dst_float - test.expected_float) < 1e-6) {
                printf("Тест %s - %ld пройден\n", test_types[3], i);
            } else {
                printf("Тест %s - %ld провален! (ожидалось %d, получено %d)\n",
                       test_types[3], i, test.expected_result, result);
            }
        }
    }
}

void run_add_tests() {
    add_test_case tests[] = {
        // Базовые тесты (положительные числа)
        {
            {{5, 0, 0, 0}}, {{10, 0, 0, 0}}, {{15, 0, 0, 0}}, 0  // 5 + 10 = 15 (OK)
        },
        {
            {{10, 0, 0, 0}}, {{5, 0, 0, 0}}, {{15, 0, 0, 0}}, 0   // 10 + 5 = 15 (OK)
        },
        {
            {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}}, {{1, 0, 0, 0}}, {{0, 0, 1, 0}}, 0  // MAX_UINT + 1 = 2^32 (OK)
        },

        // Отрицательные числа
        {
            {{5, 0, 0, 0x80000000}}, {{3, 0, 0, 0x80000000}}, {{8, 0, 0, 0x80000000}}, 0  // -5 + (-3) = -8 (OK)
        },
        {
            {{5, 0, 0, 0}}, {{3, 0, 0, 0x80000000}}, {{2, 0, 0, 0}}, 0  // 5 + (-3) = 2 (OK)
        },

        // Числа с разным масштабом (scale)
        {
            {{5, 0, 0, 0x00010000}}, {{5, 0, 0, 0x00020000}}, {{55, 0, 0, 0x00020000}}, 0  // 0.5 + 0.05 = 0.55 (OK)
        },

        // Переполнение (слишком большое число)
        {
            {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}}, {{1, 0, 0, 0}}, {{0, 0, 0, 0}}, 1  // MAX_DECIMAL + 1 → INF (ERROR 1)
        },

        // Потеря точности (банковское округление)
        {
            {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x001C0000}}, {{1, 0, 0, 0x001C0000}}, {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x001C0000}}, 0  // MAX_DECIMAL - 0.000...1 ≈ MAX_DECIMAL (округление)
        },

        // Деление на 0 (не относится к сложению, но для примера)
        {
            {{5, 0, 0, 0}}, {{0, 0, 0, 0}}, {{0, 0, 0, 0}}, 0  // 5 + 0 = 5 (OK, но если бы было деление, то ERROR 3)
        }
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        s21_decimal result;
        int error_code = s21_add(tests[i].a, tests[i].b, &result);

        if (error_code == tests[i].expected_error_code) {
            if (error_code == 0) {
                // Проверяем, что результат корректен
                if (s21_is_equal(result, tests[i].expected_result)) {
                    printf("Тест s21_add - %ld пройден\n", i);
                } else {
                    printf("Тест s21_add - %ld провален! (неверный результат)\n", i);
                }
            } else {
                printf("Тест s21_add - %ld пройден (ожидалась ошибка %d)\n", i, error_code);
            }
        } else {
            printf("Тест s21_add - %ld провален! (ожидалась ошибка %d, получено %d)\n", i, tests[i].expected_error_code, error_code);
        }
    }
}

// Тесты для всех функций сравнения
void run_comparison_tests() {
  comparison_test_case tests[] = {// Основные тесты (положительные числа)
                                  // a=5, b=10
                                  {
                                      {{5, 0, 0, 0}},
                                      {{10, 0, 0, 0}},  // 5 и 10
                                      1,                // 5 < 10 → TRUE (1)
                                      1,                // 5 <= 10 → TRUE (1)
                                      0,                // 5 > 10 → FALSE (0)
                                      0,                // 5 >= 10 → FALSE (0)
                                      0,                // 5 == 10 → FALSE (0)
                                      1                 // 5 != 10 → TRUE (1)
                                  },
                                  // a=10, b=5
                                  {
                                      {{10, 0, 0, 0}},
                                      {{5, 0, 0, 0}},  // 10 и 5
                                      0,               // 10 < 5 → FALSE (0)
                                      0,               // 10 <= 5 → FALSE (0)
                                      1,               // 10 > 5 → TRUE (1)
                                      1,               // 10 >= 5 → TRUE (1)
                                      0,               // 10 == 5 → FALSE (0)
                                      1                // 10 != 5 → TRUE (1)
                                  },
                                  // a=5, b=5
                                  {
                                      {{5, 0, 0, 0}},
                                      {{5, 0, 0, 0}},  // 5 и 5
                                      0,               // 5 < 5 → FALSE (0)
                                      1,               // 5 <= 5 → TRUE (1)
                                      0,               // 5 > 5 → FALSE (0)
                                      1,               // 5 >= 5 → TRUE (1)
                                      1,               // 5 == 5 → TRUE (1)
                                      0                // 5 != 5 → FALSE (0)
                                  },

                                  // Отрицательные числа
                                  // a=-5 (0x80000000 - знак минус), b=-3
                                  {
                                      {{5, 0, 0, 0x80000000}},
                                      {{3, 0, 0, 0x80000000}},  // -5 и -3
                                      1,  // -5 < -3 → TRUE (1)
                                      1,  // -5 <= -3 → TRUE (1)
                                      0,  // -5 > -3 → FALSE (0)
                                      0,  // -5 >= -3 → FALSE (0)
                                      0,  // -5 == -3 → FALSE (0)
                                      1   // -5 != -3 → TRUE (1)
                                  },

                                  // Разные масштабы (scale)
                                  // a=0.5 (scale=1), b=0.50 (scale=2)
                                  {
                                      {{5, 0, 0, 0x00010000}},
                                      {{50, 0, 0, 0x00020000}},  // 0.5 и 0.50
                                      0,  // 0.5 < 0.50 → FALSE (0)
                                      1,  // 0.5 <= 0.50 → TRUE (1)
                                      0,  // 0.5 > 0.50 → FALSE (0)
                                      1,  // 0.5 >= 0.50 → TRUE (1)
                                      1,  // 0.5 == 0.50 → TRUE (1)
                                      0   // 0.5 != 0.50 → FALSE (0)
                                  }};

  const char *func_names[] = {"s21_is_less",    "s21_is_less_or_equal",
                              "s21_is_greater", "s21_is_greater_or_equal",
                              "s21_is_equal",   "s21_is_not_equal"};

  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    s21_decimal a = tests[i].a;
    s21_decimal b = tests[i].b;

    int results[] = {s21_is_less(a, b),    s21_is_less_or_equal(a, b),
                     s21_is_greater(a, b), s21_is_greater_or_equal(a, b),
                     s21_is_equal(a, b),   s21_is_not_equal(a, b)};

    int expected_results[] = {
        tests[i].expected_less,    tests[i].expected_less_or_equal,
        tests[i].expected_greater, tests[i].expected_greater_or_equal,
        tests[i].expected_equal,   tests[i].expected_not_equal};

    for (int j = 0; j < 6; j++) {
      if (results[j] == expected_results[j]) {
        printf("Тест %s - %ld пройден\n", func_names[j], i);
      } else {
        printf("Тест %s - %ld провален! (ожидалось %d, получено %d)\n",
               func_names[j], i, expected_results[j], results[j]);
      }
    }
  }
}

void rounding_tests() {
  s21_decimal tests[][2] = {
      {{{5, 0, 0, 0x00010000}}, {{0, 0, 0, 0}}},
      {{{5, 0, 0, 0x80010000}}, {{1, 0, 0, 0x80000000}}},
      {{{15, 0, 0, 0x00010000}}, {{1, 0, 0, 0}}},
      {{{15, 0, 0, 0x80010000}}, {{2, 0, 0, 0x80000000}}},
  };

  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    s21_decimal result;
    s21_floor(tests[i][0], &result);
    // ck_assert(s21_is_equal(result, tests[i][1]));
    if (s21_is_equal(result, tests[i][1])) {
      printf("Тест s21_floor - %ld пройден\n", i);
    } else {
      printf("Тест s21_floor - %ld провален!!!\n", i);
    }
  }

  s21_decimal round_tests[][2] = {
      // Базовые случаи (положительные числа)
      {{{15, 0, 0, 0x00010000}}, {{2, 0, 0, 0x00000000}}},   // 1.5 → 2
      {{{25, 0, 0, 0x00010000}}, {{3, 0, 0, 0x00000000}}},   // 2.5 → 3
      {{{14, 0, 0, 0x00010000}}, {{1, 0, 0, 0x00000000}}},   // 1.4 → 1
      {{{149, 0, 0, 0x00020000}}, {{1, 0, 0, 0x00000000}}},  // 1.49 → 1
      {{{150, 0, 0, 0x00020000}}, {{2, 0, 0, 0x00000000}}},  // 1.50 → 2

      // Отрицательные числа
      {{{15, 0, 0, 0x80010000}}, {{2, 0, 0, 0x80000000}}},   // -1.5 → -2
      {{{25, 0, 0, 0x80010000}}, {{3, 0, 0, 0x80000000}}},   // -2.5 → -3
      {{{14, 0, 0, 0x80010000}}, {{1, 0, 0, 0x80000000}}},   // -1.4 → -1
      {{{149, 0, 0, 0x80020000}}, {{1, 0, 0, 0x80000000}}},  // -1.49 → -1
      {{{150, 0, 0, 0x80020000}}, {{2, 0, 0, 0x80000000}}},  // -1.50 → -2

      // Граничные случаи (максимальные значения)
      {{{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000}},
       {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
         0x00000000}}},  // 2^96-1 (не округляется)
      {{{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00010000}},
       {{0x9999999A, 0x99999999, 0x19999999,
         0x00000000}}},  // (2^96-1)/10 → округляется вверх (если не
                         // переполнение)

      // Нули и -0.0
      {{{0, 0, 0, 0x00000000}}, {{0, 0, 0, 0x00000000}}},  // 0.0 → 0
      {{{0, 0, 0, 0x80000000}}, {{0, 0, 0, 0x00000000}}},  // -0.0 → 0

      // Дробная часть очень близка к 0.5
      {{{499999999, 0, 0, 0x00090000}},
       {{0, 0, 0, 0x00000000}}},  // 0.499999999 → 0
      {{{500000000, 0, 0, 0x00090000}},
       {{1, 0, 0, 0x00000000}}},  // 0.500000000 → 1
      {{{499999999, 0, 0, 0x80090000}},
       {{0, 0, 0, 0x00000000}}},  // -0.499999999 → 0
      {{{500000000, 0, 0, 0x80090000}},
       {{1, 0, 0, 0x80000000}}},  // -0.500000000 → -1
  };

  for (size_t i = 0; i < sizeof(round_tests) / sizeof(round_tests[0]); i++) {
    s21_decimal result;
    s21_round(round_tests[i][0], &result);
    // ck_assert(s21_is_equal(result, round_tests[i][1]));
    if (s21_is_equal(result, round_tests[i][1])) {
      printf("Тест round_tests - %ld пройден\n", i);
    } else {
      printf("Тест round_tests - %ld провален!!!\n", i);
    }
  }

  s21_decimal trunc_tests[][2] = {
      {{{123456, 0, 0, 0x00030000}}, {{123, 0, 0, 0}}},
      {{{123456, 0, 0, 0x80030000}}, {{123, 0, 0, 0x80000000}}},
  };

  for (size_t i = 0; i < sizeof(trunc_tests) / sizeof(trunc_tests[0]); i++) {
    s21_decimal result;
    s21_truncate(trunc_tests[i][0], &result);
    // ck_assert(s21_is_equal(result, trunc_tests[i][1]));
    if (s21_is_equal(result, trunc_tests[i][1])) {
      printf("Тест s21_truncate - %ld пройден\n", i);
    } else {
      printf("Тест s21_truncate - %ld провален!!!\n", i);
    }
  }

  s21_decimal neg_tests[][2] = {
      {{{5, 0, 0, 0}}, {{5, 0, 0, 0x80000000}}},
      {{{5, 0, 0, 0x80000000}}, {{5, 0, 0, 0}}},
      {{{0, 0, 0, 0}}, {{0, 0, 0, 0}}},
  };

  for (size_t i = 0; i < sizeof(neg_tests) / sizeof(neg_tests[0]); i++) {
    s21_decimal result;
    s21_negate(neg_tests[i][0], &result);
    if (s21_is_equal(result, neg_tests[i][1])) {
      printf("Тест s21_negate - %ld пройден\n", i);
    } else {
      printf("Тест s21_negate - %ld провален!!!\n", i);
    }
  }
}

void print_decimal_bits(s21_decimal dec) {
    for (int i = 0; i < 4; i++) {
        printf("bits[%d]: ", i);
        for (int j = 31; j >= 0; j--) {
            printf("%d", (dec.bits[i] >> j) & 1);
        }
        printf("\n");
    }
}

void custom_test_add() {
    s21_decimal a = {{0}};
    s21_decimal b = {{0}};
    s21_decimal result = {{0}};

    
    a.bits[0] = 25;    //4294967295
    a.bits[1] = 0000000000;   
    a.bits[2] = 0000000000;  
    s21_set_scale(&a, 1);        
    s21_set_sign(&a, 0);       
    
     b.bits[0] = 45;
     b.bits[1] = 0000000000;  
     b.bits[2] = 0000000000;                  
    s21_set_scale(&b, 1);        
    s21_set_sign(&b, 0);     
    

        // s21_big_decimal c = {{0}};
        // s21_from_decimal_to_big(b, &c);
        //  s21_mul_ten(&c);
   

    
    print_decimal_bits(a);
    printf("Scale: %d\n", s21_get_scale(&a));
    printf("Sign : %d\n\n", s21_get_sign(&a));

    print_decimal_bits(b);
    printf("Scale: %d\n", s21_get_scale(&b));
    printf("Sign : %d\n\n", s21_get_sign(&b));

    int status = s21_add(a, b, &result);
    
    if (status == 0) {
        print_decimal_bits(result);
        printf("Scale: %d\n", s21_get_scale(&result));
        printf("Sign : %d\n", s21_get_sign(&result));
        
    } else {
        printf("ERROR\n\n");
    }

}

void print_float_bits(float f) {
    uint32_t bits = *(uint32_t*)&f;
    printf("Знак: %d\n", (bits >> 31) & 1);
    printf("Экспонента: %d\n", ((bits >> 23) & 0xFF) - 127);
    printf("Мантисса: 0x%X\n", bits & 0x7FFFFF);
}

void test_mul(){
    s21_decimal a = {{45, 0, 0, 0}};
    s21_decimal b = {{10, 0, 0, 0}};
    s21_decimal result = {(0)};
    s21_mul(a, b, &result); 
}
int main() {
  
  rounding_tests();
  run_comparison_tests();
  run_add_tests();
  run_conversion_tests();
  // custom_test_add();


    // float num = -12.375f;
    // print_float_bits(num);
    // test_mul();

  return 0;
}
