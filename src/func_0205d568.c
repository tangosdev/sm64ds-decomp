// NONMATCHING: args are homed to the stack (push {r0-r3}) because a1's address is
// taken, so a1/a2 read SP-relative like the ROM. The two reloaded args color into
// the r2/r3 pair opposite to the ROM (values correct, only the register pair + load
// order of self+0x2c / self+0x30 differ). Same size; pure register coloring.
extern int func_0205cdf4(char *self, int n);

int func_0205d568(char *self, int a1, int a2)
{
    int *p;
    if (a1 == 0) return 0;
    *(int *)(self + 8) = a1;
    *(int *)(self + 0x2c) = a1;
    *(int *)(self + 0x30) = (&a1)[1];
    if (func_0205cdf4(self, 6) == 0) return 0;
    p = (int *)(((long long)(int)(self + 0xc)) & 0xFFFFFFFFFFFFFFFFLL);
    *p |= 0x10;
    *p &= ~0x20;
    return 1;
}
