/*
 * OS Course Design - Exp 2
 * 
 * Syscall - test mycopy
 * 
 * Created by myw on 2020.02.20
 * 
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv) {
  printf("Test2: Test mycopy\n");
  if (argc != 3) {
    printf("Usage ./testcp <source_file> <target_file>\n");
    return 0;
  }
  printf("Copy: %s -> %s\n", argv[1], argv[2]);
  long ret = syscall(334, argv[1], argv[2]);
  printf("ret: %ld\n", ret);
  printf("errno: %d\n", errno);
  return 0;
}