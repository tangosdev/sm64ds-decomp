extern int Wireless_Reset(void);
extern int func_0203fa50(void);
extern void func_02040bb0(void);
extern int data_020a0f5c;
extern int data_020a0f94;

/* NitroSDK WMStartMPCallback, as far as this reads it. */
struct WMStartMPCallback {
    unsigned short apiid;   /* 0x0 */
    unsigned short errcode; /* 0x2 */
    unsigned short state;   /* 0x4 */
};

/* The WM_StartMP callback func_0203faa8 installs. On MP_START (0xa): in link
   mode 0 reset; as parent (3) or child (4) start data sharing
   (func_0203fa50), and if that fails clear the mode and reset, else copy
   the mode into the link status data_020a0f94 and go on (func_02040bb0).
   Errcodes 9, 0xd and 0xf are let pass; any other error clears the mode
   and resets. */
void func_0203fdac(void *arg) {
    struct WMStartMPCallback *cb = (struct WMStartMPCallback *)arg;
    unsigned short errcode = cb->errcode;
    if (errcode == 0) {
        switch (cb->state) {
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
        switch (errcode) {
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
