struct Obj;
int func_0200751c(struct Obj *o) {
    int *p = (int *)((char *)o + 0x110);
    int tmp = *(short *)((char *)*p + 0x8e);
    tmp += 0x8000;
    *(short *)((char *)o + 0x186) = (short)tmp;
    *(int *)((char *)o + 0xb0) = 0;
    *(int *)((char *)o + 0xb4) = 0;
    *(int *)((char *)o + 0xb8) = 0;
    return 1;
}
