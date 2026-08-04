struct S { int w[2]; };
extern struct S data_ov006_0213b020;
void func_ov006_020c802c(char *p) { *(struct S *)(p + 0x3c) = data_ov006_0213b020; }
