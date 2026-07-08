extern int CURR_PLAYER_ID;
extern int data_0209b118;
extern void _ZN6Camera11ChangeStateEPNS_5StateE(void *thiz, void *state);

void func_0200d148(struct Camera *thiz, unsigned char playerID) {
    if (playerID != *(unsigned char *)&CURR_PLAYER_ID)
        return;
    _ZN6Camera11ChangeStateEPNS_5StateE(thiz, (void *)&data_0209b118);
}
