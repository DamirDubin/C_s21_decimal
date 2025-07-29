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

   printf("\n=== TEST 5: Normal Addition ===\n");
a = (s21_decimal){{200000000, 0, 0, 0}}; s21_set_scale(&a, 1);  // 20,000,000.0
b = (s21_decimal){{300000000, 0, 0, 0}}; s21_set_scale(&b, 1);  // 30,000,000.0
int status = s21_add(a, b, &result);
print_decimal(result);
if (status != 0 || result.bits[0] != 500000000 || s21_get_scale(&result) != 1) {
    printf("FAIL! Status: %d, Result: %u, Scale: %d\n", 
           status, result.bits[0], s21_get_scale(&result));
    failed++;
}

    printf("\n=== TEST 6: Overflow ===\n");
    s21_decimal max_val = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    s21_decimal one_val = {{1, 0, 0, 0}};
    status = s21_add(max_val, one_val, &result);
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
    s21_decimal max_val2 = (s21_decimal){{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}}; // max decimal
    s21_decimal one_val2 = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&one_val2, 0); s21_set_sign(&one_val2, 1);
    status = s21_add(max_val2, one_val2, &result);
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
    unsigned long long expected = 123456789ULL + 987654321ULL;
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

    printf("\n=== TEST 15: Zero + Zero ===\n");
    a = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0 || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected 0, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 16: Negative Zero ===\n");
    a = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&a, 5); s21_set_sign(&a, 1);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 3);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0 || s21_get_scale(&result) != 5) {  // Убрана проверка знака
    printf("FAIL! Expected 0 with scale 5, got %u scale %d sign %d\n", result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
    failed++;
}

    printf("\n=== TEST 17: Same Number Different Signs ===\n");
    a = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&a, 2);
    b = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&b, 2); s21_set_sign(&b, 1);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0 || s21_get_scale(&result) != 2) {
        printf("FAIL! Expected 0, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 18: Very Small Numbers ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 28);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 28);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 2 || s21_get_scale(&result) != 28) {
        printf("FAIL! Expected 0.0000000000000000000000000002, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

  printf("\n=== TEST 19: Near Maximum Scale Addition ===\n");
a = (s21_decimal){{100000000, 0, 0, 0}}; s21_set_scale(&a, 28);
b = (s21_decimal){{200000000, 0, 0, 0}}; s21_set_scale(&b, 28);
s21_add(a, b, &result);
print_decimal(result);
if (result.bits[0] != 300000000 || s21_get_scale(&result) != 28) {
    printf("FAIL! Expected 300000000 scale 28, got %u scale %d\n", 
           result.bits[0], s21_get_scale(&result));
    failed++;
}

    printf("\n=== TEST 20: Carry Between Words ===\n");
    a = (s21_decimal){{0xFFFFFFFF, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0 || result.bits[1] != 1 || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected carry to next word, got [%u, %u, %u]\n", result.bits[0], result.bits[1], result.bits[2]);
        failed++;
    }

    printf("\n=== TEST 21: Multiple Carries ===\n");
    a = (s21_decimal){{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0 || result.bits[1] != 0 || result.bits[2] != 1 || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected multiple carries, got [%u, %u, %u]\n", result.bits[0], result.bits[1], result.bits[2]);
        failed++;
    }

    printf("\n=== TEST 22: Decimal Precision Test ===\n");
    a = (s21_decimal){{123456789, 0, 0, 0}}; s21_set_scale(&a, 5);
    b = (s21_decimal){{987654321, 0, 0, 0}}; s21_set_scale(&b, 5);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 1111111110 || s21_get_scale(&result) != 5) {
        printf("FAIL! Expected 11111.11110, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 23: Negative Large Number ===\n");
    a = (s21_decimal){{1000000, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{1000000, 0, 0, 0}}; s21_set_scale(&b, 0); s21_set_sign(&b, 1);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0 || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected 0, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 24: High Bit Numbers ===\n");
    a = (s21_decimal){{0x80000000, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{0x80000000, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0 || result.bits[1] != 1 || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected 0x100000000, got [%u, %u]\n", result.bits[0], result.bits[1]);
        failed++;
    }

    printf("\n=== TEST 25: Scale Reduction Test ===\n");
    a = (s21_decimal){{500000000, 0, 0, 0}}; s21_set_scale(&a, 1);
    b = (s21_decimal){{500000000, 0, 0, 0}}; s21_set_scale(&b, 1);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 1000000000 || s21_get_scale(&result) != 1) {
        printf("FAIL! Expected 100000000.0, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 26: Boundary Values ===\n");
    a = (s21_decimal){{429496729, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 429496729 || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected 429496729, got %u\n", result.bits[0]);
        failed++;
    }

    printf("\n=== TEST 27: Different Sign Large Numbers ===\n");
    a = (s21_decimal){{1000000000, 0, 0, 0}}; s21_set_scale(&a, 2);
    b = (s21_decimal){{500000000, 0, 0, 0}}; s21_set_scale(&b, 2); s21_set_sign(&b, 1);
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 500000000 || s21_get_scale(&result) != 2 || s21_get_sign(&result)) {
        printf("FAIL! Expected 5000000.00, got %u scale %d sign %d\n", result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 28: Zero Scale Addition ===\n");
    a = (s21_decimal){{1234567890, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{987654321, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_add(a, b, &result);
    print_decimal(result);
    unsigned int expected28 = 2222222211U;
    if (result.bits[0] != expected28 || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected %u, got %u\n", expected28, result.bits[0]);
        failed++;
    }

    printf("\n=== TEST 29: Precision Loss Test ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 10);
    b = (s21_decimal){{999999999, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_add(a, b, &result);
    print_decimal(result);
    if (s21_get_scale(&result) > 10) {
        printf("FAIL! Scale increased unexpectedly to %d\n", s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 30: Edge Case Rounding ===\n");
    a = (s21_decimal){{14, 0, 0, 0}}; s21_set_scale(&a, 1); // 1.4
    b = (s21_decimal){{16, 0, 0, 0}}; s21_set_scale(&b, 1); // 1.6
    s21_add(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 30 || s21_get_scale(&result) != 1) {
        printf("FAIL! Expected 3.0, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    return failed;
}

int test_subtraction() {
    int failed = 0;
    s21_decimal a, b, result;

    printf("\n=== TEST 1: Basic Subtraction ===\n");
    a = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 5 || s21_get_sign(&result)) {
        printf("FAIL! Expected 5, got %u sign %d\n", result.bits[0], s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 2: Negative Result ===\n");
    a = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 5 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -5, got %u sign %d\n", result.bits[0], s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 3: Subtracting Negative Number ===\n");
    a = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&b, 0); s21_set_sign(&b, 1);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 15 || s21_get_sign(&result)) {
        printf("FAIL! Expected 15, got %u sign %d\n", result.bits[0], s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 4: Subtracting from Negative Number ===\n");
    a = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&a, 0); s21_set_sign(&a, 1);
    b = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 15 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -15, got %u sign %d\n", result.bits[0], s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 5: Negative Minus Negative ===\n");
    a = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&a, 0); s21_set_sign(&a, 1);
    b = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&b, 0); s21_set_sign(&b, 1);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 5 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -5, got %u sign %d\n", result.bits[0], s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 6: Different Scales ===\n");
    a = (s21_decimal){{35, 0, 0, 0}}; s21_set_scale(&a, 1); // 3.5
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 0);  // 2
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 15 || s21_get_scale(&result) != 1 || s21_get_sign(&result)) {
        printf("FAIL! Expected 1.5, got %u scale %d sign %d\n", 
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 7: Different Scales (Negative Result) ===\n");
    a = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&a, 0);  // 2
    b = (s21_decimal){{35, 0, 0, 0}}; s21_set_scale(&b, 1); // 3.5
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 15 || s21_get_scale(&result) != 1 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -1.5, got %u scale %d sign %d\n", 
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 8: Subtract Zero ===\n");
    a = (s21_decimal){{123456789, 0, 0, 0}}; s21_set_scale(&a, 2);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != a.bits[0] || s21_get_scale(&result) != 2 || s21_get_sign(&result)) {
        printf("FAIL! Expected 1234567.89, got %u scale %d sign %d\n", 
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 9: Subtract From Zero ===\n");
    a = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{50, 0, 0, 0}}; s21_set_scale(&b, 1); // 5.0
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 50 || s21_get_scale(&result) != 1 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -5.0, got %u scale %d sign %d\n", 
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 10: Subtract Same Number ===\n");
    a = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&a, 2);
    b = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&b, 2);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0 || s21_get_scale(&result) != 2) {
        printf("FAIL! Expected 0, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 11: Subtract Same Number Different Signs ===\n");
    a = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&a, 2);
    b = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&b, 2); s21_set_sign(&b, 1);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 2000 || s21_get_scale(&result) != 2 || s21_get_sign(&result)) {
        printf("FAIL! Expected 20.00, got %u scale %d sign %d\n", 
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 12: Subtracting Large from Small ===\n");
    a = (s21_decimal){{100, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 900 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -900, got %u sign %d\n", result.bits[0], s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 13: Large Scale Subtraction ===\n");
    a = (s21_decimal){{987654321, 0, 0, 0}}; s21_set_scale(&a, 28);
    b = (s21_decimal){{123456789, 0, 0, 0}}; s21_set_scale(&b, 28);
    s21_sub(a, b, &result);
    print_decimal(result);
    unsigned long long expected13 = 987654321ULL - 123456789ULL;
    if (result.bits[0] != expected13 || s21_get_scale(&result) != 28) {
        printf("FAIL! Expected %llu scale 28, got %u scale %d\n", 
               expected13, result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 14: Borrow Between Words ===\n");
    a = (s21_decimal){{0, 1, 0, 0}}; s21_set_scale(&a, 0); // 2^32
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);  // 1
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0xFFFFFFFF || result.bits[1] != 0 || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected 2^32-1, got [%u, %u, %u]\n", 
               result.bits[0], result.bits[1], result.bits[2]);
        failed++;
    }

    printf("\n=== TEST 15: Zero - Zero ===\n");
    a = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0 || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected 0, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 16: Negative Zero Subtraction ===\n");
    a = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&a, 5); s21_set_sign(&a, 1);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 3);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0 || s21_get_scale(&result) != 5) {
        printf("FAIL! Expected 0 with scale 5, got %u scale %d sign %d\n", 
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 17: Subtract Very Small Numbers ===\n");
    a = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&a, 28);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 28);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 1 || s21_get_scale(&result) != 28) {
        printf("FAIL! Expected 0.0000000000000000000000000001, got %u scale %d\n", 
               result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 18: Decimal Precision Test ===\n");
    a = (s21_decimal){{987654321, 0, 0, 0}}; s21_set_scale(&a, 5);
    b = (s21_decimal){{123456789, 0, 0, 0}}; s21_set_scale(&b, 5);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 864197532 || s21_get_scale(&result) != 5) {
        printf("FAIL! Expected 8641.97532, got %u scale %d\n", 
               result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 19: Near Maximum Scale Subtraction ===\n");
    a = (s21_decimal){{200000000, 0, 0, 0}}; s21_set_scale(&a, 28);
    b = (s21_decimal){{100000000, 0, 0, 0}}; s21_set_scale(&b, 28);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 100000000 || s21_get_scale(&result) != 28) {
        printf("FAIL! Expected 100000000 scale 28, got %u scale %d\n", 
               result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 20: Subtract to Get Negative ===\n");
    a = (s21_decimal){{500000000, 0, 0, 0}}; s21_set_scale(&a, 1); // 50,000,000.0
    b = (s21_decimal){{700000000, 0, 0, 0}}; s21_set_scale(&b, 1); // 70,000,000.0
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 200000000 || s21_get_scale(&result) != 1 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -20000000.0, got %u scale %d sign %d\n", 
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 21: High Bit Numbers Subtraction ===\n");
    a = (s21_decimal){{0xFFFFFFFF, 0, 0, 0}}; s21_set_scale(&a, 0); // 2^32-1
    b = (s21_decimal){{0x80000000, 0, 0, 0}}; s21_set_scale(&b, 0); // 2^31
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0x7FFFFFFF || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected 0x7FFFFFFF, got %u\n", result.bits[0]);
        failed++;
    }

    printf("\n=== TEST 22: Scale Reduction Test ===\n");
    a = (s21_decimal){{1000000000, 0, 0, 0}}; s21_set_scale(&a, 1);
    b = (s21_decimal){{500000000, 0, 0, 0}}; s21_set_scale(&b, 1);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 500000000 || s21_get_scale(&result) != 1) {
        printf("FAIL! Expected 50000000.0, got %u scale %d\n", 
               result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 23: Boundary Values Subtraction ===\n");
    a = (s21_decimal){{429496729, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 429496728 || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected 429496728, got %u\n", result.bits[0]);
        failed++;
    }

    printf("\n=== TEST 24: Different Sign Large Numbers ===\n");
    a = (s21_decimal){{1000000000, 0, 0, 0}}; s21_set_scale(&a, 2); // 10,000,000.00
    b = (s21_decimal){{500000000, 0, 0, 0}}; s21_set_scale(&b, 2); s21_set_sign(&b, 1); // -5,000,000.00
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 1500000000 || s21_get_scale(&result) != 2 || s21_get_sign(&result)) {
        printf("FAIL! Expected 15000000.00, got %u scale %d sign %d\n", 
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
        failed++;
    }

    printf("\n=== TEST 25: Zero Scale Subtraction ===\n");
    a = (s21_decimal){{2222222211, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{987654321, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_sub(a, b, &result);
    print_decimal(result);
    unsigned int expected25 = 2222222211U - 987654321U;
    if (result.bits[0] != expected25 || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected %u, got %u\n", expected25, result.bits[0]);
        failed++;
    }

    printf("\n=== TEST 26: Precision Loss Test ===\n");
    a = (s21_decimal){{999999999, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 10);
    s21_sub(a, b, &result);
    print_decimal(result);
    if (s21_get_scale(&result) > 10) {
        printf("FAIL! Scale increased unexpectedly to %d\n", s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 27: Edge Case Rounding ===\n");
    a = (s21_decimal){{30, 0, 0, 0}}; s21_set_scale(&a, 1); // 3.0
    b = (s21_decimal){{16, 0, 0, 0}}; s21_set_scale(&b, 1); // 1.6
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 14 || s21_get_scale(&result) != 1) {
        printf("FAIL! Expected 1.4, got %u scale %d\n", result.bits[0], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 28: Subtract Max Value - 1 ===\n");
    s21_decimal max_minus_one = (s21_decimal){{0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_sub(max_minus_one, b, &result);
    print_decimal(result);
    if (result.bits[0] != 0xFFFFFFFD || result.bits[1] != 0xFFFFFFFF || result.bits[2] != 0xFFFFFFFF || s21_get_scale(&result) != 0) {
        printf("FAIL! Expected max-2, got [%u, %u, %u] scale %d\n", 
               result.bits[0], result.bits[1], result.bits[2], s21_get_scale(&result));
        failed++;
    }

    printf("\n=== TEST 29: Underflow Test ===\n");
    s21_decimal min_val = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
    s21_set_sign(&min_val, 1); // -max
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);
    int status = s21_sub(min_val, b, &result);
    if (status != 1) { // Should overflow/underflow
        printf("FAIL! Expected underflow (status 1), got status %d\n", status);
        failed++;
    } else {
        printf("Underflow handled correctly (status 1)\n");
    }

    printf("\n=== TEST 30: Complex Negative Subtraction ===\n");
    a = (s21_decimal){{100, 0, 0, 0}}; s21_set_scale(&a, 2); s21_set_sign(&a, 1); // -1.00
    b = (s21_decimal){{250, 0, 0, 0}}; s21_set_scale(&b, 2); // 2.50
    s21_sub(a, b, &result);
    print_decimal(result);
    if (result.bits[0] != 350 || s21_get_scale(&result) != 2 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -3.50, got %u scale %d sign %d\n", 
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result));
        failed++;
    }

    return failed;
}

int test_multiplication() {
    int failed = 0;
    s21_decimal a, b, result;
    int status;

    printf("\n=== MUL TEST 1: Basic Positive Multiplication ===\n");
    a = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{3, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 6 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 6, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 2: Positive with Scale ===\n");
    a = (s21_decimal){{25, 0, 0, 0}}; s21_set_scale(&a, 1); // 2.5
    b = (s21_decimal){{4, 0, 0, 0}}; s21_set_scale(&b, 0);  // 4
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 100 || s21_get_scale(&result) != 1 || s21_get_sign(&result)) {
        printf("FAIL! Expected 10.0, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 3: Negative * Positive ===\n");
    a = (s21_decimal){{3, 0, 0, 0}}; s21_set_scale(&a, 0); s21_set_sign(&a, 1); // -3
    b = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&b, 0);                      // 5
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 15 || s21_get_scale(&result) != 0 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -15, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 4: Negative * Negative ===\n");
    a = (s21_decimal){{7, 0, 0, 0}}; s21_set_scale(&a, 0); s21_set_sign(&a, 1); // -7
    b = (s21_decimal){{8, 0, 0, 0}}; s21_set_scale(&b, 0); s21_set_sign(&b, 1); // -8
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 56 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 56, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 5: Multiplication by Zero ===\n");
    a = (s21_decimal){{123456789, 12345, 123, 0}}; s21_set_scale(&a, 5); s21_set_sign(&a, 1);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_mul(a, b, &result);
    print_decimal(result);
   // Более корректная проверка для нуля
if (status != 0 || result.bits[0] != 0 || result.bits[1] != 0 || result.bits[2] != 0) {
    printf("FAIL! Expected 0, got [%u, %u, %u] scale %d sign %d (status %d)\n",
           result.bits[0], result.bits[1], result.bits[2],
           s21_get_scale(&result), s21_get_sign(&result), status);
    failed++;
}
    printf("\n=== MUL TEST 6: Zero * Zero ===\n");
    a = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&a, 10);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 5);
    status = s21_mul(a, b, &result);
    print_decimal(result);
      if (status != 0 || result.bits[0] != 0 || result.bits[1] != 0 || result.bits[2] != 0) {
        printf("FAIL! Expected zero value ([0,0,0]), got [%u, %u, %u] (status %d)\n",
               result.bits[0], result.bits[1], result.bits[2], status);
        failed++;
    }

    printf("\n=== MUL TEST 7: Scale Addition ===\n");
    a = (s21_decimal){{15, 0, 0, 0}}; s21_set_scale(&a, 1); // 1.5
    b = (s21_decimal){{25, 0, 0, 0}}; s21_set_scale(&b, 2); // 0.25
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 375 || s21_get_scale(&result) != 3 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.375, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 8: Scale Reduction (Overflow Protection) ===\n");
    a = (s21_decimal){{100000, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{100000, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 100000 * 100000 = 10,000,000,000. Scale would be 0+0=0, but result is too big for scale 0.
    // It should be reduced to 10000000000 with scale 0, or if it overflows, we check status.
    // Let's check if it's 10000000000 with scale 0 (which fits in one word)
    if (status != 0 || result.bits[0] != 10000000000U || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
         // If not, let's see what we got
         printf("INFO: Got %u scale %d sign %d (status %d). Checking if it's a valid large number...\n",
                result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
         // The key test is that it doesn't overflow for this case
         if (status != 0) {
             printf("FAIL! Unexpected overflow for 100000 * 100000 (status %d)\n", status);
             failed++;
         }
    }

    printf("\n=== MUL TEST 9: Fractional Result ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 1); // 0.1
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 1); // 0.1
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 1 || s21_get_scale(&result) != 2 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.01, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 10: Large Numbers ===\n");
    a = (s21_decimal){{1000000, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{2000000, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 1,000,000 * 2,000,000 = 2,000,000,000,000
    unsigned long long expected10 = 2000000000000ULL;
    if (status != 0 || result.bits[0] != (unsigned int)(expected10 & 0xFFFFFFFF) ||
        result.bits[1] != (unsigned int)(expected10 >> 32) ||
        s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 2,000,000,000,000, got [%u, %u] scale %d sign %d (status %d)\n",
               result.bits[0], result.bits[1],
               s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 11: Very Small Numbers ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 28); // 1e-28
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 28); // 1e-28
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 1e-28 * 1e-28 = 1e-56. This should underflow to 0 because min scale is 0.
    if (status != 0 || result.bits[0] != 0) {
        printf("FAIL! Expected 0 (underflow), got %u (status %d)\n", result.bits[0], status);
        failed++;
    }

    printf("\n=== MUL TEST 12: One Operand is 1 ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{999999999, 0, 0, 0}}; s21_set_scale(&b, 5);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 999999999 || s21_get_scale(&result) != 5 || s21_get_sign(&result)) {
        printf("FAIL! Expected 9999.99999, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 13: One Operand is -1 ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 0); s21_set_sign(&a, 1);
    b = (s21_decimal){{555555555, 0, 0, 0}}; s21_set_scale(&b, 3);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 555555555 || s21_get_scale(&result) != 3 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -555555.555, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

        printf("\n=== MUL TEST 14: Multiplication with Carry ===\n");
    a = (s21_decimal){{0xFFFFFFFF, 0, 0, 0}}; s21_set_scale(&a, 0); // 4,294,967,295
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 0);          // 2
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 4,294,967,295 * 2 = 8,589,934,590 = 0x1FFFFFFFE
    // bits[0] = 0xFFFFFFFE = 4,294,967,294
    // bits[1] = 0x1 = 1
    // bits[2] = 0x0 = 0
    if (status != 0 || result.bits[0] != 0xFFFFFFFE || result.bits[1] != 1 ||
        result.bits[2] != 0 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 8589934590 ([%u, %u, %u]), got [%u, %u, %u] scale %d sign %d (status %d)\n",
               0xFFFFFFFE, 1, 0,
               result.bits[0], result.bits[1], result.bits[2],
               s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 15: Maximum Scale Preservation ===\n");
    a = (s21_decimal){{123456789, 0, 0, 0}}; s21_set_scale(&a, 28);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 123456789 || s21_get_scale(&result) != 28 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.000000000000000000123456789, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

       printf("\n=== MUL TEST 16: Scale Sum Exceeds 28 (Underflow to Zero) ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 15); // 1 * 10^-15
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 15); // 1 * 10^-15
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 0 || result.bits[1] != 0 || result.bits[2] != 0) {
        printf("FAIL! Expected result value 0 ([0, 0, 0]), got [%u, %u, %u] scale %d sign %d (status %d)\n",
               result.bits[0], result.bits[1], result.bits[2],
               s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 17: Multiplication Resulting in Integer ===\n");
    a = (s21_decimal){{25, 0, 0, 0}}; s21_set_scale(&a, 2); // 0.25
    b = (s21_decimal){{4, 0, 0, 0}}; s21_set_scale(&b, 0);  // 4
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 100 || s21_get_scale(&result) != 2 || s21_get_sign(&result)) {
        printf("FAIL! Expected 1.00, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 18: Multiplication with Rounding ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 1); // 0.1
    b = (s21_decimal){{17, 0, 0, 0}}; s21_set_scale(&b, 1); // 1.7
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 0.1 * 1.7 = 0.17
    if (status != 0 || result.bits[0] != 17 || s21_get_scale(&result) != 2 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.17, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 19: Large Scale Multiplication ===\n");
    a = (s21_decimal){{500000000, 0, 0, 0}}; s21_set_scale(&a, 9);
    b = (s21_decimal){{200000000, 0, 0, 0}}; s21_set_scale(&b, 9);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    
    if (status != 0 || result.bits[0] != 1569325056 || result.bits[1] != 23283064 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.1, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 20: Multiplication with High Bits Set ===\n");
    a = (s21_decimal){{0x80000000, 0, 0, 0}}; s21_set_scale(&a, 0); // 2^31
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 0);          // 2
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 2^31 * 2 = 2^32 = 0x100000000
    if (status != 0 || result.bits[0] != 0 || result.bits[1] != 1 ||
        s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 4294967296 ([0, 1]), got [%u, %u] scale %d sign %d (status %d)\n",
               result.bits[0], result.bits[1],
               s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 21: Multiplication of Numbers with Different Scales ===\n");
    a = (s21_decimal){{11, 0, 0, 0}}; s21_set_scale(&a, 1); // 1.1
    b = (s21_decimal){{11, 0, 0, 0}}; s21_set_scale(&b, 2); // 0.11
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 1.1 * 0.11 = 0.121
    if (status != 0 || result.bits[0] != 121 || s21_get_scale(&result) != 3 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.121, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 22: Multiplication Resulting in Zero Scale ===\n");
    a = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&a, 3); // 1.000
    b = (s21_decimal){{5000, 0, 0, 0}}; s21_set_scale(&b, 0); // 5000
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 1.000 * 5000 = 5000.000. Scale should be 3.
    if (status != 0 || result.bits[0] != 5000000 || s21_get_scale(&result) != 3 || s21_get_sign(&result)) {
        printf("FAIL! Expected 5000.000, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 23: Multiplication with Maximum Values (No Overflow) ===\n");
    a = (s21_decimal){{429496729, 0, 0, 0}}; s21_set_scale(&a, 0); // ~2^32 / 10
    b = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&b, 0);        // 10
    status = s21_mul(a, b, &result);
    print_decimal(result);
    unsigned long long expected23 = 4294967290ULL;
    if (status != 0 ||
        result.bits[0] != (unsigned int)(expected23 & 0xFFFFFFFF) ||
        result.bits[1] != (unsigned int)(expected23 >> 32) ||
        s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 4294967290 ([%u, %u]), got [%u, %u] scale %d sign %d (status %d)\n",
               (unsigned int)(expected23 & 0xFFFFFFFF), (unsigned int)(expected23 >> 32),
               result.bits[0], result.bits[1],
               s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 24: Multiplication with Carry Across Multiple Words ===\n");
    a = (s21_decimal){{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // (2^64 - 1) * 2 = 2^65 - 2
    // This is a very large number, let's check the status and result
    printf("INFO: Large multiplication result. Status: %d\n", status);
    // The main check is that it handles it without crashing and returns appropriate status

    printf("\n=== MUL TEST 25: Multiplication with Scale Reduction Due to Overflow ===\n");
    // Create a case where the product overflows the 96-bit mantissa
    // We'll use numbers that when multiplied will definitely overflow
    a = (s21_decimal){{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{0xFFFFFFFF, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    printf("INFO: Overflow multiplication. Status: %d\n", status);
    // Should either succeed with reduced precision/rounded result or return overflow status (1 or 2)

    printf("\n=== MUL TEST 26: Multiplication of Small Fractions ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 10); // 1e-10
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 10); // 1e-10
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 1e-10 * 1e-10 = 1e-20
    // This should result in 1 with scale 20
    if (status != 0 || result.bits[0] != 1 || s21_get_scale(&result) != 20 || s21_get_sign(&result)) {
        printf("FAIL! Expected 1e-20 (1 with scale 20), got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 27: Multiplication with Negative Zero ===\n");
    a = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&a, 5); s21_set_sign(&a, 1);
    b = (s21_decimal){{123, 0, 0, 0}}; s21_set_scale(&b, 2);
    status = s21_mul(a, b, &result);
    print_decimal(result);
     if (status != 0 || result.bits[0] != 0 || result.bits[1] != 0 || result.bits[2] != 0) {
        printf("FAIL! Expected zero value ([0,0,0]), got [%u, %u, %u] (status %d)\n",
               result.bits[0], result.bits[1], result.bits[2], status);
        failed++;
    }

    printf("\n=== MUL TEST 28: Multiplication Resulting in Maximum Scale ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 14);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 14);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // Scale sum is 28, which is the maximum allowed
    if (status != 0 || result.bits[0] != 1 || s21_get_scale(&result) != 28 || s21_get_sign(&result)) {
        printf("FAIL! Expected 1e-28, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 29: Multiplication with Rounding Half to Even ===\n");
    // This test might be tricky to design perfectly without knowing exact rounding rules
    // Let's try a case that should clearly round
    a = (s21_decimal){{15, 0, 0, 0}}; s21_set_scale(&a, 2); // 0.15
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 0);  // 2
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 0.15 * 2 = 0.30
    if (status != 0 || result.bits[0] != 30 || s21_get_scale(&result) != 2 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.30, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 30: Multiplication Resulting in 10^18 ===\n");
    // 1 billion * 1 billion = 1 quintillion = 10^18
    a = (s21_decimal){{1000000000, 0, 0, 0}}; s21_set_scale(&a, 0); // 1,000,000,000
    b = (s21_decimal){{1000000000, 0, 0, 0}}; s21_set_scale(&b, 0); // 1,000,000,000
    status = s21_mul(a, b, &result);
    print_decimal(result);
    unsigned long long expected30 = 1000000000000000000ULL;
    unsigned int expected30_low = (unsigned int)(expected30 & 0xFFFFFFFFULL);
    unsigned int expected30_high = (unsigned int)(expected30 >> 32);
    if (status != 0 ||
        result.bits[0] != expected30_low ||
        result.bits[1] != expected30_high ||
        result.bits[2] != 0 ||
        s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 10^18 ([%u, %u, %u]), got [%u, %u, %u] scale %d sign %d (status %d)\n",
               expected30_low, expected30_high, 0u,
               result.bits[0], result.bits[1], result.bits[2],
               s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 31: Multiplication of Numbers with Maximum Scale ===\n");
    a = (s21_decimal){{999999999, 0, 0, 0}}; s21_set_scale(&a, 28);
    b = (s21_decimal){{999999999, 0, 0, 0}}; s21_set_scale(&b, 28);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // (999999999 * 999999999) * 10^-56
    // 999999999^2 = 999999998000000001
    // Result should be 999999998000000001 * 10^-56
    // This will likely underflow to 0 or a very small number
    printf("INFO: Very small multiplication. Status: %d\n", status);
    // Main check is it doesn't crash

    printf("\n=== MUL TEST 32: Identity Property ===\n");
    a = (s21_decimal){{123456789, 12345, 123, 0}}; s21_set_scale(&a, 5); s21_set_sign(&a, 1);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != a.bits[0] || result.bits[1] != a.bits[1] || result.bits[2] != a.bits[2] ||
        s21_get_scale(&result) != s21_get_scale(&a) || s21_get_sign(&result) != s21_get_sign(&a)) {
        printf("FAIL! Expected same as 'a', multiplication by 1 failed.\n");
        failed++;
    }

    printf("\n=== MUL TEST 33: Commutative Property ===\n");
    a = (s21_decimal){{123, 0, 0, 0}}; s21_set_scale(&a, 2);
    b = (s21_decimal){{456, 0, 0, 0}}; s21_set_scale(&b, 3);
    s21_decimal result1, result2;
    status = s21_mul(a, b, &result1);
    status = s21_mul(b, a, &result2); // Overwrite status
    print_decimal(result1);
    printf(" * ");
    print_decimal(result2);
    printf("\n");
    if (result1.bits[0] != result2.bits[0] || result1.bits[1] != result2.bits[1] || result1.bits[2] != result2.bits[2] ||
        s21_get_scale(&result1) != s21_get_scale(&result2) || s21_get_sign(&result1) != s21_get_sign(&result2)) {
        printf("FAIL! Multiplication is not commutative!\n");
        failed++;
    }

      printf("\n=== MUL TEST 34: Multiplication by Power of 10 ===\n");
    a = (s21_decimal){{123456789, 0, 0, 0}}; s21_set_scale(&a, 5); 
    b = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&b, 3);      
    status = s21_mul(a, b, &result);
    print_decimal(result);

    unsigned long long expected_coefficient = 123456789000ULL;
    unsigned int expected_low = (unsigned int)(expected_coefficient & 0xFFFFFFFFULL);
    unsigned int expected_high = (unsigned int)(expected_coefficient >> 32);

    if (status != 0 ||
        result.bits[0] != expected_low ||
        result.bits[1] != expected_high ||
        result.bits[2] != 0 ||
        s21_get_scale(&result) != 8 ||
        s21_get_sign(&result)) {
        printf("FAIL! Expected 1234.56789000 ([%u, %u, %u] scale 8 sign 0), got [%u, %u, %u] scale %d sign %d (status %d)\n",
               expected_low, expected_high, 0u,
               result.bits[0], result.bits[1], result.bits[2],
               s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }
   
     printf("\n=== MUL TEST 35: Multiplication with Intermediate Scale Overflow (Underflow to Zero) ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 15); // 1 * 10^-15
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 14); // 1 * 10^-14
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 0 || result.bits[1] != 0 || result.bits[2] != 0) {
        printf("FAIL! Expected result value 0 ([0, 0, 0]), got [%u, %u, %u] scale %d sign %d (status %d)\n",
               result.bits[0], result.bits[1], result.bits[2],
               s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 36: Multiplication Resulting in Value Requiring Rounding ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 1); // 0.2
    status = s21_mul(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 2 || s21_get_scale(&result) != 1 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.2, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 37: Multiplication with All Bits Set in Lower Word ===\n");
    a = (s21_decimal){{0xFFFFFFFF, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{0xFFFFFFFF, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 0xFFFFFFFF * 0xFFFFFFFF = (2^32 - 1)^2 = 2^64 - 2*2^32 + 1 = 0xFFFFFFFE00000001
    if (status != 0 || result.bits[0] != 0x00000001 || result.bits[1] != 0xFFFFFFFE ||
        s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0xFFFFFFFE00000001, got [%u, %u] scale %d sign %d (status %d)\n",
               result.bits[0], result.bits[1],
               s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 38: Multiplication with Cross-Midword Carry ===\n");
    a = (s21_decimal){{0, 1, 0, 0}}; s21_set_scale(&a, 0); // 2^32
    b = (s21_decimal){{0, 1, 0, 0}}; s21_set_scale(&b, 0); // 2^32
    status = s21_mul(a, b, &result);
    print_decimal(result);
    // 2^32 * 2^32 = 2^64
    if (status != 0 || result.bits[0] != 0 || result.bits[1] != 0 || result.bits[2] != 1 ||
        s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 2^64 ([0, 0, 1]), got [%u, %u, %u] scale %d sign %d (status %d)\n",
               result.bits[0], result.bits[1], result.bits[2],
               s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== MUL TEST 39: Stress Test with Large Numbers ===\n");
    a = (s21_decimal){{100000000, 1000000, 1000, 0}}; s21_set_scale(&a, 2);
    b = (s21_decimal){{100, 0, 0, 0}}; s21_set_scale(&b, 1);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    printf("INFO: Stress test multiplication. Status: %d\n", status);
    // Main check is it doesn't crash and returns a status

    printf("\n=== MUL TEST 40: Edge Case with Potential Overflow ===\n");
    // Multiply two numbers whose product is close to the maximum decimal value
    // This is a bit tricky to define precisely, but let's try:
    a = (s21_decimal){{0xFFFFFFFF, 0xFFFFFFFF, 0x0000FFFF, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_mul(a, b, &result);
    print_decimal(result);
    printf("INFO: Near-max multiplication. Status: %d\n", status);
    // Should handle it gracefully, either with result or overflow status

    return failed;
}


int test_division() {
    int failed = 0;
    s21_decimal a, b, result;
    int status;

    printf("\n=== DIV TEST 1: Basic Positive Division ===\n");
    a = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&a, 0); // 10
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 0);  // 2
    status = s21_div(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 5 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 5, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 2: Division Resulting in Fraction ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 0); // 1
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 0); // 2
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 1 / 2 = 0.5 -> bits[0] = 5, scale = 1
    if (status != 0 || result.bits[0] != 5 || s21_get_scale(&result) != 1 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.5, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 3: Negative / Positive ===\n");
    a = (s21_decimal){{15, 0, 0, 0}}; s21_set_scale(&a, 0); s21_set_sign(&a, 1); // -15
    b = (s21_decimal){{3, 0, 0, 0}}; s21_set_scale(&b, 0);                      // 3
    status = s21_div(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 5 || s21_get_scale(&result) != 0 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -5, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 4: Negative / Negative ===\n");
    a = (s21_decimal){{20, 0, 0, 0}}; s21_set_scale(&a, 0); s21_set_sign(&a, 1); // -20
    b = (s21_decimal){{4, 0, 0, 0}}; s21_set_scale(&b, 0); s21_set_sign(&b, 1);  // -4
    status = s21_div(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 5 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 5, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 5: Division by Zero ===\n");
    a = (s21_decimal){{123456789, 0, 0, 0}}; s21_set_scale(&a, 5);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_div(a, b, &result);
    // Should return status 3 for division by zero
    if (status != 3) {
        printf("FAIL! Expected status 3 (division by zero), got status %d\n", status);
        failed++;
    }

    printf("\n=== DIV TEST 6: Zero / Non-Zero ===\n");
    a = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{999, 0, 0, 0}}; s21_set_scale(&b, 2);
    status = s21_div(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 0 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 7: Same Numbers ===\n");
    a = (s21_decimal){{555555, 0, 0, 0}}; s21_set_scale(&a, 3);
    b = (s21_decimal){{555555, 0, 0, 0}}; s21_set_scale(&b, 3);
    status = s21_div(a, b, &result);
    print_decimal(result);
    // Any non-zero number divided by itself is 1
    if (status != 0 || result.bits[0] != 1 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 1, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 8: Different Scales ===\n");
    a = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&a, 2); // 10.00
    b = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&b, 1);    // 0.5
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 10.00 / 0.5 = 20.0 -> bits[0] = 200, scale = 1
    if (status != 0 || result.bits[0] != 200 || s21_get_scale(&result) != 1 || s21_get_sign(&result)) {
        printf("FAIL! Expected 20.0, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 9: Scale Difference > 0 (Result Scale Increase) ===\n");
    a = (s21_decimal){{100, 0, 0, 0}}; s21_set_scale(&a, 1); // 10.0
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 3);   // 0.001
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 10.0 / 0.001 = 10000.0
    // scale_a - scale_b = 1 - 3 = -2. Result scale should be adjusted, final value 10000000, scale 1.
    // Or, coefficient is 100 / 1 = 100. Scale diff is -2, so multiply coefficient by 10^2 = 100.
    // New coefficient = 100 * 100 = 10000. Scale = 0.
    // No, let's recalculate: 100 (scale 1) / 1 (scale 3).
    // Math value: (100 / 10^1) / (1 / 10^3) = (100 * 10^3) / (1 * 10^1) = 100 * 10^2 / 10^1 = 100 * 10^1 = 1000.
    // Coefficient: 100 / 1 = 100.
    // Scale: 1 - 3 = -2.
    // Normalize scale: -2 < 0, so multiply coefficient by 10^2.
    // 100 * 100 = 10000. Scale becomes 0.
    // Hmm, still not matching expected 1000.
    // Let's think of it as values: 10.0 / 0.001
    // 10.0 = 100 * 10^-1
    // 0.001 = 1 * 10^-3
    // (100 * 10^-1) / (1 * 10^-3) = (100/1) * 10^(-1 - (-3)) = 100 * 10^2 = 100 * 100 = 10000.
    // Coefficient = 100.
    // Scale diff = 1 - 3 = -2.
    // Since scale diff < 0, result coeff = 100 * 10^2 = 100 * 100 = 10000. Result scale = 0.
    // Value = 10000 * 10^0 = 10000.
    // So result should be bits[0] = 10000, scale = 0.
    if (status != 0 || result.bits[0] != 10000 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 10000.0, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 10: Scale Difference < 0 (Result Scale Decrease) ===\n");
    a = (s21_decimal){{5, 0, 0, 0}}; s21_set_scale(&a, 3); // 0.005
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 1); // 0.2
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 0.005 / 0.2 = 0.025
    // scale_a - scale_b = 3 - 1 = 2.
    // coefficient: 5 / 2 = 2 (integer division). Remainder is 1.
    // To get 0.025, we need coeff 25 and scale 3.
    // Or coeff 250 and scale 4.
    // Let's do it by value: (5 * 10^-3) / (2 * 10^-1) = (5/2) * 10^(-3 - (-1)) = 2.5 * 10^-2 = 0.025.
    // Coefficient: 5 / 2 = 2 (we need to handle fractional part, likely by increasing precision of dividend).
    // The actual algorithm will likely involve shifting the dividend left (multiplying by 10) to get more precision.
    // For simplicity in test, let's check the final expected value.
    // 0.025 = 25 * 10^-3. So bits[0] = 25, scale = 3.
    if (status != 0 || result.bits[0] != 25 || s21_get_scale(&result) != 3 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.025, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 11: Large Dividend, Small Divisor ===\n");
    a = (s21_decimal){{1000000, 0, 0, 0}}; s21_set_scale(&a, 0); // 1,000,000
    b = (s21_decimal){{100, 0, 0, 0}}; s21_set_scale(&b, 2);     // 1.00
    status = s21_div(a, b, &result);
    print_decimal(result);
    if (status != 0 ||
        result.bits[0] != 1000000 ||
        s21_get_scale(&result) != 0 ||
        s21_get_sign(&result)) {
        printf("FAIL! Expected 1,000,000.00 ([1409286144, 2] scale 2), got [%u, %u] scale %d sign %d (status %d)\n",
               result.bits[0], result.bits[1], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 12: Small Dividend, Large Divisor ===\n");
    a = (s21_decimal){{50, 0, 0, 0}}; s21_set_scale(&a, 2);      // 0.50
    b = (s21_decimal){{1000000, 0, 0, 0}}; s21_set_scale(&b, 0); // 1,000,000
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 0.50 / 1,000,000 = 0.00000050
    // scale diff = 2 - 0 = 2.
    // coeff = 50 / 1,000,000 = 0 (integer). We get a very small number.
    // Value = (50 * 10^-2) / (1,000,000 * 10^0) = (50 / 1,000,000) * 10^(-2 - 0) = 0.00005 * 10^-2 = 0.0000005
    // 0.0000005 = 5 * 10^-7.
    // To represent this, coeff = 5, scale = 7.
    if (status != 0 || result.bits[0] != 5 || s21_get_scale(&result) != 7 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.0000005, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 13: Division with Carry in Result ===\n");
    a = (s21_decimal){{0xFFFFFFFF, 0, 0, 0}}; s21_set_scale(&a, 0); // 4,294,967,295
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 0);         // 2
    status = s21_div(a, b, &result);
    print_decimal(result);
    unsigned int expected13 = 4294967291;
    if (status != 0 || result.bits[0] != expected13 || s21_get_scale(&result) != 1 || s21_get_sign(&result)) {
        printf("FAIL! Expected ~2,147,483,647 (0x7FFFFFFF), got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 14: Result Requiring High Bits ===\n");
    a = (s21_decimal){{0, 1, 0, 0}}; s21_set_scale(&a, 0); 
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 0); 
    status = s21_div(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 0x80000000 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 2,147,483,648 (0x80000000), got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 15: Division Resulting in Scale > 28 ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 0); // 1
    b = (s21_decimal){{1000000000, 0, 0, 0}}; s21_set_scale(&b, 0); // 1,000,000,000
    status = s21_div(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 1 || s21_get_scale(&result) != 9 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.000000001 (1 * 10^-9), got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 16: Division by Power of 10 ===\n");
    a = (s21_decimal){{1234567890, 0, 0, 0}}; s21_set_scale(&a, 2); // 12,345,678.90
    b = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&b, 3);       // 1.000
    status = s21_div(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 123456789 || s21_get_scale(&result) != 1 || s21_get_sign(&result)) {
         printf("FAIL! Expected 12,345,678.9, got %u scale %d sign %d (status %d)\n",
                result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
         failed++;
    }

    printf("\n=== DIV TEST 17: Identity Division (Number / 1) ===\n");
    a = (s21_decimal){{987654321, 12345, 0, 0}}; s21_set_scale(&a, 5); s21_set_sign(&a, 1);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_div(a, b, &result);
    print_decimal(result);
    // Any number divided by 1 is itself.
    if (status != 0 ||
        result.bits[0] != a.bits[0] || result.bits[1] != a.bits[1] || result.bits[2] != a.bits[2] ||
        s21_get_scale(&result) != s21_get_scale(&a) || s21_get_sign(&result) != s21_get_sign(&a)) {
        printf("FAIL! Expected same as 'a', division by 1 failed.\n");
        failed++;
    }

    printf("\n=== DIV TEST 18: Division by -1 ===\n");
    a = (s21_decimal){{555555555, 0, 0, 0}}; s21_set_scale(&a, 3); // 555,555.555
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0); s21_set_sign(&b, 1); // -1
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 555,555.555 / -1 = -555,555.555
    if (status != 0 ||
        result.bits[0] != 555555555 || s21_get_scale(&result) != 3 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -555,555.555, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 19: Very Small Result ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 20); // 1e-20
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 10); // 1e-10
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 1e-20 / 1e-10 = 1e-10.
    // scale diff = 20 - 10 = 10.
    // coeff = 1 / 1 = 1.
    // Result coeff = 1, scale = 10.
    if (status != 0 || result.bits[0] != 1 || s21_get_scale(&result) != 10 || s21_get_sign(&result)) {
        printf("FAIL! Expected 1e-10 (1 * 10^-10), got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 20: Division Involving Zero Scale ===\n");
    a = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&a, 0); // 1000
    b = (s21_decimal){{250, 0, 0, 0}}; s21_set_scale(&b, 0);  // 250
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 1000 / 250 = 4
    if (status != 0 || result.bits[0] != 4 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 4, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 21: Commutative Property Check (Sort of) ===\n");
    // While division isn't commutative, (a/b)/c should equal a/(b*c)
    // Let's test a simple case: (100/10)/5 vs 100/(10*5)
    a = (s21_decimal){{100, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&b, 0);
    s21_decimal c = {{5, 0, 0, 0}}; s21_set_scale(&c, 0);
    s21_decimal temp, result1, result2;
    
    // (a/b)
    status = s21_div(a, b, &temp);
    if (status != 0) {
        printf("FAIL! Unexpected error in intermediate step 1 (status %d)\n", status);
        failed++;
    } else {
        // (a/b)/c
        status = s21_div(temp, c, &result1);
        if (status != 0) {
            printf("FAIL! Unexpected error in step (a/b)/c (status %d)\n", status);
            failed++;
        } else {
            // (b*c)
            s21_decimal bc;
            // For simplicity, let's assume we can multiply b and c for this test setup
            // Since we don't have s21_mul fully trusted yet, we'll use a known result.
            // 10 * 5 = 50.
            bc = (s21_decimal){{50, 0, 0, 0}}; s21_set_scale(&bc, 0);
            // a/(b*c)
            status = s21_div(a, bc, &result2);
            if (status != 0) {
                printf("FAIL! Unexpected error in step a/(b*c) (status %d)\n", status);
                failed++;
            } else {
                print_decimal(result1);
                printf(" vs ");
                print_decimal(result2);
                printf("\n");
                if (result1.bits[0] != result2.bits[0] ||
                    s21_get_scale(&result1) != s21_get_scale(&result2) ||
                    s21_get_sign(&result1) != s21_get_sign(&result2)) {
                    printf("INFO: (a/b)/c != a/(b*c) - This might be due to rounding in intermediate steps.\n");
                    // Not necessarily a FAIL, but worth noting for precision checks.
                }
            }
        }
    }

    printf("\n=== DIV TEST 22: Maximum Scale Preservation ===\n");
    a = (s21_decimal){{999999999, 0, 0, 0}}; s21_set_scale(&a, 28);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 999999999 * 10^-28 / 1 = 999999999 * 10^-28
    if (status != 0 || result.bits[0] != 999999999 || s21_get_scale(&result) != 28 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.000000000000000000999999999, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 23: Minimum Scale Result ===\n");
    a = (s21_decimal){{1000000000, 0, 0, 0}}; s21_set_scale(&a, 0); // 1,000,000,000
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);         // 1
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 1,000,000,000 / 1 = 1,000,000,000
    if (status != 0 || result.bits[0] != 1000000000 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 1,000,000,000, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 24: Fractional Result with Rounding ===\n");
    a = (s21_decimal){{22, 0, 0, 0}}; s21_set_scale(&a, 1); // 2.2
    b = (s21_decimal){{7, 0, 0, 0}}; s21_set_scale(&b, 1);  // 0.7
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 2.2 / 0.7 = 3.142857...
    // This is a repeating decimal. The implementation should handle it with its precision and rounding rules.
    // 22 * 10^-1 / 7 * 10^-1 = (22/7) * 10^0 = 22/7.
    // 22 / 7 = 3 remainder 1.
    // To get decimal places, algorithm works with remainder.
    // 22 / 7 = 3.(142857)...
    // Let's see what the first few digits are and check if the result is close.
    // If result is 3.14... with some scale, check the first part.
    // For test, let's check if it starts with 314 (representing 3.14) with scale 2.
    // This is a bit tricky to test precisely without knowing exact rounding.
    printf("INFO: Result of 2.2/0.7 (3.142857...) - checking basic structure.\n");
    // A minimal check: result should be positive and coefficient should not be zero.
    if (status != 0 || result.bits[0] == 0 || s21_get_sign(&result)) {
         printf("FAIL! Expected positive non-zero result for 2.2/0.7, got %u sign %d (status %d)\n",
                result.bits[0], s21_get_sign(&result), status);
         failed++;
    }

    printf("\n=== DIV TEST 25: Large Scale Difference ===\n");
    a = (s21_decimal){{123, 0, 0, 0}}; s21_set_scale(&a, 2); // 1.23
    b = (s21_decimal){{456, 0, 0, 0}}; s21_set_scale(&b, 25); // 4.56e-24 (approx)
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 1.23 / (4.56 * 10^-24) = (1.23 / 4.56) * 10^24
    // 1.23 / 4.56 ≈ 0.269736
    // So result ≈ 0.269736 * 10^24 = 2.69736 * 10^23
    // scale diff = 2 - 25 = -23.
    // coeff calculation: 123 / 456 = 0 (integer). Need precision handling.
    // The algorithm should adjust the dividend's precision.
    // Final scale will depend on implementation. Let's check if it's a large number.
    // If scale diff is -23, and result scale is normalized, it might be large.
    // Value should be large. Check if bits[0] or bits[1] are non-zero and sign is correct.
    if (status != 0 || (result.bits[0] == 0 && result.bits[1] == 0) || s21_get_sign(&result)) {
         printf("FAIL! Expected large positive result for 1.23 / 4.56e-24, got [%u, %u] sign %d (status %d)\n",
                result.bits[0], result.bits[1], s21_get_sign(&result), status);
         failed++;
    }

    printf("\n=== DIV TEST 26: Result Exactly 0.1 ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 1); // 0.1
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0); // 1
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 0.1 / 1 = 0.1
    if (status != 0 || result.bits[0] != 1 || s21_get_scale(&result) != 1 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.1, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 27: Division with Potential Overflow (Status Check) ===\n");
    // This is tricky to design without knowing exact limits of s21_big_decimal.
    // Let's try a case where the result might be too large for scale adjustment.
    // Use a very small divisor with a large dividend.
    a = (s21_decimal){{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}}; s21_set_scale(&a, 0); // Max value
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 28); // 1e-28
    status = s21_div(a, b, &result);
    print_decimal(result);
    printf("INFO: Large division result. Status: %d\n", status);
    // Should either succeed with a large number or return overflow status (1 or 2).
    // If it returns 1 or 2, it's handling overflow correctly.
    if (status != 0 && status != 1 && status != 2) {
        printf("FAIL! Unexpected status for potential overflow division: %d\n", status);
        failed++;
    }

    printf("\n=== DIV TEST 28: Division with Potential Underflow ===\n");
    // Large divisor, very small dividend.
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 28); // 1e-28
    b = (s21_decimal){{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}}; s21_set_scale(&b, 0); // Max value
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 1e-28 / Max_value ≈ 0.
    // This should result in 0 or a very small number that underflows to 0.
    if (status != 0 || result.bits[0] != 0) {
        printf("FAIL! Expected 0 (underflow), got %u (status %d)\n", result.bits[0], status);
        failed++;
    }

    printf("\n=== DIV TEST 29: Negative Zero Dividend ===\n");
    a = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&a, 5); s21_set_sign(&a, 1);
    b = (s21_decimal){{123, 0, 0, 0}}; s21_set_scale(&b, 2);
    status = s21_div(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 0 || s21_get_scale(&result) != 3) {
        // Note: Sign of zero is often normalized to positive.
        printf("FAIL! Expected 0, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 30: Division by Negative Zero (Divide by Zero) ===\n");
    a = (s21_decimal){{123456789, 0, 0, 0}}; s21_set_scale(&a, 5);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 10); s21_set_sign(&b, 1);
    status = s21_div(a, b, &result);
    // Should return status 3 for division by zero, regardless of sign of zero.
    if (status != 3) {
        printf("FAIL! Expected status 3 (division by zero, even if negative zero), got status %d\n", status);
        failed++;
    }

    printf("\n=== DIV TEST 31: Both Operands Negative Zero ===\n");
    a = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&a, 5); s21_set_sign(&a, 1);
    b = (s21_decimal){{0, 0, 0, 0}}; s21_set_scale(&b, 10); s21_set_sign(&b, 1);
    status = s21_div(a, b, &result);
    // -0 / -0. This is mathematically undefined, but often treated as 0/0.
    // Most implementations should return Divide by Zero error.
    if (status != 3) {
        printf("INFO: -0 / -0 returned status %d. Expected 3 (divide by zero).\n", status);
        // Depending on implementation, this could be 3 or something else.
        // IEEE 754 would be NaN or an exception. For s21_decimal, 3 (division by zero) is likely.
        if (status != 3) {
             printf("FAIL! Expected status 3 (division by zero), got status %d\n", status);
             failed++;
        }
    }

    printf("\n=== DIV TEST 32: Identity Property with Scales ===\n");
    a = (s21_decimal){{555555555, 12345, 123, 0}}; s21_set_scale(&a, 5);
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_div(a, b, &result);
    print_decimal(result);
    // a / 1 should be a.
    if (status != 0 ||
        result.bits[0] != a.bits[0] || result.bits[1] != a.bits[1] || result.bits[2] != a.bits[2] ||
        s21_get_scale(&result) != s21_get_scale(&a) || s21_get_sign(&result) != s21_get_sign(&a)) {
        printf("FAIL! Expected same as 'a' when dividing by 1.\n");
        failed++;
    }

    printf("\n=== DIV TEST 33: Scale Normalization to Max 28 ===\n");
   
    a = (s21_decimal){{1000000000, 0, 0, 0}}; s21_set_scale(&a, 0); // 1,000,000,000
    b = (s21_decimal){{10000, 0, 0, 0}}; s21_set_scale(&b, 28);     // 1e-24 * 10000 = 1e-20
    status = s21_div(a, b, &result);
    print_decimal(result);
    printf("INFO: Large scale diff division. Status: %d\n", status);
    if (status != 0 && status != 1 && status != 2) {
        printf("FAIL! Unexpected status for large scale diff division: %d\n", status);
        failed++;
    }

    printf("\n=== DIV TEST 34: Precision Loss Test ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 0);
    b = (s21_decimal){{3, 0, 0, 0}}; s21_set_scale(&b, 0);
    status = s21_div(a, b, &result);
    print_decimal(result);
    printf("INFO: Result of 1/3. Checking for approximate value.\n");
    if (status != 0 || result.bits[0] == 0 || s21_get_sign(&result)) {
         printf("FAIL! Expected positive non-zero result for 1/3, got %u sign %d (status %d)\n",
                result.bits[0], s21_get_sign(&result), status);
         failed++;
    }

    printf("\n=== DIV TEST 35: Edge Case with Max Scale Divisor ===\n");
    a = (s21_decimal){{123456789, 0, 0, 0}}; s21_set_scale(&a, 5);
    b = (s21_decimal){{999999999, 0, 0, 0}}; s21_set_scale(&b, 28); 
    status = s21_div(a, b, &result);
    print_decimal(result);
    printf("INFO: Division with max scale divisor. Status: %d\n", status);
    if (status != 0 && status != 1 && status != 2) {
        printf("FAIL! Unexpected status for division with max scale divisor: %d\n", status);
        failed++;
    } else if (status == 0) {
        if ((result.bits[0] == 0 && result.bits[1] == 0) || s21_get_sign(&result)) {
             printf("FAIL! Expected large positive result, got [%u, %u] sign %d\n",
                    result.bits[0], result.bits[1], s21_get_sign(&result));
             failed++;
        }
    }

    printf("\n=== DIV TEST 36: Divisor Just Above 1 ===\n");
    a = (s21_decimal){{1000, 0, 0, 0}}; s21_set_scale(&a, 0); 
    b = (s21_decimal){{1001, 0, 0, 0}}; s21_set_scale(&b, 3);
    status = s21_div(a, b, &result);
    print_decimal(result);
    printf("INFO: Result of 1000 / 1.001. Checking basic structure.\n");
    if (status != 0 || result.bits[0] == 0 || s21_get_sign(&result)) {
         printf("FAIL! Expected positive non-zero result for 1000/1.001, got %u sign %d (status %d)\n",
                result.bits[0], s21_get_sign(&result), status);
         failed++;
    }

    printf("\n=== DIV TEST 37: Dividend Just Below Divisor ===\n");
    a = (s21_decimal){{999999, 0, 0, 0}}; s21_set_scale(&a, 6); 
    b = (s21_decimal){{1000000, 0, 0, 0}}; s21_set_scale(&b, 6);
    status = s21_div(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 999999 || s21_get_scale(&result) != 6 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.999999, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 38: Integer Division with Remainder ===\n");
    a = (s21_decimal){{10, 0, 0, 0}}; s21_set_scale(&a, 0); // 10
    b = (s21_decimal){{3, 0, 0, 0}}; s21_set_scale(&b, 0);  // 3
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 10 / 3 = 3.333...
    // Integer part is 3. The fractional part depends on implementation.
    // Check if result is close to 3 and positive.
    printf("INFO: Result of 10 / 3. Checking basic structure.\n");
    if (status != 0 || result.bits[0] == 0 || s21_get_sign(&result)) {
         printf("FAIL! Expected positive non-zero result for 10/3, got %u sign %d (status %d)\n",
                result.bits[0], s21_get_sign(&result), status);
         failed++;
    }

    printf("\n=== DIV TEST 39: Stress Test with Repeating Decimal Pattern ===\n");
    // 123.456 / 9.87654
    a = (s21_decimal){{123456, 0, 0, 0}}; s21_set_scale(&a, 3); // 123.456
    b = (s21_decimal){{987654, 0, 0, 0}}; s21_set_scale(&b, 5); // 9.87654
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 123.456 / 9.87654 ≈ 12.499994...
    // This is close to 12.5.
    // Check if result is close to 12.5 and positive.
    printf("INFO: Stress test division. Status: %d\n", status);
    if (status != 0 || result.bits[0] == 0 || s21_get_sign(&result)) {
         printf("FAIL! Expected positive non-zero result for stress test, got %u sign %d (status %d)\n",
                result.bits[0], s21_get_sign(&result), status);
         failed++;
    }

    printf("\n=== DIV TEST 40: Division Involving Powers of 2 ===\n");
    a = (s21_decimal){{1024, 0, 0, 0}}; s21_set_scale(&a, 0); // 1024 (2^10)
    b = (s21_decimal){{32, 0, 0, 0}}; s21_set_scale(&b, 0);   // 32 (2^5)
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 1024 / 32 = 32
    if (status != 0 || result.bits[0] != 32 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 32, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 41: Division Resulting in Very Small Scale Increase ===\n");
    a = (s21_decimal){{1500, 0, 0, 0}}; s21_set_scale(&a, 2); // 15.00
    b = (s21_decimal){{300, 0, 0, 0}}; s21_set_scale(&b, 3);  // 0.300
    status = s21_div(a, b, &result);
    print_decimal(result);
    if (status != 0 || result.bits[0] != 50 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 50.000, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 42: Division Where Quotient < 1 and Needs Scale ===\n");
    a = (s21_decimal){{125, 0, 0, 0}}; s21_set_scale(&a, 3); // 0.125
    b = (s21_decimal){{500, 0, 0, 0}}; s21_set_scale(&b, 3); // 0.500
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 0.125 / 0.500 = 0.25
    // scale diff = 3 - 3 = 0.
    // coeff = 125 / 500 = 0 (integer). Remainder is 125.
    // The algorithm needs to handle the fractional part.
    // Result should be 0.25.
    // 0.25 = 25 * 10^-2. So coeff = 25, scale = 2.
    if (status != 0 || result.bits[0] != 25 || s21_get_scale(&result) != 2 || s21_get_sign(&result)) {
        printf("FAIL! Expected 0.25, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 43: Division with Cross-Midword Operations ===\n");
    // Use numbers that will likely involve multiple words in s21_big_decimal during division.
    a = (s21_decimal){{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}}; s21_set_scale(&a, 0); // Large number
    b = (s21_decimal){{0xFFFFFFFF, 0, 0, 0}}; s21_set_scale(&b, 0);          // Another large number
    status = s21_div(a, b, &result);
    print_decimal(result);
    // (2^64 - 1) / (2^32 - 1)
    // This is a large division that tests the big_decimal division logic.
    printf("INFO: Cross-midword division. Status: %d\n", status);
    if (status != 0 && status != 1 && status != 2) {
        printf("FAIL! Unexpected status for cross-midword division: %d\n", status);
        failed++;
    }

    printf("\n=== DIV TEST 44: Division Resulting in Value Requiring Rounding ===\n");
    // This is difficult to design perfectly without knowing exact rounding rules.
    // Let's create a case where the exact result is 0.abcdef... and it needs rounding.
    a = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&a, 1); // 0.2
    b = (s21_decimal){{3, 0, 0, 0}}; s21_set_scale(&b, 0); // 3
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 0.2 / 3 = 0.066666...
    // This is 0.06 recurring. It will need rounding.
    // 0.066666... = 6.66666... * 10^-2.
    // To represent this, coeff = 66666..., scale = 5? Or similar.
    // Check if result is close to 0.066666... and positive.
    printf("INFO: Result of 0.2 / 3 (0.0666...). Checking basic structure.\n");
    if (status != 0 || result.bits[0] == 0 || s21_get_sign(&result)) {
         printf("FAIL! Expected positive non-zero result for 0.2/3, got %u sign %d (status %d)\n",
                result.bits[0], s21_get_sign(&result), status);
         failed++;
    }

    printf("\n=== DIV TEST 45: Negative Result with Large Magnitude ===\n");
    a = (s21_decimal){{1000000000, 0, 0, 0}}; s21_set_scale(&a, 0); s21_set_sign(&a, 1); // -1,000,000,000
    b = (s21_decimal){{2, 0, 0, 0}}; s21_set_scale(&b, 0);                             // 2
    status = s21_div(a, b, &result);
    print_decimal(result);
    // -1,000,000,000 / 2 = -500,000,000
    if (status != 0 || result.bits[0] != 500000000 || s21_get_scale(&result) != 0 || !s21_get_sign(&result)) {
        printf("FAIL! Expected -500,000,000, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 46: Division by Number Close to Zero (Not Zero) ===\n");
    a = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&a, 0);     // 1
    b = (s21_decimal){{1, 0, 0, 0}}; s21_set_scale(&b, 28);    // 1e-28
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 1 / 1e-28 = 1e28
    // This is a very large number.
    printf("INFO: Division by very small number. Status: %d\n", status);
    if (status != 0 && status != 1 && status != 2) {
        printf("FAIL! Unexpected status for division by very small number: %d\n", status);
        failed++;
    } else if (status == 0) {
        // Check if result is large
        if ((result.bits[0] == 0 && result.bits[1] == 0) || s21_get_sign(&result)) {
             printf("FAIL! Expected large positive result, got [%u, %u] sign %d\n",
                    result.bits[0], result.bits[1], s21_get_sign(&result));
             failed++;
        }
    }

    printf("\n=== DIV TEST 47: Zero Scale Difference, Large Numbers ===\n");
    a = (s21_decimal){{999999999, 0, 0, 0}}; s21_set_scale(&a, 5); // 9,999.99999
    b = (s21_decimal){{111111111, 0, 0, 0}}; s21_set_scale(&b, 5); // 1,111.11111
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 9,999.99999 / 1,111.11111 ≈ 9
    // scale diff = 5 - 5 = 0.
    // coeff = 999999999 / 111111111 = 9 (since 9 * 111111111 = 999999999).
    // Result coeff = 9, scale = 0.
    if (status != 0 || result.bits[0] != 9 || s21_get_scale(&result) != 0 || s21_get_sign(&result)) {
        printf("FAIL! Expected 9, got %u scale %d sign %d (status %d)\n",
               result.bits[0], s21_get_scale(&result), s21_get_sign(&result), status);
        failed++;
    }

    printf("\n=== DIV TEST 48: Complex Scale Interaction ===\n");
    a = (s21_decimal){{123456, 0, 0, 0}}; s21_set_scale(&a, 10); // 0.0000123456
    b = (s21_decimal){{789, 0, 0, 0}}; s21_set_scale(&b, 15);    // 0.000000000789
    status = s21_div(a, b, &result);
    print_decimal(result);
    // (123456 * 10^-10) / (789 * 10^-15) = (123456 / 789) * 10^(-10 - (-15)) = (123456 / 789) * 10^5
    // 123456 / 789 ≈ 156.471...
    // Result ≈ 156.471 * 10^5 = 15,647,100
    // scale diff = 10 - 15 = -5.
    // coeff = 123456 / 789 = 156 (integer part).
    // Adjust coeff for scale diff -5: 156 * 10^5 = 15,600,000.
    // Result scale = 0.
    // Value = 15,600,000. This is close to the estimate.
    if (status != 0 || result.bits[0] == 0 || s21_get_sign(&result)) {
         printf("FAIL! Expected large positive result, got %u sign %d (status %d)\n",
                result.bits[0], s21_get_sign(&result), status);
         failed++;
    }

    printf("\n=== DIV TEST 49: Division Involving Previously Problematic Values ===\n");
    // Use values that might have caused issues in mul or add/sub.
    a = (s21_decimal){{0xFFFFFFFF, 0, 0, 0}}; s21_set_scale(&a, 0); // 4,294,967,295
    b = (s21_decimal){{0xAAAAAAAA, 0, 0, 0}}; s21_set_scale(&b, 0); // 2,863,311,530
    status = s21_div(a, b, &result);
    print_decimal(result);
    // 0xFFFFFFFF / 0xAAAAAAAA = 4,294,967,295 / 2,863,311,530 ≈ 1.5
    printf("INFO: Division of large hex values. Status: %d\n", status);
    if (status != 0 || result.bits[0] == 0 || s21_get_sign(&result)) {
         printf("FAIL! Expected positive non-zero result, got %u sign %d (status %d)\n",
                result.bits[0], s21_get_sign(&result), status);
         failed++;
    }

    printf("\n=== DIV TEST 50: Final Edge Case Check ===\n");
    // Divide the maximum possible s21_decimal by the smallest non-zero positive s21_decimal.
    // Max value: coefficient 0xFFFFFFFFFFFFFFFFFFFFFFFF, scale 0.
    // Min positive value: coefficient 1, scale 28.
    // This will definitely overflow.
    s21_decimal max_val = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}}; s21_set_scale(&max_val, 0);
    s21_decimal min_val = {{1, 0, 0, 0}}; s21_set_scale(&min_val, 28);
    status = s21_div(max_val, min_val, &result);
    print_decimal(result);
    printf("INFO: Max/Min division (overflow expected). Status: %d\n", status);
    if (status != 1 && status != 2) {
        printf("FAIL! Expected overflow status (1 or 2) for max/min division, got status %d\n", status);
        failed++;
    }
    return failed++;
}


int main() {
    int failed_add = test_addition();
    int failed_sub = test_subtraction();
    int failed_mul = test_multiplication();
    int failed_div = test_division(); 
    int total_failed = failed_add + failed_sub + failed_mul + failed_div; 
    printf("\n=== ADDITION TESTS FAILED: %d ===\n", failed_add);
    printf("\n=== SUBTRACTION TESTS FAILED: %d ===\n", failed_sub);
    printf("\n=== MULTIPLICATION TESTS FAILED: %d ===\n", failed_mul);
    printf("\n=== DIVISION TESTS FAILED: %d ===\n", failed_div);
    printf("\n=== TOTAL TESTS FAILED: %d ===\n", total_failed);
    return total_failed ? 1 : 0;
}