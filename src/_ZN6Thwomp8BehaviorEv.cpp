//cpp
extern "C" {
void func_ov091_02133020(char *c);
void func_ov091_02132ff4(char *c);
void func_ov091_02132f04(char *c);
void func_ov091_02132e98(char *c);
void func_ov091_02132e64(char *c);
void func_ov091_02133098(char *c);
int func_ov091_02132dc0(char *c);
int _ZN9Animation7AdvanceEv(void *self);
int _ZN9Animation8FinishedEv(void *self);
void _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);

int _ZN6Thwomp8BehaviorEv(char *c)
{
    if (*(int *)(c + 0x398) < 2) {
        if (*(unsigned char *)(c + 0x3a2) != 0)
            *(int *)(c + 0x398) = 2;
    }
    switch (*(int *)(c + 0x398)) {
    case 0: {
        unsigned short cnt = (unsigned short)(*(int *)(c + 0x32c) >> 12);
        if (cnt != 0) {
            *(int *)(c + 0x32c) = (int)((((unsigned)cnt - 1) << 16) >> 4);
            cnt = (unsigned short)(*(int *)(c + 0x32c) >> 12);
            if (cnt == 0) {
                char *p = c + 0x300;
                *(unsigned short *)(p + 0xa0) = 0xa;
            }
        } else {
            char *p = (char *)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFFLL);
            int t = *(unsigned short *)(p + 0xa0);
            if (t != 0) {
                unsigned short *q = (unsigned short *)(((int)c + 0x3a0) & 0xFFFFFFFFFFFFFFFFLL);
                *q = *q - 1;
            } else {
                func_ov091_02133020(c);
            }
        }
        break;
    }
    case 1:
        func_ov091_02132ff4(c);
        break;
    case 2:
        if (*(unsigned char *)(c + 0x3a2) != 0) {
            *(int *)(c + 0x32c) = 0;
            func_ov091_02132f04(c);
        } else {
            _ZN9Animation7AdvanceEv(c + 0x324);
            if (_ZN9Animation8FinishedEv(c + 0x324) != 0) {
                char *p = (char *)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFFLL);
                int t = *(unsigned short *)(p + 0xa0);
                if (t != 0) {
                    unsigned short *q = (unsigned short *)(((int)c + 0x3a0) & 0xFFFFFFFFFFFFFFFFLL);
                    *q = *q - 1;
                } else {
                    func_ov091_02132f04(c);
                }
            } else {
                char *p = c + 0x300;
                *(unsigned short *)(p + 0xa0) = 5;
            }
        }
        break;
    case 3:
        func_ov091_02132e98(c);
        if (*(int *)(c + 0x398) == 4) {
            if (*(unsigned char *)(c + 0x3a2) != 0) {
                *(unsigned char *)(c + 0x39e) = 0x5a;
                *(unsigned char *)(c + 0x3a2) = 0;
            }
        }
        break;
    case 4:
        if (*(unsigned char *)(c + 0x3a2) != 0) {
            *(unsigned char *)(c + 0x3a2) = 0;
            *(unsigned char *)(c + 0x39e) = 0x5a;
        }
        func_ov091_02132e64(c);
        break;
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    func_ov091_02133098(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) == 0) {
        if (func_ov091_02132dc0(c) == 0)
            goto done;
    }
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
done:
    return 1;
}
}
