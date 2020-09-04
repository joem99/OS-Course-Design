/*
 * OS Course Design - Exp 2
 * 
 * Syscall - test myhello
 * 
 * Created by myw on 2020.02.20
 * 
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(void) {
  printf("Test1: Test myhello\n");
  syscall(333);
  return 0;
}