struct Pair { int a, b; };
extern struct Pair data_ov006_0213cb5c;

void func_ov006_020ee4e0(char *p)
{
    *(unsigned short *)(p + 0x5014) = 0x20;
    *(struct Pair *)(p + 0x5004) = data_ov006_0213cb5c;
}
