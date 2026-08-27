//cpp
struct V {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21();
};

extern "C" void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern "C" int _ZN9Animation8FinishedEv(void* a);
extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void* self, void* file, int b, int d, int fix, unsigned short e);
extern "C" int _ZNK9Animation12WillHitFrameEi(void* a, int frame);
extern "C" void func_ov006_020c1420(char* c, short a1, int a2);

extern V* data_ov004_020beb68;

extern "C" void func_ov006_020c14bc(char* c)
{
    if (*(int*)(c + 0x268) == 0) {
        _ZN5Sound12PlayBank2_2DEj(0x139);
        *(int*)(c + 0x268) = 1;
    }

    if (*(int*)(c + 0x7c) == *(int*)(c + 0x204)) {
        if (_ZN9Animation8FinishedEv(c + 0x6c))
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void**)(c + 0x20c), 0, 0, 0x800, 0);
        return;
    }

    if (_ZNK9Animation12WillHitFrameEi(c + 0x6c, 0)) {
        short cv = *(short*)(c + 0x100 + 0xd8);
        short* p = (short*)(((long)c + 0x1d8));
        short old = *p;
        *p = old - 1;
        if (cv <= 0) {
            func_ov006_020c1420(c, *(short*)(c + 0x1de), *(int*)(c + 0x1d4));
            return;
        }
    }

    if (!_ZNK9Animation12WillHitFrameEi(c + 0x6c, 4))
        return;

    _ZN5Sound12PlayBank2_2DEj(0x141);
    if (*(short*)(c + 0x1d8) == *(short*)(c + 0x1da)) {
        V* o = data_ov004_020beb68;
        if (o)
            o->v21();
    }
}
