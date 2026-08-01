extern int _ZN6Player7IsStateERNS_5StateE(void *p, void *st);
extern int _ZN6Player11ChangeStateERNS_5StateE(void *p, void *st);
extern int _ZNK6Player14GetBodyModelIDEjb(void *p, unsigned int a, unsigned char b);
extern int _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(void *self, unsigned int a, int b, int c, unsigned short d);
extern int func_ov002_020cd28c(void *c);

extern char data_ov002_021105ec;
extern char data_ov002_021105bc;
extern char data_ov002_02110394;
extern char data_ov002_02110364;
extern int data_ov002_020ff480[];

enum { false, true };

int func_ov002_020dacb4(char *p, char *actor)
{
    int t;

    *(char **)(p + 0x358) = actor;
    *(int *)(((long long)(int)(*(char **)(p + 0x358) + 0xb0))) &= ~0x400;
    *(int *)(((long long)(int)(*(char **)(p + 0x358) + 0xb0))) &= ~0x2000;
    *(int *)(((long long)(int)(*(char **)(p + 0x358) + 0xb0))) |= 0x100;
    *(int *)(p + 0x98) = 0;

    if (*(unsigned char *)(p + 0x706) != 0) {
        func_ov002_020cd28c(p);
    } else {
        t = 0;
        if (*(unsigned short *)(*(char **)(p + 0x358) + 0xc) == 0x116) t = 1;
        if (t != 0) {
            *(unsigned char *)(p + 0x6e3) = 0;
            _ZN6Player11ChangeStateERNS_5StateE(p, &data_ov002_021105ec);
            return 1;
        }
        if (!_ZN6Player7IsStateERNS_5StateE(p, &data_ov002_021105bc)) {
            unsigned short fc;
            int u;
            *(unsigned char *)(p + 0x6e3) = 0;
            fc = *(unsigned short *)(*(char **)(p + 0x358) + 0xc);
            t = 0;
            if (fc == 0xbd) t = 1;
            if (t != 0) goto chg394;
            u = fc;
            u = u == 0xbe;
            if (u == false) goto chg364;
        chg394:
            _ZN6Player11ChangeStateERNS_5StateE(p, &data_ov002_02110394);
            goto after;
        chg364:
            _ZN6Player11ChangeStateERNS_5StateE(p, &data_ov002_02110364);
        after: ;
        }
    }

    {
        char *a2 = *(char **)(p + 0x358);
        int offset = 0xc4;
        int s = *(int *)(a2 + 0xb0);
        s = (s & 0x8000) != false;
        if (s != false) {
            offset = 0x224;
            if (*(int *)(p + 8) == 2) {
                int c = *(unsigned short *)(a2 + 0xc);
                c = c == 0xce;
                if (c != false) offset = 0xc4;
            }
        }
        {
            int player_var = *(int *)(p + 8);
            int model_idx = _ZNK6Player14GetBodyModelIDEjb(p, player_var & 0xff, 0);
            void *self = *(void **)(p + model_idx * 4 + 0xdc);
            int j = *(int *)(data_ov002_020ff480[offset + player_var] + 4);
            _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(self, j, 0, 0x1000, 0);
        }
    }
    return 1;
}
