typedef volatile unsigned short vu16;

typedef struct Node {
    int f0;
    int f4;
    int f8;
    char pad_c[0x30 - 0xc];
    struct Node *next;
} Node;

typedef struct Arg {
    int flags;
    int mask;
    Node *head;
} Arg;

extern int data_020d7d6c[];
extern void func_ov007_020c076c(Node *node, int flags);

void func_ov007_020c03b0(Arg *arg) {
    Node *volatile arr_a[4];
    int arr_b[4];
    Node *volatile *pa;
    int *pb;
    Node *node;
    int mask;
    int idx, pri;
    int bit1, bit2;
    int f;

    pa = (Node *volatile *)(((long long)(int)arr_a) & 0xffffffffffffffffLL);
    pa[0] = 0;
    pa[1] = 0;
    pa[2] = 0;
    pa[3] = 0;
    pb = (int *)(((long long)(int)arr_b) & 0xffffffffffffffffLL);
    pb[0] = 0;
    pb[1] = 0;
    pb[2] = 0;
    pb[3] = 0;

    f = arg->flags;
    node = arg->head;
    mask = arg->mask;
    bit1 = f & 1;
    bit2 = f & 2;
    if (node == 0) return;

    do {
        idx = node->f0;
        pri = node->f4;
        if (idx >= 0 && idx <= 3 && pri >= 0 && pri <= 3 && arr_b[idx] == 0) {
            arr_a[idx] = node;
            if (node->f8 == 1 && (mask & data_020d7d6c[idx]) != 0) {
                func_ov007_020c076c(node, arg->flags);
                if (bit1) {
                    switch (idx) {
                        case 0: *(vu16 *)0x4000008 = (*(vu16 *)0x4000008 & ~3) | pri; break;
                        case 1: *(vu16 *)0x400000a = (*(vu16 *)0x400000a & ~3) | pri; break;
                        case 2: *(vu16 *)0x400000c = (*(vu16 *)0x400000c & ~3) | pri; break;
                        case 3: *(vu16 *)0x400000e = (*(vu16 *)0x400000e & ~3) | pri; break;
                    }
                }
                if (bit2) {
                    switch (idx) {
                        case 0: *(vu16 *)0x4001008 = (*(vu16 *)0x4001008 & ~3) | pri; break;
                        case 1: *(vu16 *)0x400100a = (*(vu16 *)0x400100a & ~3) | pri; break;
                        case 2: *(vu16 *)0x400100c = (*(vu16 *)0x400100c & ~3) | pri; break;
                        case 3: *(vu16 *)0x400100e = (*(vu16 *)0x400100e & ~3) | pri; break;
                    }
                }
                arr_b[idx] = 1;
            }
        }
        node = node->next;
    } while (node != 0);
}
