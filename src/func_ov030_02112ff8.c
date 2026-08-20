extern int Vec3_Dist(void *a, void *b);
extern void _ZN8dActor_c13SpawnSoundObjEj(void *c, int a);
extern void _ZN5dCc_c5ClearEv(void *p);

int func_ov030_02112ff8(char *c)
{
    *(int *)(c + 0xb0) &= ~0x80000;
    if (Vec3_Dist(c + 0x380, c + 0x5c) < 0x514000 &&
        *(int *)(c + 0x60) > *(int *)(c + 0x384) - 0x12c000) {
        *(unsigned char *)(c + 0x3c7) = 0;
        _ZN8dActor_c13SpawnSoundObjEj(c, 1);
    } else {
        *(unsigned char *)(c + 0x3c7) = 3;
    }
    *(int *)(c + 0x98) = 0;
    *(unsigned char *)(c + 0x3c6) = 0x3c;
    _ZN5dCc_c5ClearEv(c + 0x160);
    *(int *)(c + 0x3b8) = *(int *)(c + 0x3b4);
    *(int *)(c + 0x3b4) = 6;
    return 1;
}
