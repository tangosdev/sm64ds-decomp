typedef short s16;
typedef unsigned char u8;
typedef unsigned int u32;

extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern int _ZN6Player17SetNoControlStateEhih(void *self, int a, int b, int c);
extern void _ZN6Player18SetNewHatCharacterEjjb(void *self, unsigned int a, unsigned int b, int c);
extern void func_02013a88(void);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int a, int fix, unsigned int j);
extern int _ZN9Animation8FinishedEv(void *self);
extern int _ZN6Player12Unk_020c9e5cEh(void *self, int a);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void *v, void *rot, int e, int f);

extern int data_ov002_0210de38[];
extern int data_ov002_0210de30[];

int func_ov002_020b74d0(char *c) {
    if (DecIfAbove0_Short((unsigned short *)(c + 0x100)) == 0) {
        _ZN9ActorBase18MarkForDestructionEv(c);
        return 1;
    }

    if (*(void **)(c + 0x3c0) == 0) {
        *(void **)(c + 0x3c0) = _ZN5Actor13ClosestPlayerEv(c);
        return 1;
    }

    *(int *)(c + 0xc8) = 0;

    {
        char *p = *(char **)(c + 0x3c0);
        char *q = (char *)(((int)(p + 0x8c)) & 0xFFFFFFFFFFFFFFFFLL);
        *(s16 *)(c + 0x92) = *(s16 *)(q + 0);
        *(s16 *)(c + 0x94) = *(s16 *)(q + 2);
        *(s16 *)(c + 0x96) = *(s16 *)(q + 4);
    }
    *(s16 *)(c + 0x8c) = *(s16 *)(c + 0x92);
    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
    *(s16 *)(c + 0x90) = *(s16 *)(c + 0x96);

    {
        char *p = *(char **)(c + 0x3c0);
        char *q = (char *)(((int)(p + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        *(int *)(c + 0x5c) = *(int *)(q + 0);
        *(int *)(c + 0x60) = *(int *)(q + 4);
        *(int *)(c + 0x64) = *(int *)(q + 8);
    }

    switch (*(u8 *)(c + 0x401)) {
    case 1:
        if (_ZN6Player17SetNoControlStateEhih(*(void **)(c + 0x3c0), 0xf, -1, 0) == 1) {
            _ZN6Player18SetNewHatCharacterEjjb(*(void **)(c + 0x3c0), *(u32 *)(c + 0x3f4) & 0xff, 0, 0);
            func_02013a88();
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void *)data_ov002_0210de38[1], 0x40000000, 0x1000, 0);
            *(u8 *)(c + 0x401) = 2;
        }
        break;
    case 2:
        if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
            _ZN9ActorBase18MarkForDestructionEv(c);
        }
        break;
    }

    if (*(u8 *)(c + 0x401) != 0) {
        return 1;
    }

    if (*(u8 *)(c + 0x3fe) == 0) {
        if (_ZN6Player12Unk_020c9e5cEh(*(void **)(c + 0x3c0), 8) == 1) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void *)data_ov002_0210de30[1], 0x40000000, 0x1000, 0);
            *(u8 *)(c + 0x3fe) = 1;
        }
    }

    if (*(u8 *)(c + 0x3fe) == 1) {
        if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
            if (*(int *)(c + 0x3f0) == 4) {
                _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x10d, *(u32 *)(c + 8), c + 0x3c4, 0, *(signed char *)(c + 0xcc), -1);
            }
            _ZN9ActorBase18MarkForDestructionEv(c);
        }
    }

    return 1;
}
