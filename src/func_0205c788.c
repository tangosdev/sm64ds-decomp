struct Node;
extern void func_0205d920(struct Node *node);
extern void func_0205807c(unsigned short *self);

void func_0205c788(char *self, int arg)
{
    func_0205d920((struct Node *)self);
    *(int *)(((long long)(int)(self + 0xc)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0xf;
    *(int *)(self + 0x14) = arg;
    func_0205807c((unsigned short *)(self + 0x18));
}
