struct S48 { int w[12]; };
extern volatile struct S48 data_ov006_0213ad28;
void func_ov006_020c0af8(char* c)
{
    struct S48 tmp;
    *(volatile struct S48*)&tmp = data_ov006_0213ad28;
    *(struct S48*)(c + 0x38) = *(struct S48*)&data_ov006_0213ad28;
    *(struct S48*)(c + 0xa8) = tmp;
}
