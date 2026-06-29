//cpp
extern "C" {
extern short Vec3_HorzAngle(void* v0, void* v1);
extern void _Z14ApproachLinearRsss(short* r, short a, short b);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, void* v, unsigned int d);
extern int AngleDiff(int a, int b);
extern int Vec3_ApproachHorz(void* out, void* a, int maxStep);
extern void _Z14ApproachLinearRiii(int* r, int a, int b);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e);
extern int _ZN6Player18HasFinishedTalkingEv(void* thiz);
extern void _ZN9ActorBase18MarkForDestructionEv(void* thiz);
extern unsigned char data_0209f284[];

int func_ov085_0212e078(char* c)
{
    _Z14ApproachLinearRsss((short*)(c + 0x94), Vec3_HorzAngle(c + 0x5c, c + 0x2a4), 0x800);
    _Z14ApproachLinearRsss((short*)(c + 0x8e), *(short*)(c + 0x94), 0x800);
    *(unsigned int*)(c + 0x2e4) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(unsigned int*)(c + 0x2e4), 3, 0x182, c + 0x74, 0);
    if (AngleDiff(*(short*)(c + 0x94), Vec3_HorzAngle(c + 0x5c, c + 0x2a4)) < 0x2000) {
        Vec3_ApproachHorz(c + 0x5c, c + 0x2a4, 0x1e000);
        _Z14ApproachLinearRiii((int*)(c + 0x60), *(int*)(c + 0x2a8), 0x1e000);
    }
    if (*(unsigned short*)(c + 0x100) == 0) {
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4a, 0x7f, 0, 0x7222, 0) != 0) {
            if (_ZN6Player18HasFinishedTalkingEv(*(void**)(c + 0x2a0)) == 1) {
                _ZN9ActorBase18MarkForDestructionEv(c);
                data_0209f284[0] = 0;
            }
        }
    }
    return 1;
}
}
