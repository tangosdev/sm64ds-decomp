//cpp
extern "C" {
void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e);
void _ZN7Message7EndTalkEv(void);
void _ZN9ActorBase18MarkForDestructionEv(void* p);
}
extern int* data_0209f318;

extern "C" void func_ov085_0212cd0c(char* c)
{
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x7f, 0, 0x7444, 0);
    _ZN7Message7EndTalkEv();
    {
        int* p = (int*)(((int)data_0209f318 + 0x154) & 0xFFFFFFFFFFFFFFFF);
        *p = *p & ~8;
    }
    {
        unsigned short* p = (unsigned short*)(((int)(*(char**)(c + 0x18c)) + 0x6ce) & 0xFFFFFFFFFFFFFFFF);
        *p = *p & ~0x800;
    }
    _ZN9ActorBase18MarkForDestructionEv(c);
}
