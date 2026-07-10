//cpp
struct Actor;
typedef void (Actor::*PMF)();
struct Entry { PMF pmf; };
extern "C" Entry data_ov060_0211afb4[];
extern "C" void func_02038420(void *p);
extern "C" int _ZNK12WithMeshClsn10IsOnGroundEv(void *c);
extern "C" void func_ov060_02116740(char* c);
extern "C" void func_ov060_02117624(char *c);
struct CylinderClsn { void Clear(); void Update(); };

extern "C" int _ZN10BowserFire8BehaviorEv(char *thiz)
{
    Actor *self = (Actor*)thiz;
    *(int*)(((int)thiz + 0x370) & 0xFFFFFFFFFFFFFFFF) += 1;
    (self->*data_ov060_0211afb4[*(int*)(thiz + 0x35c)].pmf)();
    *(unsigned short*)(((int)thiz + 0x374) & 0xFFFFFFFFFFFFFFFF) += 1;
    if (*(int*)(thiz + 0x9c) != 0) {
        func_02038420(thiz + 0x110);
        if (*(int*)(thiz + 0x35c) != 4) {
            if (_ZNK12WithMeshClsn10IsOnGroundEv(thiz + 0x110) != 0) {
                *(int*)(thiz + 0xa8) = 0;
                *(int*)(thiz + 0x9c) = 0;
            }
        }
    }
    func_ov060_02116740(thiz);
    func_ov060_02117624(thiz);
    ((CylinderClsn*)(thiz + 0x2d0))->Clear();
    ((CylinderClsn*)(thiz + 0x2d0))->Update();
    return 1;
}
