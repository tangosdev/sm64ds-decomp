struct Node;
extern void func_0205d920(struct Node *node);
extern void OS_WakeupThread(unsigned short *self);

void func_0205c788(char *self, int arg)
{
    func_0205d920((struct Node *)self);
    *(int *)(self + 0xc) &= ~0xf;
    *(int *)(self + 0x14) = arg;
    OS_WakeupThread((unsigned short *)(self + 0x18));
}
