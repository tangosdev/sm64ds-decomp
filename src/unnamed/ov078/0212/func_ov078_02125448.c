// @symbol func_ov078_02125448
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;



extern void func_02012694(int a, void* p);
extern char* _ZN8dActor_c13ClosestPlayerEv(void* c);
extern int Vec3_Dist(const void* a, const void* b);
extern int KingBobOmb_SetState(void* c, void* p);
extern s16 _ZN8dActor_c18HorzAngleToCPlayerEv(void* c);
extern void ApproachAngle(short* a, short b, short c, short d, int e);
extern char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern int _ZN6Player7TryGrabER8dActor_c(void* p, void* a);
extern int func_ov002_020db5f4(char* c, char* arg);
extern int _ZN8dActor_c13DistToCPlayerEv(void* c);

extern int data_ov078_021270fc[];
extern int data_ov078_0212707c[];
extern unsigned char data_0209f220[];

int func_ov078_02125448(char* c)
{
    char* p;
    char* a;
    struct Vector3 v;
    int isType;
    int index;
    int empty;
    int z;
    unsigned int id;
    int v4a0;
    int slot;

    if (*(unsigned char*)(c + 0x505) == 0) {
        func_02012694(0x12d, c + 0x74);
        *(unsigned char*)(c + 0x505) = 0xf;
    }
    if (func_ov078_02123804(c) == 1) {
        func_ov078_02123864(c);
        return 1;
    }

    p = _ZN8dActor_c13ClosestPlayerEv(c);
    if (p != 0) {
        v = *(struct Vector3*)(p + 0x5c);
        if (Vec3_Dist(c + 0x4d4, &v) > 0x640000
            || (*(int*)(c + 0x4d8) - 0xa000) > v.y) {
            *(int*)(c + 0x98) = 0;
            KingBobOmb_SetState(c, data_ov078_021270fc);
            return 1;
        }

        ApproachAngle((short*)(c + 0x94), _ZN8dActor_c18HorzAngleToCPlayerEv(c), 0xa, 0x200, 0x100);
        {
            s16 ang = *(s16*)(c + 0x94);
            int* pf = (int*)(c + 0xb0);
            *(s16*)(c + 0x8e) = ang;
            *pf = *pf & ~0x80;
        }

        id = *(unsigned int*)(c + 0x3a0);
        if (id != 0) {
            a = _ZN8dActor_c10FindWithIDEj(id);
            if (a != 0) {
                isType = (int)(*(unsigned short*)(a + 0xc) == 0xbf);
                if (isType != 0) {
                    if (AngleDiff(_ZN8dActor_c18HorzAngleToCPlayerEv(c), *(short*)(c + 0x8e)) > 0x2800) {
                        if ((*(int*)(c + 0x39c) & 0x1000) != 0) {
                            {
                                int* pf = (int*)(c + 0xb0);
                                *pf = *pf | 0x80;
                            }
                            if (_ZN6Player7TryGrabER8dActor_c(a, c) != 0) {
                                *(int*)(c + 0x494) = (int)a;
                                *(int*)(c + 0x98) = 0;
                                KingBobOmb_SetState(c, data_ov078_0212707c);
                            }
                        }
                    } else if (func_ov002_020db5f4(a, c) != 0) {
                        *(int*)(c + 0x494) = (int)a;
                        *(int*)(c + 0x98) = 0;
                        KingBobOmb_SetState(c, data_ov078_0212706c);
                    }
                }
            }
            return 1;
        }

        if (*(unsigned char*)(c + 0x504) == 0) {
            if (data_0209f220[0] == 1) {
                if (_ZN8dActor_c13DistToCPlayerEv(c) < 0x3e8000) {
                    if (AngleDiff(_ZN8dActor_c18HorzAngleToCPlayerEv(c), *(short*)(c + 0x8e)) < 0x2800) {
                        index = 0;
                        empty = index;
                        z = index;
#pragma opt_strength_reduction off
#pragma opt_common_subs off
                        for (; index < 2; index++) {
                            slot = *(int*)(c + 0x424 + index * 4);
                            if (slot != 0) {
                                if (_ZN8dActor_c10FindWithIDEj((unsigned int)slot) == 0) {
                                    *(int*)(c + 0x424 + index * 4) = z;
                                    *(unsigned char*)(c + 0x42c + index) = (unsigned char)z;
                                }
                            }
                            if (*(int*)(c + 0x424 + index * 4) == 0) {
                                empty++;
                                if (empty == 2)
                                    *(int*)(c + 0x4a0) ^= 3;
                            }
                        }
                        v4a0 = *(int*)(c + 0x4a0);
                        if ((v4a0 == 2 && *(int*)(c + 0x428) == 0)
                            || (v4a0 == 1 && empty == 2)) {
                            KingBobOmb_SetState(c, data_ov078_0212704c);
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 1;
}
