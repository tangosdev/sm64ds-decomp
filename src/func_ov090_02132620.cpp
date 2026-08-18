//cpp
// @symbol func_ov090_02132620
// recovered name: Skeeter_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daMenbo_c::OnTurnIntoEgg - recovered from vtable slot identity */
class Player;

class dActor_c {
public:
void GivePlayerCoins(Player &player, unsigned char count, unsigned int unknown);
void KillAndTrackInDeathTable();
};

extern "C" void func_ov090_02132620(char *c, void *player) {
dActor_c *r4 = (dActor_c *)c;
unsigned char r2 = ((unsigned char *)r4)[0x10a];
r4->GivePlayerCoins(*(Player *)player, r2 + 1, 0);
r4->KillAndTrackInDeathTable();
}
