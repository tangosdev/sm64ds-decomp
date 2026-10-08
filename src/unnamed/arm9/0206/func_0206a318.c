typedef unsigned int u32;

extern int IPCSend(int cmd, void *arg, int flag);
extern void WaitByLoop(int count);

void func_0206a318(void *arg)
{
    int r6, r5, r4;
    if (!IPCSend(0xd, arg, 0))
        return;
    r6 = 1;
    r5 = 0xd;
    r4 = 0;
    while (1) {
        WaitByLoop(r6);
        if (!IPCSend(r5, arg, r4))
            break;
    }
}
