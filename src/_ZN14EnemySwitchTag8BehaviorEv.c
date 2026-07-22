typedef unsigned int u32;

extern int _ZNK6Camera12IsUnderwaterEv(void *self);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, void *v, u32 e);
extern int data_ov002_02110aec;
extern int data_0209b4ac;
extern void *data_0209f318;
extern int data_ov002_0210b498[];

int _ZN14EnemySwitchTag8BehaviorEv(char *c)
{
    u32 param;
    int a;
    void *cam;

    if (data_ov002_02110aec != 0)
        return 1;

    param = *(u32 *)(c + 8);
    if (param >= 1 && param <= 4) {
        cam = data_0209f318;
        a = data_0209b4ac;
        if (a == 0x32 || a == 0x33 || a == 0x34 ||
            _ZNK6Camera12IsUnderwaterEv(cam))
            return 1;
    }

    *(u32 *)(c + 0xd4) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(u32 *)(c + 0xd4), 3, (u32)data_ov002_0210b498[*(u32 *)(c + 8)],
        c + 0x74, 0);
    return 1;
}
