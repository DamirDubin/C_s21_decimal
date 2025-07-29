

#include "s21_decimal.h"
#include <stdio.h>

void s21_shift_left(s21_big_decimal* num, int shift_value) {
    
     while (shift_value >= 32) {
        for (int i = 6; i > 0; --i) {
            num->bits[i] = num->bits[i - 1];
        }
        num->bits[0] = 0;
        shift_value -= 32;
    }
    
    if (shift_value > 0){
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
     return (value->bits[bit_number / 32] >> (bit_number % 32)) & 1;
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

void s21_from_decimal_to_big(s21_decimal az, s21_big_decimal* big) {
    for (int i = 0; i < 3; i++) {
        big->bits[i] = az.bits[i]; 
    }
    for (int i = 3; i < 7; i++) {
        big->bits[i] = 0;
    }
}

int s21_from_big_to_decimal(s21_big_decimal big, s21_decimal* dec){
    if (big.bits[3] || big.bits[4] || big.bits[5] || big.bits[6]) {     
        return 1;
    }

    for (int i = 0; i < 3; i++) {
        dec->bits[i] = big.bits[i];
    }
    return 0;
}

    void s21_mul_ten(s21_big_decimal* t){
    s21_big_decimal temp = *t;
    s21_shift_left(t, 3);
    s21_shift_left(&temp, 1); 
     s21_bitwise_addition(t, &temp, t); 
}

int s21_find_oldest_positive_bit(s21_big_decimal* a){
    int number = 0;
 for (int i = 32*7 - 1; i >= 0; i--){
    if (s21_big_get_bit(a, i)){
       number = i;
       return number;
    }
 }
  return 0;
}


void s21_div_big_10(s21_big_decimal* a, s21_big_decimal* result) {
    *result = (s21_big_decimal){{0}};
    s21_big_decimal ten = {{0b1010, 0, 0, 0, 0, 0, 0}}; 
    s21_big_decimal a_copy = *a;
    s21_big_decimal one = {{1, 0, 0, 0, 0, 0, 0}};
    s21_big_decimal temp = {{0}};

    int bits_number = s21_find_oldest_positive_bit(&a_copy);

    while (bits_number >= 0) {
        s21_shift_left(&temp, 1);
        if (s21_big_get_bit(&a_copy, bits_number)){
            s21_bitwise_addition(&temp, &one, &temp);
        }

        if (s21_big_is_greater_or_equal(temp, ten)) {
            s21_shift_left(result, 1);
            s21_bitwise_addition(result, &one, result);
            s21_big_subtraction(&temp, &ten, &temp);
        } else {
            s21_shift_left(result, 1);
        }
        bits_number--;
    }
}


void s21_normalize_big_decimals(s21_big_decimal* a, s21_big_decimal* b, int* scale_a, int* scale_b) {
    while (*scale_a < *scale_b) {
        s21_mul_ten(a);
        (*scale_a)++;
    }

    while (*scale_b < *scale_a) {;
        s21_mul_ten(b);
        (*scale_b)++;
    }
}


int s21_is_out_of_96_bits(s21_big_decimal big) {
    return big.bits[3] || big.bits[4] || big.bits[5] || big.bits[6];
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

int last_digit(s21_big_decimal* big, int z){
    int x = 1; int a = 0; 

    for(int i = 0; i < 32 * 7; i++){
    if ((big->bits[i/32] >> i % 32) & 1){
        a = (a + x) % z;
    }
    x = (x * 2) % z;
    }
    return a;
}

int s21_reduce_and_round(s21_big_decimal* big, int* scale) {
    int error = 0;

    while (s21_is_out_of_96_bits(*big) && *scale > 0) {
        s21_big_decimal temp = *big;
        s21_big_decimal result = {{0}};

        int last_digit_before_division = last_digit(&temp, 10);

        s21_div_big_10(&temp, &result);
        
        (*scale)--;

        if (last_digit_before_division > 5 || (last_digit_before_division == 5 && last_digit_before_division % 2 != 0)) {
            s21_big_decimal one = {{1, 0, 0, 0, 0, 0, 0}};
             s21_bitwise_addition(&result, &one, &result);
            }
            *big = result;
        }
    
    if (s21_is_out_of_96_bits(*big)) {
        error = 1;
    }
    return error;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    *result = (s21_decimal){{0}};

    s21_big_decimal a, b, res = {0};
    s21_from_decimal_to_big(value_1, &a);
    s21_from_decimal_to_big(value_2, &b);

    int scale_a = s21_get_scale(&value_1);
    int scale_b = s21_get_scale(&value_2);
    int sign_a = s21_get_sign(&value_1);
    int sign_b = s21_get_sign(&value_2);
    int result_sign = sign_a;   

    s21_normalize_big_decimals(&a, &b, &scale_a, &scale_b);
       

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

    if (s21_reduce_and_round(&res, &scale_a)) {
        return 1;
    }

    if (s21_from_big_to_decimal(res, result)) {
        return 1;
    }

    s21_set_scale(result, scale_a);
    s21_set_sign(result, result_sign);

    return S21_OK;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result){
    s21_decimal reverse_value_2 = value_2;
    s21_set_sign(&reverse_value_2, !s21_get_sign(&value_2)); 
    
    return s21_add(value_1, reverse_value_2, result);
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result){

    *result = (s21_decimal){{0}};
    s21_big_decimal a, b, res = {0};
    s21_from_decimal_to_big(value_1, &a);
    s21_from_decimal_to_big(value_2, &b);
    int scale_a = s21_get_scale(&value_1);
    int scale_b = s21_get_scale(&value_2);
    int sign_a = s21_get_sign(&value_1);
    int sign_b = s21_get_sign(&value_2);
    int result_sign = (sign_a != sign_b) ? 1 : 0;
    int result_scale = scale_a + scale_b;
    s21_big_decimal temp = {0};

    for (int i = 0; i < 96; i++) {
        if (s21_big_get_bit(&b, i)) {
            s21_big_decimal shifted_a = a;
            s21_shift_left(&shifted_a, i);
            s21_bitwise_addition(&temp, &shifted_a, &temp);
        }
    }

    while (result_scale > 28) {
        int last_digit_before_division = last_digit(&temp, 10);
        s21_big_decimal divided = {0};
        s21_div_big_10(&temp, &divided);
    
        if (last_digit_before_division > 5 || 
            (last_digit_before_division == 5 && (s21_big_get_bit(&divided, 0) & 1))) {
            s21_big_decimal one = {{1, 0, 0, 0, 0, 0, 0}};
            s21_bitwise_addition(&divided, &one, &divided);
        }  
        temp = divided;
        result_scale--;
    }

     if (s21_is_out_of_96_bits(temp)) {
        if (s21_reduce_and_round(&temp, &result_scale)) {
            return (result_sign) ? 2 : 1; 
        }
    }

    if (s21_from_big_to_decimal(temp, result)) {
        return (result_sign) ? 2 : 1;
    }

    s21_set_scale(result, result_scale);
    s21_set_sign(result, result_sign);
    return S21_OK;
}


 void s21_big_div(s21_big_decimal a, s21_big_decimal b, s21_big_decimal* result, s21_big_decimal* temp){
    s21_big_decimal one = {{1, 0, 0, 0, 0, 0, 0}};
    int bits_number = s21_find_oldest_positive_bit(&a);

    while (bits_number >= 0) {
        s21_shift_left(temp, 1);
        if (s21_big_get_bit(&a, bits_number)){
            s21_bitwise_addition(temp, &one, temp);
        }

        if (s21_big_is_greater_or_equal(*temp, b)) {
            s21_shift_left(result, 1);
            s21_bitwise_addition(result, &one, result);
            s21_big_subtraction(temp, &b, temp);
        } else {
            s21_shift_left(result, 1);
        }
        bits_number--;
    }
 }

int s21_big_is_zero(s21_big_decimal* val) {
    for (int i = 0; i < 7; i++) {
        if (val->bits[i] != 0) {
            return 0;
        }
    }
    return 1;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result){
    if (value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0){
        return 3;
    }
     
    *result = (s21_decimal){{0}};
    s21_big_decimal a, b, res = {0};
    s21_from_decimal_to_big(value_1, &a);
    s21_from_decimal_to_big(value_2, &b);
    int scale_a = s21_get_scale(&value_1);
    int scale_b = s21_get_scale(&value_2);
    int sign_a = s21_get_sign(&value_1);
    int sign_b = s21_get_sign(&value_2);
    int result_sign = (sign_a != sign_b) ? 1 : 0;
    int result_scale = scale_a - scale_b;
    
    s21_big_decimal remainder = {0};
    s21_big_div(a, b, &res, &remainder);

    int added_precision = 0;
    const int max_added_precision = 28;
    
    while (!s21_big_is_zero(&remainder) && added_precision < max_added_precision && result_scale < 28) {
        s21_mul_ten(&remainder); 
        added_precision++;
        result_scale++; 
        s21_big_decimal new_quotient_digit = {{0}};
        s21_big_decimal new_remainder = {{0}};
        s21_big_div(remainder, b, &new_quotient_digit, &new_remainder);
        s21_mul_ten(&res);
        s21_bitwise_addition(&res, &new_quotient_digit, &res);
        remainder = new_remainder;
    }

    if (result_scale < 0) {
        int scale_reverse = -result_scale;
        for (int i = 0; i < scale_reverse && i < 28; i++) {
             s21_mul_ten(&res);
        }
        result_scale = 0; 
    } 
  
    if (s21_is_out_of_96_bits(res)) {
        if (s21_reduce_and_round(&res, &result_scale)) {
            return (result_sign) ? 2 : 1; 
        }
    }

    if (s21_from_big_to_decimal(res, result)) {
        return (result_sign) ? 2 : 1;
    }

    s21_set_scale(result, result_scale);
    s21_set_sign(result, result_sign);

    return 0;
}

///////////////////////////////////////////////////////

int s21_is_less(s21_decimal a, s21_decimal b){
    int result = 0;

    s21_big_decimal c = {0};
    s21_big_decimal d = {0};
    s21_from_decimal_to_big(a, &c);
    s21_from_decimal_to_big(b, &d);

    s21_normalize_big_decimals(&c, &d, s21_get_scale(&a), s21_get_scale(&b)); 
    
    int sign_a = s21_get_sign(&a); 
    int sign_b = s21_get_sign(&b); 
    int abs_cmp = s21_abs_compare(c, d);

    if (sign_a != sign_b) {
        result = sign_a;  
        }
    else if (sign_a == 1) {
        result = (abs_cmp == 1);
    }
    else {
        result = (abs_cmp == -1);
    }
    return result;
}

int s21_abs_compare(s21_big_decimal a, s21_big_decimal b) {
    int result = 0; 
    
    for (int i = 6; i >= 0 && result == 0; i--) {
        if (a.bits[i] > b.bits[i]) {
            result = 1;
        } else if (a.bits[i] < b.bits[i]) {
            result = -1;
        }
    }
    return result;
}

int s21_is_equal(s21_decimal a, s21_decimal b){
    int result = 1;

    s21_big_decimal c = {0};
    s21_big_decimal d = {0};
    s21_from_decimal_to_big(a, &c);
    s21_from_decimal_to_big(b, &d);

    s21_normalize_decimals(&c, &d, s21_get_scale(&a), s21_get_scale(&b));

    int sign_a = s21_get_sign(&a); 
    int sign_b = s21_get_sign(&b); 

if(sign_a != sign_b){
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

// int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
//     return s21_is_less(a, b) || s21_is_equal(a, b);
// }

// int s21_is_greater(s21_decimal a, s21_decimal b) {
//     return !s21_is_less_or_equal(a, b);
// }

// int s21_is_not_equal(s21_decimal a, s21_decimal b) {
//     return !s21_is_equal(a, b);
// }

// int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
//     return !s21_is_less(a, b);
// }








// // Округляет указанное Decimal число до ближайшего целого числа в сторону
// // отрицательной бесконечности.
// int s21_floor(s21_decimal value, s21_decimal *result) {
//   int status_result = S21_OK;

//   if (result == S21_NULL) {
//     return ERROR;
//   }

//   //s21_init_decimal(result);

//   if (s21_truncate(value, result) == S21_OK) {
//     if (s21_get_sign(&value) && !s21_is_equal(value, *result)) {
//       s21_decimal one = {{1, 0, 0, 0}};
//       s21_set_sign(&one, 1);
//       if (s21_add(*result, one, result) != S21_OK)
//         status_result = ERROR;
//     }

//   } else {
//     status_result = ERROR;
//   }

//   return status_result;
// }

// // Округляет Decimal до ближайшего целого числа.
// int s21_round(s21_decimal value, s21_decimal *result) {
//   if (result == S21_NULL) {
//     return ERROR;
//   }

//   //s21_init_decimal(result);

//   if (s21_is_zero(value)) return S21_OK;

//   int sign = s21_get_sign(&value);
//   s21_set_sign(&value, 0);

//   s21_decimal truncated;
//   s21_truncate(value, &truncated);  // целая часть

//   s21_decimal fractional;
//   // s21_sub(value, truncated, &fractional);  // дробная часть

//   s21_decimal negate_truncated;
//   s21_negate(truncated, &negate_truncated);  // negate_truncated = -truncated
//   s21_add(value, negate_truncated, &fractional);  // value - truncated

//   s21_decimal half = {{5, 0, 0, 0}};  // проверка >= 0.5
//   s21_set_exp(&half, 1);

//   *result = truncated;

//   // if (s21_is_greater(fractional, half) ||
//   //     (s21_is_equal(fractional, half) && (truncated.bits[0] & 1))) {
//   //   s21_decimal one = {{1, 0, 0, 0}};
//   //   s21_add(*result, one, result);  // округление вверх
//   // }
//   s21_decimal max_dec = {
//     {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000}  // 2^96 - 1, scale = 0
// };
//   if ((s21_is_greater_or_equal(fractional, half)) && (!s21_is_equal(truncated, max_dec))) {
//     s21_decimal one = {{1, 0, 0, 0}};
//     s21_add(*result, one, result);  // Округляем вверх
//   }

//   s21_set_sign(result, sign);
//   return S21_OK;
// }
// // Возвращает целые цифры указанного Decimal числа; любые дробные цифры
// // отбрасываются, включая конечные нули.
// int s21_truncate(s21_decimal value, s21_decimal *result) {
//   if (result == S21_NULL) {
//     return ERROR;
//   }

//   int status_result = S21_OK;

//   // Валидация экспоненты
//   int exp = s21_get_exp(&value);
//   if (exp > 28 || exp < 0) {
//     status_result = ERROR;
//   }

//   if (status_result == S21_OK) {
//     while (exp--> 0) {
//       s21_div_by_10(&value);  // деление на 10^эксп
//     }

//     *result = value;
//     s21_set_exp(result, 0);  // сброс экспо
//   }

//   return status_result;
// }

// // Возвращает результат умножения указанного Decimal на -1.
// int s21_negate(s21_decimal value, s21_decimal *result) {
//   // Проверка указателя
//   if (result == S21_NULL) {
//     return ERROR;
//   }

//   *result = value;

//   s21_set_sign(result, !s21_get_sign(&value));
//   return S21_OK;
// }


