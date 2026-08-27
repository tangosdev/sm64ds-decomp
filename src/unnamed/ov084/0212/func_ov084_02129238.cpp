//cpp
// @symbol func_ov084_02129238
/* recovered: shared common types */
#include "common.h"
typedef int s32;

struct dActor_c;

struct dBgCh_Gnd {
    char pad0[0x14];
    int field14;     /* 0x14 */
    char pad18[0x44 - 0x18];
    int field44;     /* 0x44 */
    char pad48[0x50 - 0x48];
    dBgCh_Gnd();
    ~dBgCh_Gnd();
    void StartDetectingWater();
    void StartDetectingToxic();
    void StopDetectingOrdinary();
    void SetObjAndPos(const Vector3& pos, dActor_c* a);
    int DetectClsn();
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN5dBgCh19StartDetectingWaterEv(void *);
extern "C" void _ZN5dBgCh19StartDetectingToxicEv(void *);
extern "C" void _ZN5dBgCh21StopDetectingOrdinaryEv(void *);


extern "C" {
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* p);
int func_02037e20(int* p);
void _ZN12dEnemyBase_c9SpawnCoinEv(void* p);
void _ZN8dActor_c8PoofDustEv(void* p);
void func_ov084_02129498(char* r0);
void _ZN11dCapEnemy_c10ReleaseCapERK7Vector3(void* thiz, const Vector3* v);
void _ZN11dCapEnemy_c15RespawnIfHasCapEv(void* p);
}

extern "C" void func_ov084_02129238(char* c);
void func_ov084_02129238(char* c)
{
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x1b4) != 0) return;
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
        dBgCh_Gnd rg;
        _ZN5dBgCh19StartDetectingWaterEv(&(rg));
        _ZN5dBgCh19StartDetectingToxicEv(&(rg));
        _ZN5dBgCh21StopDetectingOrdinaryEv(&(rg));
        rg.SetObjAndPos(pos, (dActor_c*)c);
        if (rg.DetectClsn() != 0) {
            if (func_02037e20(&rg.field14) != 0) {
                if (rg.field44 != (int)0x80000000) {
                    if (*(int*)(c + 0x60) < rg.field44) {
                        _ZN12dEnemyBase_c9SpawnCoinEv(c);
                        _ZN8dActor_c8PoofDustEv(c);
                        func_ov084_02129498(c);
                        {
                            Vector3 cap;
                            cap.x = 0;
                            cap.y = 0x6c000;
                            cap.z = 0;
                            _ZN11dCapEnemy_c10ReleaseCapERK7Vector3(c, &cap);
                        }
                        *(int*)(c + 0x5c) = *(int*)(c + 0x41c);
                        *(int*)(c + 0x60) = *(int*)(c + 0x420);
                        *(int*)(c + 0x64) = *(int*)(c + 0x424);
                        _ZN11dCapEnemy_c15RespawnIfHasCapEv(c);
                    }
                }
            }
        }
    }
}
