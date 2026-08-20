//cpp
#include "types.h"
// @symbol func_ov098_021390ec
/* recovered: shared common types */
#include "common.h"
/* Final name, not a shadow method: the ROM symbol takes Fix12<int> and the call
   site has an int. See src/_ZN17BowserSkyPlatform13InitResourcesEv.cpp for the same case. */
extern "C" short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void* self, int a, int b,
                                                       short c);
struct dBgPi { int GetClsnID() const; };
#include "SurfaceInfo.h"
struct dBgCh_Actr {
    int IsOnWall() const;
    dBgPi *GetWallResult() const;
};
struct dActor_c {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void vcall();
    static dActor_c *FindWithID(u32 id);
};

extern "C" u8 DecIfAbove0_Byte(u8 *p);
extern "C" int func_ov002_020ef228(void *c, int arg);

extern "C" void func_ov098_021390ec(char *cc)
{
    dActor_c *c = (dActor_c *)cc;
    if (DecIfAbove0_Byte((u8 *)((char *)c + 0x605)) != 0)
        return;
    if (((dBgCh_Actr *)((char *)c + 0x320))->IsOnWall() != 0) {
        dBgPi *wr = ((dBgCh_Actr *)((char *)c + 0x320))->GetWallResult();
        if (wr->GetClsnID() != -1) {
            dActor_c *a = dActor_c::FindWithID((u32)wr->GetClsnID());
            if (a != 0) {
                int isF = (*(unsigned short *)((char *)a + 0xc) == 0xf);
                if (isF == 0) {
                    c->vcall();
                    return;
                }
            }
        }
    }
    if (func_ov002_020ef228((char *)c + 0x320, (int)c) != 0) {
        *(u8 *)((char *)c + 0x605) = 3;
        return;
    }
    if (((dBgCh_Actr *)((char *)c + 0x320))->IsOnWall() == 0)
        return;
    if (*(int *)((char *)c + 0x98) > 0x14000) {
        c->vcall();
        return;
    }
    Vector3 v;
    ((SurfaceInfo *)((char *)((dBgCh_Actr *)((char *)c + 0x320))->GetWallResult() + 4))->CopyNormalTo(v);
    *(s16 *)((char *)c + 0x94) =
        _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(c, v.x, v.z, *(s16 *)((char *)c + 0x94));
}
