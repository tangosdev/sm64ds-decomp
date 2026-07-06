//cpp
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
int func_ov043_021112a8(char *c)
{
    if (!DecIfAbove0_Byte((unsigned char *)(c + 0x31e))) {
        *(unsigned char *)(c + 0x31e) = 0x3c;
        *(short *)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFF) = *(short *)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFF) + 0x4000;
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
}
