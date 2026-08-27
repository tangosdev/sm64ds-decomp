extern int Wireless_Reset(void);
extern int func_0203fa50(void);
extern void func_02040bb0(void);
extern int data_020a0f5c;
extern int data_020a0f94;

struct S { unsigned short h0; unsigned short h2; unsigned short h4; };

void func_0203fdac(struct S *a) {
    unsigned short h2 = a->h2;
    if (h2 == 0) {
        switch (a->h4) {
        case 0xa:
            switch (data_020a0f5c) {
            case 0:
                Wireless_Reset();
                return;
            case 3:
            case 4:
                if (func_0203fa50() == 0) {
                    data_020a0f5c = 0;
                    Wireless_Reset();
                } else {
                    data_020a0f94 = data_020a0f5c;
                    func_02040bb0();
                }
                return;
            default:
                return;
            }
        case 0xb:
        case 0xc:
            return;
        default:
            return;
        }
    } else {
        switch (h2) {
        case 9:
        case 0xd:
        case 0xf:
            return;
        default:
            data_020a0f5c = 0;
            Wireless_Reset();
            return;
        }
    }
}
