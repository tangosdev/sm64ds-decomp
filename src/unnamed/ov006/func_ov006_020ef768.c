struct Pair { int a; int b; };
extern struct Pair data_ov006_0213cc84;

void func_ov006_020ef768(char *self)
{
    *(short *)(self + 0x5a74) = 0xf0;
    *(struct Pair *)(self + 0x5004) = data_ov006_0213cc84;
}
