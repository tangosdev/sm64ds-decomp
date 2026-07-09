extern void _ZN10FaderColor11AdvanceFadeEv(void);
extern void func_0202fb30(void *o);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(unsigned short *p, unsigned short a, int b);
extern void func_0202f290(void *o);

void func_0202f428(char *obj)
{
    if (*(int *)(obj + 0x14) == 1) {
        _ZN10FaderColor11AdvanceFadeEv();
        return;
    }
    switch (*(int *)(obj + 0x10)) {
    case 0:
        return;
    case 1:
        *(int *)(((long long)(int)(obj + 0x1c)) & 0xffffffffffffffffLL) += *(int *)(obj + 0x20);
        *(int *)(((long long)(int)(obj + 0x20)) & 0xffffffffffffffffLL) += *(int *)(obj + 0x24);
        if (*(int *)(obj + 0x1c) >= 0x200000) {
            *(int *)(obj + 0x1c) = 0x200000;
            *(char *)(obj + 0xf) = 0;
            func_0202fb30(obj);
            *(int *)(obj + 0x10) = 2;
        }
        break;
    case 2:
        return;
    case 3:
        *(int *)(((long long)(int)(obj + 0x1c)) & 0xffffffffffffffffLL) += *(int *)(obj + 0x20);
        *(int *)(((long long)(int)(obj + 0x20)) & 0xffffffffffffffffLL) += *(int *)(obj + 0x24);
        if (*(int *)(obj + 0x1c) <= 0) {
            int b;
            *(int *)(obj + 0x1c) = 0;
            *(char *)(obj + 0xf) = 0;
            b = (*(int *)(obj + 0x14) == 0) ? 0x10 : -0x10;
            _ZN3G2x18SetBlendBrightnessEPVtts((unsigned short *)0x4000050, 0x3f, b);
            _ZN3G2x18SetBlendBrightnessEPVtts((unsigned short *)0x4001050, 0x3f, b);
            func_0202fb30(obj);
            *(int *)(obj + 0x10) = 4;
        }
        break;
    case 4:
    default:
        return;
    }
    func_0202f290(obj);
}
