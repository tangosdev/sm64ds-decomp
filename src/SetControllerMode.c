struct CtrlSlot { unsigned char mode; unsigned char pad[0x17]; };
extern struct CtrlSlot data_0209f4ae[];
extern unsigned char data_020a0e40;
extern unsigned char SAVE_DATA[];

void SetControllerMode(unsigned char mode)
{
    data_0209f4ae[data_020a0e40].mode = mode;
    SAVE_DATA[0x42] = mode;
}
