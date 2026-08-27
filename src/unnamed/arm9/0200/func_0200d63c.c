struct Camera;
struct State;
extern int data_0209f250;
extern int data_0209b068;
extern void _ZN6Camera11ChangeStateEPNS_5StateE(struct Camera *, struct State *);

void func_0200d63c(struct Camera *thiz, unsigned char playerID)
{
    if (playerID != *(unsigned char *)&data_0209f250)
        return;
    _ZN6Camera11ChangeStateEPNS_5StateE(thiz, (struct State *)&data_0209b068);
}
