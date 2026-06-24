//cpp
typedef short s16;
struct CylinderClsn;
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void* a, CylinderClsn* c);
extern "C" void func_02012694(int a, void* p);
extern "C" int _Z14ApproachLinearRsss(short& v, short a, short b);
extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(void* a);
extern int data_02092138;

extern "C" int func_ov073_02122200(char* thiz)
{
    char* c = thiz;
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    if (*(unsigned short*)(c + 0x330) < 0x18d) {
        *(int*)(c + 0x9c) = 0;
        *(int*)(c + 0xa8) = 0;
        if (*(unsigned short*)(c + 0x330) == 0x183)
            func_02012694(0x171, c + 0x74);
        if (*(unsigned short*)(c + 0x330) < 0x183) {
            *(int*)(c + 0x9c) = -0xa000;
            _Z14ApproachLinearRsss(*(short*)(c + 0x8c), 0x2000, 0x80);
        }
    }
    if (DecIfAbove0_Short((unsigned short*)(c + 0x330)) == 0 ||
        data_02092138 - 0xc8000 > *(int*)(c + 0x60)) {
        _ZN9ActorBase18MarkForDestructionEv(c);
    }
    return 1;
}
