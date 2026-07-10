typedef int Fix12;
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, Fix12 a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern short data_02082214[];
int _ZN15FireSeaElevator8BehaviorEv(char *c)
{
    _ZN12CylinderClsn5ClearEv(c + 0x320);
    _ZN12CylinderClsn6UpdateEv(c + 0x320);
    if (*(unsigned int*)(c + 8) != 0xffff) {
        int idx = *(unsigned short*)(c + 0x354) >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        *(int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF) =
            *(int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF) + (int)(((long long)s * 0x7000 + 0x800) >> 12);
    } else {
        int idx = *(unsigned short*)(c + 0x354) >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        *(int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF) =
            *(int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF) - (int)(((long long)s * 0x3000 + 0x800) >> 12);
    }
    *(short*)(((int)c + 0x354) & 0xFFFFFFFFFFFFFFFF) =
        *(short*)(((int)c + 0x354) & 0xFFFFFFFFFFFFFFFF) + 0x100;
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0x400000, 0)) {
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    }
    return 1;
}
