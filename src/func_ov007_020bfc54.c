extern void func_ov007_020b3edc(int size);
extern void func_ov007_020c8b04(void *p);
extern void func_ov007_020bc53c(void *p);
extern void _ZN6Player17St_EndingFly_MainEv(void *p);

extern char *data_ov007_02104bd8;

void func_ov007_020bfc54(void)
{
    func_ov007_020b3edc(0x24);

    if (*(void **)data_ov007_02104bd8 != 0) {
        func_ov007_020c8b04(*(void **)data_ov007_02104bd8);
        *(int *)data_ov007_02104bd8 = 0;
    }

    if (*(void **)(data_ov007_02104bd8 + 4) != 0) {
        func_ov007_020bc53c(*(void **)(data_ov007_02104bd8 + 4));
        *(int *)(data_ov007_02104bd8 + 4) = 0;
    }

    if (data_ov007_02104bd8 == 0) {
        return;
    }

    _ZN6Player17St_EndingFly_MainEv(data_ov007_02104bd8);
    data_ov007_02104bd8 = 0;
}
