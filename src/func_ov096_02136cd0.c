// @symbol func_ov096_02136cd0
// recovered name: Pokey_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daSanbo_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern int _ZN5Actor15GivePlayerCoinsER6Playerhj(char *c, char *player, unsigned char r2, unsigned int r3);
extern void _ZN9ActorBase18MarkForDestructionEv(char *c);

enum Bool { FALSE, TRUE };

void func_ov096_02136cd0(char *c, char *player) {
    unsigned short v = *(unsigned short*)(c + 0xc);
    enum Bool flag = (enum Bool)(v == 0xf0);
    if (flag) {
        _ZN5Actor15GivePlayerCoinsER6Playerhj(c, player, 1, 2);
    }
    _ZN9ActorBase18MarkForDestructionEv(c);
}
