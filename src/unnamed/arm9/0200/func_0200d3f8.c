extern int data_0209f250;
extern int data_0209b0d8;
extern void _ZN6Camera11ChangeStateEPNS_5StateE(void *camera, void *state);

void func_0200d3f8(void *thiz, unsigned char playerID, void *ptr)
{
    unsigned char tmp = *(unsigned char *)&data_0209f250;
    if (playerID != tmp)
        return;
    *(void **)((char *)thiz + 0x11c) = ptr;
    _ZN6Camera11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b0d8);
}
