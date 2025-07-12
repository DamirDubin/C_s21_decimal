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

    return failed;
}

int main() {
    int failed = test_addition();
    printf("\n=== TESTS FAILED: %d ===\n", failed);
    return failed ? 1 : 0;
}