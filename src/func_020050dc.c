extern void func_0200cb58(void *obj, int index);
extern void func_0200cce4(void *obj);

int func_020050dc(void *obj)
{
    func_0200cb58(obj, 0x1e);
    func_0200cce4(obj);
    *(int *)(((long long)(int)((char *)obj + 0x154)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x100;
    return 1;
}