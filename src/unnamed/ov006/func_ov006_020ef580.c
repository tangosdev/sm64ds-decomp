struct Pair { int a; int b; };
extern struct Pair data_ov006_0213cc9c;

void func_ov006_020ef580(char *self)
{
    *(short *)(self + 0x5a74) = 0x20;
    *(struct Pair *)(self + 0x5004) = data_ov006_0213cc9c;
}
