extern int data_0209f250;
extern int data_0209b058;
extern void _ZN6Camera11ChangeStateEPNS_5StateE(void *thiz, void *state);

struct Camera;

void func_0200d678(struct Camera *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN6Camera11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b058);
}
