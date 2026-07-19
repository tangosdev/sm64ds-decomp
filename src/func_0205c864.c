extern int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int);
extern void func_020580f0(void *p);

int func_0205c864(int *thiz)
{
    unsigned int saved;
    int notset, ret, b;
    int dummy1, dummy2, dummy3; // force push {r4, r5, r6, r7, r8, sb, lr}
    saved = (unsigned int)_ZN3IRQ7DisableEv();
    notset = (thiz[4] & 8) ? 1 : 0;
    ret = (notset == 0) ? 1 : 0;
    if (ret) {
        b = (*(int*)((char*)thiz + 0x10) & 0x10) ? 1 : 0; // force ldr r0, [r7, #0x10] instead of ands r0, r1, #0x10
        if (b == 0) {
            goto setflag;
        }
        *(int *)(((long long)(int)((char*)thiz + 0x10)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x40;
        do {
            func_020580f0((char*)thiz + 0xe);
        } while ((*(int*)((char*)thiz + 0x10) & 0x40) ? 1 : 0);
        goto done;
    setflag:
        *(int *)(((long long)(int)((char*)thiz + 0x10)) & 0xFFFFFFFFFFFFFFFFLL) |= 8;
    done:
        ;
    }
    _ZN3IRQ7RestoreEj(saved);
    return ret;
}
