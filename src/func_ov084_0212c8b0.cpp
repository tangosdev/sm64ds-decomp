//cpp
extern "C" {
void* _ZN5Actor10FindWithIDEj(unsigned int id);
int _ZN6Player9StartTalkER9ActorBaseb(void* self, void* actor, int b);
void func_ov084_0212c960(void* c, int i);
}

extern "C" void func_ov084_0212c8b0(char* c)
{
    void* o;
    if ((*(int*)(c + 0xf4) & 0x8000000) == 0)
        return;
    o = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0xf8));
    if (o == 0)
        return;
    {
        int b = (*(unsigned short*)((char*)o + 0xc) == 0xbf);
        if (b == 0)
            return;
    }
    *(void**)(c + 0x194) = o;
    if (_ZN6Player9StartTalkER9ActorBaseb(*(void**)(c + 0x194), c, 0) == 0)
        return;
    func_ov084_0212c960(c, 1);
}
