struct Camera;
struct State;

extern int CURR_PLAYER_ID;
extern int data_0209b028;

extern void _ZN6Camera11ChangeStateEPNS_5StateE(struct Camera *thiz,
                                               struct State *state);

void func_0200d72c(struct Camera *thiz, unsigned char playerID)
{
    if (playerID == *(unsigned char *)&CURR_PLAYER_ID)
        _ZN6Camera11ChangeStateEPNS_5StateE(thiz,
                                            (struct State *)&data_0209b028);
}
