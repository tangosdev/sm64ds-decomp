struct Camera;
struct State;

extern unsigned char data_0209f250;
extern struct State data_0209b078;
extern void _ZN6Camera11ChangeStateEPNS_5StateE(struct Camera *this, struct State *state);
extern void func_0200cb58(void *obj, int index);

void func_0200d5fc(struct Camera *thiz, int playerID)
{
    if (playerID == data_0209f250) {
        _ZN6Camera11ChangeStateEPNS_5StateE(thiz, &data_0209b078);
        func_0200cb58(thiz, 5);
    }
}
