#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *s_hello_world_buf = "Hello world!\n";

// Invoke sys_write system call using x86-32 interface.
static void s_x86_32_print_string(const char *buf, size_t buf_len) {
  asm("mov rax,4;" // Code for sys_write system call
      "mov rbx,1;" // File descriptor = stdout
      "mov rcx,%0;" // Pointer to data
      "mov rdx,%1;" // Data length
      "int 0x80" : : "r" (buf), "r" (buf_len) : "rax", "rbx", "rcx", "rdx");
}

// Invoke sys_write system call using x86-64 interface.
static void s_x86_64_print_string(const char *buf, size_t buf_len) {
  asm("mov rax,1;" // Code for sys_write system call
      "mov rdi,1;" // File descriptor = stdout
      "mov rsi,%0;" // Pointer to data
      "mov rdx,%1;" // Data length
      "syscall" : : "r" (buf), "r" (buf_len) : "rax", "rdi", "rsi", "rdx");
}

static void s_print_string(const char *buf, size_t buf_len) {
  printf("32-bit\n");
  s_x86_32_print_string(buf, buf_len);
  printf("64-bit\n");
  s_x86_64_print_string(buf, buf_len);
}

int main() {
  // Add 1 to buffer length to accomodate for null terminator.
  const size_t buf_len = strlen(s_hello_world_buf) + 1;
  char *heap_buf = (char *)malloc(buf_len);
  strcpy(heap_buf, s_hello_world_buf);

  printf("Printing heap allocated string\n");
  s_print_string(heap_buf, buf_len);

  printf("Printing data allocated string\n");
  s_print_string(s_hello_world_buf, buf_len);

  /* Interesting point: If you compile this as 64-bit program, the 32-bit
     system call will fail when passed stack memory. This is because the 
     stack memory is at the top of the memory and outside of 32-bit memory,
     so it's treated as an invalid address. */
  printf("Printing stack allocated string\n");
  char stack_buf[buf_len];
  strcpy(stack_buf, s_hello_world_buf);
  s_print_string(stack_buf, buf_len);
}
