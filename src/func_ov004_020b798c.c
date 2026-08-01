struct S { int w[2]; };
extern struct S data_ov004_020bc9b4;
void func_ov004_020b798c(char *p) { *(int *)(p + 0x1c) = 0; *(struct S *)(p + 0x8) = data_ov004_020bc9b4; }
