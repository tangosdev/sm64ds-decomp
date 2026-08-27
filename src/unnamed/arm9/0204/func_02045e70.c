extern void func_02045ec4(void *thiz, void *arg1, int i);

void func_02045e70(char *thiz, void *arg1)
{
    int i;
    int count = *(int *)(*(char **)thiz + 0x24);
    for (i = 0; i < count; i++)
        func_02045ec4(thiz, arg1, i);
}
