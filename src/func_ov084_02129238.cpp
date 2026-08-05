//cpp
// @symbol func_ov084_02129238
/* recovered: shared common types */
#include "common.h"
typedef int s32;

struct Actor;

struct RaycastGround {
    char pad0[0x14];
    int field14;     /* 0x14 */
    char pad18[0x44 - 0x18];
    int field44;     /* 0x44 */
    char pad48[0x50 - 0x48];
    RaycastGround();
    ~RaycastGround();
    void StartDetectingWater();
    void StartDetectingToxic();
    void StopDetectingOrdinary();
    void SetObjAndPos(const Vector3& pos, Actor* a);
    int DetectClsn();
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN4BgCh19StartDetectingWaterEv(void *);
extern "C" void _ZN4BgCh19StartDetectingToxicEv(void *);
extern "C" void _ZN4BgCh21StopDetectingOrdinaryEv(void *);


extern "C" {
int _ZNK12WithMeshClsn10IsOnGroundEv(void* p);
int func_02037e20(int* p);
void _ZN5Enemy9SpawnCoinEv(void* p);
void _ZN5Actor8PoofDustEv(void* p);
void func_ov084_02129498(char* r0);
void _ZN8CapEnemy10ReleaseCapERK7Vector3(void* thiz, const Vector3* v);
void _ZN8CapEnemy15RespawnIfHasCapEv(void* p);
}

extern "C" void func_ov084_02129238(char* c);
void func_ov084_02129238(char* c)
{
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x1b4) != 0) return;
    {
        Vector3 pos;
        {
            int vx = *(int*)(c + 0x5c);
            int vz = *(int*)(c + 0x64);
            int vy = *(int*)(c + 0x60) + 0x190000;
            pos.x = vx;
            pos.y = vy;
            pos.z = vz;
        }
        RaycastGround rg;
        _ZN4BgCh19StartDetectingWaterEv(&(rg));
        _ZN4BgCh19StartDetectingToxicEv(&(rg));
        _ZN4BgCh21StopDetectingOrdinaryEv(&(rg));
        rg.SetObjAndPos(pos, (Actor*)c);
        if (rg.DetectClsn() != 0) {
            if (func_02037e20(&rg.field14) != 0) {
                if (rg.field44 != (int)0x80000000) {
                    if (*(int*)(c + 0x60) < rg.field44) {
                        _ZN5Enemy9SpawnCoinEv(c);
                        _ZN5Actor8PoofDustEv(c);
                        func_ov084_02129498(c);
                        {
                            Vector3 cap;
                            cap.x = 0;
                            cap.y = 0x6c000;
                            cap.z = 0;
                            _ZN8CapEnemy10ReleaseCapERK7Vector3(c, &cap);
                        }
                        *(int*)(c + 0x5c) = *(int*)(c + 0x41c);
                        *(int*)(c + 0x60) = *(int*)(c + 0x420);
                        *(int*)(c + 0x64) = *(int*)(c + 0x424);
                        _ZN8CapEnemy15RespawnIfHasCapEv(c);
                    }
                }
            }
        }
    }
}
