//cpp
extern "C" {
void _ZN11ShadowModel9RenderAllEv(void);
void _ZN8Particle10SysTracker6UpdateEv(void* p);
void func_ov075_02114be4(void* p);
void func_ov075_0211b3d8(void* p);
void _ZN8Particle9RenderAllEv(void);
}

struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};

extern "C" int _ZN14UnknownVsEntry6RenderEv(char* c) {
    Sub* s = (Sub*)(c + 0x86c);
    s->m(0);
    _ZN11ShadowModel9RenderAllEv();
    _ZN8Particle10SysTracker6UpdateEv(c + 0x50);
    int i = 0;
    char* p = c + 0x920;
    do {
        func_ov075_02114be4(p);
        i++;
        p += 0x158;
    } while (i < 4);
    if (*(unsigned char*)(c + 0xf40)) {
        Sub* s2 = (Sub*)(c + 0x8bc);
        s2->m(0);
    }
    func_ov075_0211b3d8(c + 0xe80);
    _ZN8Particle9RenderAllEv();
    return 1;
}
