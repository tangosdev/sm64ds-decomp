struct S { int w[2]; };
extern struct S data_ov004_020bc8bc;
void func_ov004_020b853c(char *p) { *(int *)(p + 0x1c) = 15; *(struct S *)(p + 0x8) = data_ov004_020bc8bc; }
