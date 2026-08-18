//cpp
// @symbol func_ov070_021204ec
// recovered name: FlyGuy_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daPropeller_Heyho_c::OnTurnIntoEgg - recovered from vtable slot identity */
class Player;

class dActor_c {
public:
void GivePlayerCoins(Player &player, unsigned char count, unsigned int unknown);
void KillAndTrackInDeathTable();
};

extern "C" void func_ov070_021204ec(char *c, void *player) {
dActor_c *r4 = (dActor_c *)c;
unsigned char r2 = ((unsigned char *)r4)[0x10a];
r4->GivePlayerCoins(*(Player *)player, r2 + 1, 0);
r4->KillAndTrackInDeathTable();
}
