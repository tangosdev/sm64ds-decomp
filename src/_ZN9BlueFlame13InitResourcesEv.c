extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *actor, int fix, int t, unsigned int e, unsigned int f);
int _ZN9BlueFlame13InitResourcesEv(char *t)
{
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(t + 0xe4, t, 0x41000, 0x41000, 0x200002, 0x8000);
    *(int *)(t + 0xd4) = *(int *)(t + 0x5c);
    *(int *)(t + 0xd8) = *(int *)(t + 0x60);
    *(int *)(t + 0xdc) = *(int *)(t + 0x64);
    return 1;
}
