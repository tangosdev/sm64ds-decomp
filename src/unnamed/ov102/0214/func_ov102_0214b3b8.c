extern void func_ov102_0214bc20(void *c);

void func_ov102_0214b3b8(void *c)
{
    if (*(int *)((char *)c + 0x3dc) != 3) return;
    func_ov102_0214bc20(c);
    void *p = *(void **)((char *)c + 0x390);
    *(short *)((char *)c + 0x94) = *(short *)((char *)p + 0x8e);
    *(void **)((char *)c + 0x390) = 0;
}
