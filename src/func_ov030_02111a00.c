extern void func_ov030_02111908(void* c);
extern int _ZNK9Animation12WillHitFrameEi(void* a, int f);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* m, void* f, int a, int b, unsigned int e);
extern int data_ov030_02115cf0[];
extern int data_ov030_02115cd0[];
extern int data_ov030_02115cf8[];
extern int data_ov030_02115cd8[];
extern void** data_ov030_02115bc8[];

int func_ov030_02111a00(char* c)
{
    func_ov030_02111908(c);
    if (_ZNK9Animation12WillHitFrameEi(c + 0x124, 0) == 0) {
        int v = *(int*)(c + 0x134);
        int b;
        b = (int)(v == data_ov030_02115cf0[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cd0[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cf8[1]); if (b != 0) goto fail;
        b = (int)(v == data_ov030_02115cd8[1]); if (b != 0) goto fail;
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        c + 0xd4, data_ov030_02115bc8[*(unsigned char*)(c + 0x3ca)][1], 0, 0x1000, 0);
    {
        unsigned char* p = (unsigned char*)(((int)c + 0x3ca) & 0xFFFFFFFFFFFFFFFF);
        *(int*)(c + 0x130) = 0x1000;
        (*p)++;
    }
    if (*(unsigned char*)(c + 0x3ca) >= 0xb)
        *(unsigned char*)(c + 0x3ca) = 0;
    return 1;
fail:
    return 0;
}
