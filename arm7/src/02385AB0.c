/* storage 0x02385AB0 runtime 0x037FD948 size 0x74 wram ARM. No pool. */

extern int fn_023857E0(int result, int a2);
extern int fn_02384508(int a1);
extern void fn_02384524(int a1, int a2);
extern void fn_02384518(int a1);
extern void fn_023843C0(int a1);

void fn_02385AB0(int a1, int a2, int a3) {
    int node;
    int arg;

    node = *(int *)(a1 + 0x3c);
    fn_023857E0(a1, a2);
    arg = a3 & 255;
    while (node != 0) {
        if (fn_02384508(node) != 0) {
            if (a3 >= 0)
                fn_02384524(node, arg);
            *(unsigned char *)(node + 0x22) = 1;
            fn_02384518(node);
        }
        fn_023843C0(node);
        node = *(int *)(node + 0x50);
    }
    *(int *)(a1 + 0x3c) = 0;
}
