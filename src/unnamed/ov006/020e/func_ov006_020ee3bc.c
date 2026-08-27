struct Pair { int a; int b; };
extern struct Pair data_ov006_0213cb6c;
extern void func_ov006_020c7418(void *c);

void func_ov006_020ee3bc(char *c)
{
    func_ov006_020c7418(c);
    *(struct Pair*)(c + 0x5004) = data_ov006_0213cb6c;
}
