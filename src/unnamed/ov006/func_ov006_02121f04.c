struct P2 { int a, b; };
extern short data_ov006_0212e044;
extern short data_ov006_0212e048;
extern struct P2 data_ov006_0213fab0;

void func_ov006_02121f04(char *o)
{
    short a;
    short b;

    *(int *)(o + 0x5d90) = 0x5a;
    a = data_ov006_0212e044;
    *(short *)(o + 0x5db4) = a;
    b = data_ov006_0212e048;
    *(short *)(o + 0x5db6) = b;
    *(short *)(o + 0x5db0) = a;
    *(short *)(o + 0x5db2) = b;
    *(int *)(o + 0x5dac) = 0;
    *(struct P2 *)(o + 0x5004) = data_ov006_0213fab0;
}
