//cpp
extern "C" {
void Math_Function_0203b0fc(int *p, int target, int scale, int max);
void func_ov006_020ec134(void *c);
}
struct C {
    char pad[0x10];
    void (C::*pmf)();
};
#pragma push
#pragma O3
extern "C" void func_ov006_020eb018(char *c)
{
    C *o = (C*)c;
    (o->*(o->pmf))();
    if (*(unsigned char*)(c + 0x95) != 5) return;
    *(int*)(((int)c + 0x60)) += 1;
    {
        int i = 0;
        int max = 0x200;
        int *p = (int*)(c + 0x4c);
        do {
            if (*(int*)(c + 0x60) > (5 - i) * 3)
                Math_Function_0203b0fc(p, 0x9800, 0x400, max);
            i++;
            max += 0x140;
            p++;
        } while (i < 5);
    }
    func_ov006_020ec134(c);
}
#pragma pop
