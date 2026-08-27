void func_ov070_02121ae0(void *c, int a, int b, int d)
{
    *(int *)((char *)c + 4) = a;
    *(int *)((char *)c + 8) = b;
    *(int *)c = d;
    *(int *)((char *)c + 0xc) = 0;
}
