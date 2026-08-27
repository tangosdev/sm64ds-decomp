struct dActor_c;
extern unsigned char data_0209f2d8;
extern unsigned char data_0208a0e0;
extern void *data_0209f394[];
extern int *_ZN8dActor_c13ClosestPlayerEv(void *self);

int func_02005e28(unsigned char *self)
{
    int b;
    int i;
    int n;
    b = (int)(data_0209f2d8 == 1);
    if (b != 0) {
        n = data_0208a0e0;
        for (i = 0; i < n; i++) {
            int *p = (int*)data_0209f394[i];
            if (p != 0) {
                if (p[2] == self[0x113]) return 1;
            }
        }
        return 0;
    } else {
        int *p = _ZN8dActor_c13ClosestPlayerEv(self);
        int r1;
        if (p != 0) r1 = p[2];
        else r1 = self[0x113];
        return (r1 == self[0x113]) ? 1 : 0;
    }
}
