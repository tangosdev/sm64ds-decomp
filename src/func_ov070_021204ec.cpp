//cpp
// @symbol func_ov070_021204ec
// @emits FlyGuy_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daPropeller_Heyho_c::OnTurnIntoEgg - recovered from vtable slot identity */
class Player;

class Actor {
public:
void GivePlayerCoins(Player &player, unsigned char count, unsigned int unknown);
void KillAndTrackInDeathTable();
};

extern "C" void FlyGuy_OnTurnIntoEgg(char *c, void *player) {
Actor *r4 = (Actor *)c;
unsigned char r2 = ((unsigned char *)r4)[0x10a];
r4->GivePlayerCoins(*(Player *)player, r2 + 1, 0);
r4->KillAndTrackInDeathTable();
}
