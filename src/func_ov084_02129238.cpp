//cpp
// @symbol func_ov084_02129238
/* recovered: shared common types */
#include "common.h"
#include "dBgCh_Gnd.h"

struct dActor_c;

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
        rg.StartDetectingWater();
        rg.StartDetectingToxic();
        rg.StopDetectingOrdinary();
        rg.SetObjAndPos(pos, (dActor_c*)c);
        if (rg.DetectClsn() != 0) {
            if (func_02037e20((int*)&rg.surface) != 0) {
                if (rg.clsnY != (int)0x80000000) {
                    if (*(int*)(c + 0x60) < rg.clsnY) {
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
