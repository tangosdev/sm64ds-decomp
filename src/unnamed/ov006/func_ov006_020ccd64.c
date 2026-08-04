struct S { int w[1]; };
extern struct S data_ov006_021405c8;
void func_ov006_020ccd64(char *p) { *(struct S *)(p + 0x20) = data_ov006_021405c8; }
