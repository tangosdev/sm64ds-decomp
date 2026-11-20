extern int fn_02380884(int key);
extern void fn_023808C0(int token);

int fn_02394D08(unsigned short *list, unsigned short *node) {
    int token;
    int prev;

    token = fn_02380884(0x1000000);
    if (list[4] == 0) {
        *(int *)node = -1;
        *(unsigned short **)list = node;
    } else {
        prev = *(int *)((char *)list + 4);
        *(int *)node = prev;
        *(unsigned short **)(prev + 4) = node;
    }
    *(int *)((char *)node + 4) = -1;
    node[4] = list[5];
    node[5] = 0xBF1D;
    *(unsigned short **)((char *)list + 4) = node;
    {
        unsigned short *count = list + 4;
        *count = (unsigned short)(*count + 1);
    }
    fn_023808C0(token);
    return 0;
}
