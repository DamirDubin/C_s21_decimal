#include <stdio.h>
#include "s21_decimal.h"

void print_decimal(s21_decimal d) {
    printf("Scale: %d, Sign: %d, Bits: [%u, %u, %u]\n", 
           s21_get_scale(&d), s21_get_sign(&d), 
           d.bits[0], d.bits[1], d.bits[2]);
}

int test_addition() {
    int failed = 0;
    s21_decimal a, b, result;

    printf("\n=== TEST 1: Basic Addition ===\n");
    a = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 15) {
        printf("FAIL! Expected 15, got %u\n", result.bits[0]);
        failed++;
    }

    printf("\n=== TEST 2: Negative Addition ===\n");
    a = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&a, 0); s21_set_sign(&a, 1);
    b = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&b, 0); s21_set_sign(&b, 1);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 15 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -15, got sign %d\n", s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 3: Mixed Signs ===\n");
    a = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&b, 0); s21_set_sign(&b, 1);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 5 || s21_get_sign(&result)) {
        printf("FAIL! Expected 5, got %u with sign %d\n", result.bits[0], s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 4: Different Scales ===\n");
    a = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&a, 0); s21_set_sign(&a, 1);
    b = (s21_decimal){{35, 0, 0, 0}}; s21_set_scale(&b, 1);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 15 || s21_get_scale(&result) != 1 || s21_get_sign(&result)) {
        printf("FAIL! Expected 1.5, got %u with scale %d sign %d\n", 
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 5: Large Numbers ===\n");
    a = (s21_decimal){{2000000000, 2000000000, 2000000000, 0}}; s21_set_scale(&a, 1);
    b = (s21_decimal){{3000000000, 3000000000, 3000000000, 0}}; s21_set_scale(&b, 1);
    int status = s21_add(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 500000000 || s21_get_scale(&result) != 2) {
        printf("FAIL! Status: %d, Result: %u, Scale: %d\n", 
               status, result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 6: Overflow ===\n");
    s21_decimal max = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    s21_decimal one = {{1, 0, 0, 0}};
    status = s21_add(max, one, &result);
    if (status == 0) {
        printf("FAIL! Expected overflow, but got success\n");
        failed++;
    } else {
        printf("Overflow handled correctly (status %d)\n", status);
    }

    printf("\n=== TEST 7: Addition with Zero ===\n");
    a = (s21_decimal){{123456789, 0, 0, 0}}; s21_set_scale(&a, 2);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != a.bits[0] || s21_get_scale(&result) != 2 || s21_get_sign(&result)) {
        printf("FAIL! Expected 123456789 scale 2, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 8: Mixed Signs Subtraction ===\n");
    a = (s21_decimal){{100, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{40, 0, 0, 0}}; s21_set_scale(&b, 0); s21_set_sign(&b, 1);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 60 || s21_get_sign(&result)) {
        printf("FAIL! Expected 60, got %u with sign %d\n", result.bits[0], s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 9: Different Scales and Rounding ===\n");
    a = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&a, 1); // 0.5
    b = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&b, 1); // 0.5
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 10 || s21_get_scale(&result) != 1 || s21_get_sign(&result)) {
        printf("FAIL! Expected 1.0, got %u with scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 10: Max Decimal minus 1 ===\n");
    max = (s21_decimal){{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}}; // max decimal
    one = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&one, 0); s21_set_sign(&one, 1);
    status = s21_add(max, one, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 0xFFFFFFFE || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected max -1, got error or wrong value\n");
        failed++;
    }

    printf("\n=== TEST 11: Rounding .5 to even ===\n");
    a = (s21_decimal){{15, 0, 0, 0}}; s21_set_scale(&a, 1); // 1.5
    b = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&b, 1);   // 0.5
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 20 || s21_get_scale(&result) != 1 || s21_get_sign(&result)) {
        printf("FAIL! Expected 2.0, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 12: Rounding odd .5 up ===\n");
    a = (s21_decimal){{21, 0, 0, 0}}; s21_set_scale(&a, 1); // 2.1
    b = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&b, 1); // 0.5
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 26 || s21_get_scale(&result) != 1) {
        printf("FAIL! Expected 2.6, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 13: Large Scale (28) ===\n");
    a = (s21_decimal){{123456789, 0, 0, 0}}; s21_set_scale(&a, 28);
    b = (s21_decimal){{987654321, 0, 0, 0}}; s21_set_scale(&b, 28);
    s21_add(a, b, &result);
    print_decimal(result);
    unsigned long long expected = 123456789 + 987654321;
    if (result.bits[0] != expected || s21_get_scale(&result) != 28) {
        printf("FAIL! Expected %llu scale 28, got %u scale %d\n", expected, result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 14: Overflow after scaling up ===\n");
    a = (s21_decimal){{0xFFFFFFFF, 0xFFFFFFFF, 0x00000001, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}}; s21_set_scale(&b, 0);
    status = s21_add(a, b, &result);
    if (status != 1) {
        printf("FAIL! Expected overflow, got %d\n", status);
        failed++;
    } else {
        printf("Overflow handled correctly (status 1)\n");
    }

    return failed;
}

int main() {
    int failed = test_addition();
    printf("\n=== TESTS FAILED: %d ===\n", failed);
    return failed ? 1 : 0;
}