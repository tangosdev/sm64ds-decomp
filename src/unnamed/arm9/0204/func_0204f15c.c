extern void func_0205aa9c(void *a, int b);
extern unsigned int data_020a4d50;

void func_0204f15c(void *node)
{
    if (node == 0)
        return;
    func_0205aa9c(node, 0);
    data_020a4d50 &= ~(unsigned int)node;
}
