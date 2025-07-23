#include <limits.h>
#include <math.h>
#include <stdio.h>

void itob(int x, char *buf) {
  unsigned char *ptr = (unsigned char *)&x;
  int pos = 0;
  for (int i = sizeof(int) - 1; i >= 0; i--)
    for (int j = CHAR_BIT - 1; j >= 0; j--)
      buf[pos++] = '0' + !!(ptr[i] & 1U << j);
  buf[pos] = '\0';
}

int main() {
  unsigned int a = pow(2, 32) - 1;
  char *res;
  itob(a, res);
  printf("%s", res);
}
