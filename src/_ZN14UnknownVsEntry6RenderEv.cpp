//cpp
// @symbol _ZN14UnknownVsEntry6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "UnknownVsEntry.h"
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

int UnknownVsEntry::Render()
{
    Sub* s = (Sub*)((char*)&mModel);
    s->m(0);
    _ZN11ShadowModel9RenderAllEv();
    _ZN8Particle10SysTracker6UpdateEv((char*)&mParticle);
    int i = 0;
    char* p = ((char*)this) + 0x920;
    do {
        func_ov075_02114be4(p);
        i++;
        p += 0x158;
    } while (i < 4);
    if (mAnimActive) {
        Sub* s2 = (Sub*)((char*)&mModelAnim);
        s2->m(0);
    }
    func_ov075_0211b3d8((char*)&unk_e80);
    _ZN8Particle9RenderAllEv();
    return 1;
}
