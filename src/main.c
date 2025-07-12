#include <stdio.h>
#include "s21_decimal.h"
#include <math.h>

void print_decimal_bits(s21_decimal dec) {
    for (int i = 0; i < 4; i++) {
        printf("bits[%d]: ", i);
        for (int j = 31; j >= 0; j--) {
            printf("%d", (dec.bits[i] >> j) & 1);
        }
        printf("\n");
    }
}

int main() {

    s21_decimal a = {{0}};
    s21_decimal b = {{0}};
    s21_decimal result = {{0}};

    a.bits[0] = 4000000000;    
    a.bits[1] = 4000000000;   
    a.bits[2] = 4000000000;     
    s21_set_scale(&a, 0);        
    s21_set_sign(&a, 0);       
    
     b.bits[0] = 4000000000;
     b.bits[1] = 4000000000;  
     b.bits[2] = 4000000000;                  
    s21_set_scale(&b, 0);        
    s21_set_sign(&b, 0);      

   
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
        
        int scale = s21_get_scale(&result);
        int sign = s21_get_sign(&result);
        double value = result.bits[0] / pow(10, scale);
         if (sign) {
        value = -value;
    }
        printf("Value : %.2f\n", value);
    } else {
        printf("ERROR");
    }
    
    return 0;
}
