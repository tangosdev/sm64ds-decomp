extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
    void* self, int bca, int a, int b, int fix, unsigned short t);
extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    unsigned int a, unsigned int b, void* pos, int rot, int e, int f);

extern int data_ov078_02126ef0[];

typedef struct {
    char pad0[0x98];
    int f98;
    int f9c;
    char pad_a0[0x2c];
    signed char fcc;
    char pad_cd[0x1ff];
    char anim[0x158];
    int arr[29];
    char pad498;
    signed char f499;
    char pad49a[2];
    int f49c;
    int f4a0;
    char pad4a4[0x48];
    int f4ec[3];
    int f4f8;
    int f4fc;
} T;

#pragma opt_strength_reduction off
#pragma opt_common_subs off
int func_ov078_02125350(int sl)
{
    T* t = (T*)sl;
    int i;
    int r;

    t->f4fc = 1;
    t->f9c = -0x2000;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        t->anim, data_ov078_02126ef0[1], 0, 0x40000000, 0x1000, 0);

    t->f98 = 0;

    for (i = 0; i < t->f4a0; i++) {
        if (t->arr[i] == 0) {
            r = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xce, 2, t->f4ec, 0, t->fcc, -1);
            if (r != 0) {
                t->arr[i] = *(int*)(r + 4);
                t->f49c = i;
                return 1;
            }
        }
    }

    t->f499 = 0;
    return 1;
}
