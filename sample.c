#include <stdio.h>
#include <stdlib.h>

long
to_the (long x, long y) {
  long r = 1;
  while (y > 0) {
    r = r * x;
    y = y - 1;
  }
  return r;
}

int
main (int argc, char** argv) {

  if (argc != 3) {
    fprintf(stderr, "USAGE: %s <x (int)> <y (int)>\n", argv[0]);
    return 1;
  }
  
  long x = strtoul(argv[1], NULL, 10);
  long y = strtoul(argv[2], NULL, 10);
  long z = to_the(x, y);
  printf("%d\n", z);

  return 0;

}
