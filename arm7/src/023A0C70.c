extern char *unk_380FFF4;
extern void fn_02394DF8(void *list, int node);

void fn_023A0C70(void) {
    char *base;
    unsigned i;
    int stride;
    char **pp;

    pp = &unk_380FFF4;
    base = *pp + 0x4A0;
    stride = 0x18;
    for (i = 0; i < 3; i++) {
        int off = i * stride;
        unsigned short *slot = (unsigned short *)(base + off);
        if (*(unsigned short *)(base + off) != 0) {
            *slot = (unsigned short)(*slot - 1);
            if (*slot == 0)
                fn_02394DF8(*pp + 0x14C, *(int *)((char *)slot + 0x14));
        }
    }
}
