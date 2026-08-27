struct S { int w[2]; };
extern struct S data_ov006_0213af90;
void func_ov006_020c4d20(char *p) { *(struct S *)(p + 0x30) = data_ov006_0213af90; }
