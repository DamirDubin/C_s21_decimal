#include "s21_decimal.h"

void s21_shift_left(s21_big_decimal* num, int shift_value) {
    if (shift_value >= 0 && shift_value < 32){
    unsigned memory = 0;
    for (int i = 0; i < 7; ++i) {
        unsigned temp = num->bits[i];
        num->bits[i] <<= shift_value;
        num->bits[i] |= memory;
        memory = temp >> (32 - shift_value);    
     }
  }
}

void s21_bitwise_addition(s21_big_decimal* value_1, s21_big_decimal* value_2, s21_big_decimal* result){
    unsigned int memo = 0;
    for (int i = 0; i < 32 * 7; i++){
        unsigned int result_bit = s21_big_get_bit(value_1, i) + s21_big_get_bit(value_2, i) + memo;
        memo = result_bit / 2;
        result_bit %= 2;
        
        s21_big_set_bit(result, i, result_bit);
    }
}

void s21_big_set_bit(s21_big_decimal* value, int bit_number, int bit_value){
    int int_index = bit_number / 32;
    int bit_index = bit_number % 32;

    if (int_index >= 0 && int_index < 7){
    if (bit_value == 1) {
        value->bits[int_index] |= (1 << bit_index);
    } else {
        value->bits[int_index] &= ~(1 << bit_index); 
    }
  }
}

int s21_big_get_bit(s21_big_decimal* value, int bit_number){
    int az = BIT_ERROR;
        if (bit_number >= 0 && bit_number < 32 * 7) {
            az = (value->bits[bit_number / 32] >> (bit_number % 32)) & 1;
    }
    return az;
}

void s21_set_sign(s21_decimal* az, int sign_value){
    if (sign_value == 0 || sign_value == 1) {
    if (sign_value) {
        az->bits[3] |= 1 << 31;
    } else {
        az->bits[3] &= ~(1 << 31); 
    }
  }
}

int s21_get_sign(s21_decimal* az){
    return (az->bits[3] >> 31) & 1;
}

void s21_set_scale(s21_decimal* az, int scale_value){
if (scale_value >= 0 && scale_value <= 28) {
 int a = scale_value << 16;
    int b = az->bits[3] & (1 << 31);
        az->bits[3] = a | b;
}
}

int s21_get_scale(s21_decimal* az){
    return (az->bits[3] >> 16) & 0b00000000000000000000000011111111;
}

int s21_mul_10(s21_decimal* value) {
    int az = 0;
    s21_big_decimal big;
    s21_from_decimal_to_big(*value, &big);

    s21_mul_ten(&big);
 
    if (s21_from_big_to_decimal(big, value) == 1){
        az = 1;
    }
    return az;
}

void s21_from_decimal_to_big(s21_decimal az, s21_big_decimal* big) {
    for (int i = 0; i < 3; i++) {
        big->bits[i] = (unsigned int)az.bits[i]; // оставить ли unsigned???
    }
    for (int i = 3; i < 7; i++) {
        big->bits[i] = 0;
    }
}

int s21_from_big_to_decimal(s21_big_decimal big, s21_decimal* dec){
    int az = 0;
    if (big.bits[3] == 0 && big.bits[4] == 0 && big.bits[5] == 0 && big.bits[6] == 0){
    for (int i = 0; i < 3; i++) {
        dec->bits[i] = big.bits[i];
    }
  } else {
    az = 1;
  }   
  return az;
}

    void s21_mul_ten(s21_big_decimal* t){
    s21_big_decimal temp = *t;

    s21_shift_left(t, 3);
    s21_shift_left(&temp, 1); 

     s21_bitwise_addition(t, &temp, t); 
}

// верно ли я подумал про нормализацию?
int s21_normalize_decimals(s21_decimal* a, s21_decimal* b) {
    
    int scale_a = s21_get_scale(a);
    int scale_b = s21_get_scale(b);

    while (scale_a < scale_b) {
        int error = s21_mul_10(a);
        if (error != 0) break;
        scale_a++;
    }

    while (scale_b < scale_a) {
        int error = s21_mul_10(b);
        if (error != 0) break;
        scale_b++;
    }

    s21_set_scale(a, scale_a);
    s21_set_scale(b, scale_b);

    if (scale_a != scale_b) {
        return 1;
    }
    return 0;
}

int s21_is_out_of_96_bits(s21_big_decimal d) {
    int az = 0;
    for (int i = 3; i < 7; i++) {
        if (d.bits[i] != 0) 
            az = 1; 
    }
    return az; 
}

void s21_big_subtraction(s21_big_decimal* a, s21_big_decimal* b, s21_big_decimal* res) {
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

int s21_reduce_and_round(s21_big_decimal* big, int* scale) {
    int error = 0;

    while (s21_is_out_of_96_bits(*big) && *scale < 28) {
        int az = 0;

        for (int i = 6; i >= 0; i--) {
            unsigned long long temp = ((unsigned long long)az << 32) | big->bits[i];
            big->bits[i] = (unsigned int)(temp / 10);
            az = (int)(temp % 10);
        }

        (*scale)++;

        // Округление
        if (az >= 5) {
            s21_big_decimal one = {{1, 0, 0, 0, 0, 0, 0}};
            s21_bitwise_addition(big, &one, big);

            if (s21_is_out_of_96_bits(*big)) {
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
  
    int error = s21_normalize_decimals(&value_1, &value_2);
    if (error != 0) return 1;

    int scale = s21_get_scale(&value_1); 
    int sign_a = s21_get_sign(&value_1);
    int sign_b = s21_get_sign(&value_2);

    s21_big_decimal a, b, res;
    s21_from_decimal_to_big(value_1, &a);
    s21_from_decimal_to_big(value_2, &b);

    if (sign_a == sign_b) {
        s21_bitwise_addition(&a, &b, &res);
    } else {
        if (s21_big_is_greater_or_equal(a, b)) {
            s21_big_subtraction(&a, &b, &res);
        } else {
            s21_big_subtraction(&b, &a, &res);
            sign_a = sign_b; 
        }
    }

    if (!error && !s21_is_out_of_96_bits(res)) {
        error = s21_reduce_and_round(&res, &scale);
    }

    if (!error) {
        if (s21_from_big_to_decimal(res, result)) {
            error = 1;
        } else {
            s21_set_scale(result, scale);
            s21_set_sign(result, sign_a);
        }
    }

    return error;
}
