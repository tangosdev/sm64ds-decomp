extern void func_ov071_021202ec(void* t, int n);
extern void _ZN12CylinderClsn5ClearEv(void* clsn);

int func_ov071_0211fa54(void* thiz)
{
    char* c = (char*)thiz;
    int b0 = (int)((*(int*)(c + 0xb0) & 0x40000) != 0);
    if (b0 != 0) {
        int* src = (int*)(((int)(*(char**)(c + 0xd0)) + 0x5c) & 0xFFFFFFFFFFFFFFFF);
        *(int*)(c + 0x5c) = src[0];
        *(int*)(c + 0x60) = src[1];
        *(int*)(c + 0x64) = src[2];
    }
    {
        int v = *(int*)(c + 0xb0);
        int b1 = (int)((v & 0x80000) != 0);
        if (b1 != 0) {
            func_ov071_021202ec(c, 7);
            goto done;
        }
        {
            int b2 = (int)((v & 0x20000) != 0);
            if (b2 != 0) goto done;
        }
        {
            int b3 = (int)((v & 0x40000) != 0);
            if (b3 != 0) goto done;
        }
        *(int*)(c + 0xd0) = 0;
        func_ov071_021202ec(c, 2);
    }
done:
    _ZN12CylinderClsn5ClearEv(c + 0x160);
    return 1;
}
