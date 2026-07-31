struct Pair { int a, b; };
extern void* data_ov006_0214041c;
extern struct Pair data_ov006_0213b068;
extern struct Pair data_ov006_0213b078;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int b, int f, unsigned int g);
extern void func_02012718(void* a, int b);
extern void Sound_PlayBank1Panned(int a0, char* a1, void* a2);

void func_ov006_020c8270(char* c)
{
    struct Pair* g;
    struct Pair* p;
    int t0, t1;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c + 0x4c), data_ov006_0214041c, 0x40000000, 0x800, 0);
    *(int*)(c + 0xa4) = 0;
    p = (struct Pair*)(((int)c + 0x3c) & 0xFFFFFFFFFFFFFFFF);
    g = &data_ov006_0213b068;
    if (p->a == g->a &&
        (p->b == g->b || *(int*)(c + 0x3c) == 0)) {
        func_02012718((void*)0x110, *(short*)(c + 0x36) << 12);
    } else {
        Sound_PlayBank1Panned(0, (char*)4, *(void**)(c + 0x14));
    }
    func_02012718((void*)0x1b5, *(short*)(c + 0x36) << 12);
    t0 = data_ov006_0213b078.a;
    t1 = t0 ? data_ov006_0213b078.b : data_ov006_0213b078.b;
    *(int*)(c + 0x3c) = t0;
    *(int*)(c + 0x40) = t1;
}
