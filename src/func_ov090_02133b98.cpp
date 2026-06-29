//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* file, int a, int b);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* f);
extern void _ZN7PathPtrC1Ev(void* thiz);
extern void _ZN7PathPtr6FromIDEj(void* thiz, unsigned int id);
extern int _ZNK7PathPtr8NumNodesEv(void* thiz);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
    void* thiz, void* actor, void* pos, int f, int g, unsigned int h, unsigned int i);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* thiz, void* out, unsigned int j);
extern void func_ov090_021338b4(void* c, void* p);

extern char data_ov090_021345a4[];
extern char data_ov090_021345ac[];
extern char data_ov090_021345cc[];

struct PathPtr { int a; int b; };

int func_ov090_02133b98(char* c)
{
    PathPtr p1;
    PathPtr p2;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x30c,
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov090_021345a4), 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov090_021345ac);
    *(int*)(c + 0x388) = *(int*)(c + 8) & 0xff;
    if (*(int*)(c + 0x388) < 0) *(int*)(c + 0x388) = 0;
    _ZN7PathPtrC1Ev(&p1);
    _ZN7PathPtr6FromIDEj(&p1, *(int*)(c + 0x388));
    *(int*)(c + 0x38c) = _ZNK7PathPtr8NumNodesEv(&p1);
    *(int*)(c + 0xa0) = -0x3c000;
    *(int*)(c + 0x374) = 0;
    *(int*)(c + 0x378) = 0;
    *(int*)(c + 0x37c) = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        c + 0x110, c, c + 0x374, 0x42000, 0x6e000, 0x200004, 0);
    _ZN7PathPtrC1Ev(&p2);
    _ZN7PathPtr6FromIDEj(&p2, *(int*)(c + 0x388));
    *(int*)(c + 0x390) = 1;
    _ZNK7PathPtr7GetNodeER7Vector3j(&p2, c + 0x5c, *(int*)(c + 0x390));
    func_ov090_021338b4(c, data_ov090_021345cc);
    return 1;
}
}
