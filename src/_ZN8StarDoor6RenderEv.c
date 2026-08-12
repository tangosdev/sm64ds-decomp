extern unsigned char IsAreaShowing(int idx);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void _ZN11CommonModel6RenderEPK7Vector3(int self, int v);

int _ZN4Door6RenderEv(int c)
{
    if (IsAreaShowing((signed char)*(short*)(c + 0x8c)) != 0 ||
        IsAreaShowing((signed char)*(short*)(c + 0x90)) != 0) {
        Matrix4x3_FromRotationY((void*)(c + 0xe0), *(short*)(c + 0x8e));
        *(int*)(c + 0x104) = (*(int*)(c + 0x5c) + *(int*)(c + 0xa4)) >> 3;
        *(int*)(c + 0x108) = (*(int*)(c + 0x60) + *(int*)(c + 0xa8)) >> 3;
        *(int*)(c + 0x10c) = (*(int*)(c + 0x64) + *(int*)(c + 0xac)) >> 3;
        _ZN11CommonModel6RenderEPK7Vector3(c + 0xd4, 0);

        Matrix4x3_FromRotationY((void*)(c + 0xe0), (short)(*(short*)(c + 0x8e) + 0x8000));
        *(int*)(c + 0x104) = (*(int*)(c + 0x5c) - *(int*)(c + 0xa4)) >> 3;
        *(int*)(c + 0x108) = (*(int*)(c + 0x60) - *(int*)(c + 0xa8)) >> 3;
        *(int*)(c + 0x10c) = (*(int*)(c + 0x64) - *(int*)(c + 0xac)) >> 3;
        _ZN11CommonModel6RenderEPK7Vector3(c + 0xd4, 0);
    }

    return 1;
}
