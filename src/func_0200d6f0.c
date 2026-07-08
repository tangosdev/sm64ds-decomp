typedef struct Camera Camera;

extern int CURR_PLAYER_ID;
extern int data_0209b038;
extern void _ZN6Camera11ChangeStateEPNS_5StateE(Camera *thiz, void *state);

void func_0200d6f0(Camera *thiz, unsigned char playerID)
{
    if (playerID == *(unsigned char *)&CURR_PLAYER_ID)
    {
        _ZN6Camera11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b038);
    }
}
