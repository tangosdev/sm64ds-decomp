typedef int Fix12i;

void func_ov002_020bfa74(void);
void func_ov002_020c0108(void *self, int x);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *c);
void func_ov002_020ce798(void *self);

void func_ov002_020bf36c(char *self, void *arg)
{
    unsigned char r2;
    Fix12i saved;

    r2 = *(unsigned char *)(self + 0x709);
    if (r2 == 0) {
        func_ov002_020bfa74();
    }
    if ((*(unsigned char *)(self + 0x6e9) & 1) != 0 || *(unsigned char *)(self + 0x706) != 0) {
        func_ov002_020c0108(self, 1);
    }
    if (*(int *)(self + 0x37c) != 0) {
        return;
    }
    saved = *(Fix12i *)(self + 0x98);
    if ((*(unsigned char *)(self + 0x6e9) & 1) != 0) {
        *(Fix12i *)(self + 0x98) = (Fix12i)(((long long)saved * *(Fix12i *)(self + 0x558) + 0x800) >> 12);
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, arg);
    *(Fix12i *)(self + 0x98) = saved;
    func_ov002_020ce798(self);
}