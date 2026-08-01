//cpp
// @symbol func_ov077_02126640
// recovered name: Spiny_Kill
/* recovered: renamed to Class_Method */
/* daTgz_c::Kill - recovered from vtable slot identity */
struct Vector3 { int x, y, z; Vector3() {} };
extern "C" {
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *thiz, const Vector3 &v);
void *_ZN5Actor10FindWithIDEj(unsigned int id);
int _ZN6Player12GetHurtStateEv(void *p);
int func_ov002_020db674(void *c, int a1, int a2, int a3);
void func_02012694(int a, void *b, int c);
int _ZN9Animation8FinishedEv(void *p);
void func_ov077_02126d5c(void *c, void *p);
extern int data_ov077_02127a5c[3];
extern int data_ov077_02127cf8;
}

extern "C" int func_ov077_02126640(char *c)
{
    Vector3 v;
    char *a;
    int t;
    if (*(int *)(c + 0x400) != 0) {
        v.x = data_ov077_02127a5c[0];
        v.y = data_ov077_02127a5c[1];
        v.z = data_ov077_02127a5c[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x144, v);
        if (*(int *)(c + 0x168) != 0) {
            a = (char *)_ZN5Actor10FindWithIDEj(*(int *)(c + 0x168));
            if (a != 0) {
                if (a == *(char **)(c + 0x400)) {
                    if (*(unsigned char *)(a + 0x6f9) == 0) {
                        if (*(unsigned char *)(a + 0x703) == 0) {
                            t = (*(unsigned char *)(a + 0x709) != 0);
                            if (t == 0) {
                                if (_ZN6Player12GetHurtStateEv(a) < 0) {
                                    if (func_ov002_020db674(a, 0x28000, 0x70000,
                                            (int)(short)(*(short *)(c + 0x8e) + 0x8000)) != 0) {
                                        *(int *)(c + 0x400) = 0;
                                        func_02012694(0x10b, c + 0x74, 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (_ZN9Animation8FinishedEv(c + 0x390) != 0) {
        func_ov077_02126d5c(c, &data_ov077_02127cf8);
    }
    return 1;
}
