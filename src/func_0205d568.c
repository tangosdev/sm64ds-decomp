typedef struct Node Node;

struct Node {
    char _pad0[0x8];
    int field8;
    unsigned int fieldC;
    char _pad10[0x2C - 0x10];
    int field2C;
    int field30;
};

extern int func_0205cdf4(Node *node, int a, int b, int c);

int func_0205d568(Node *node, int b, ...)
{
    unsigned int *p;
    if (b == 0) {
        return 0;
    }
    node->field8 = b;
    {
        int b2 = b;
        int c = *(int *)((char *)&b + 4);
        node->field2C = b2;
        node->field30 = c;
        if (func_0205cdf4(node, 6, c, b2) == 0) {
            return 0;
        }
    }
    p = (unsigned int *)((char *)node + 0xc);
    *p |= 0x10;
    *p &= ~0x20;
    return 1;
}
