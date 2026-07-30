extern void func_ov007_020c6d20(char *self, int idx);
extern void func_ov007_020c28ac(void *p);

int func_ov007_020c6550(char *c)
{
    int status = 1;
    void *obj = *(void **)(c + 0x20);
    int left;
    if (*(unsigned short *)((char *)obj + 8) == 0 ||
        (left = *(volatile int *)(c + 8)) >= *(int *)(c + 0xc)) {
        status = 0;
    } else {
        func_ov007_020c6d20(c, left);
        {
            int *p = (int *)(((unsigned int)c + 8) & 0xFFFFFFFFFFFFFFFFULL);
            *p = *p + 1;
        }
        if (*(int *)(c + 8) >= *(int *)(c + 0xc))
            status = 2;
        func_ov007_020c28ac(obj);
    }
    return status;
}
