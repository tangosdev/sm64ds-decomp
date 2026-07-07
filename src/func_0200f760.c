extern void *_ZN5Actor13ClosestPlayerEv(void);

void func_0200f760(void *self, char *obj)
{
    int *flags;
    void *player;

    flags = (int *)(obj + 0x18);
    *flags &= ~2;
    player = _ZN5Actor13ClosestPlayerEv();
    if (player == 0) {
        return;
    }
    if (((char *)player)[0x6fb] != 0) {
        *flags |= 2;
    }
}