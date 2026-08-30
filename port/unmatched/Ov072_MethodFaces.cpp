/* Lane w3-d closure addendum for the gate-193 extension: one method face.
 *
 * Two of SnowmanBody's and SnowmanHead's state handlers reach the player's
 * talk entry as a REAL C++ METHOD rather than through the ROM's C name:
 *
 *   src/func_ov072_0211fb14.cpp   declares `int StartTalk(ActorBase &, bool)`
 *                                 on its own local Player shadow and calls
 *                                 ((Player*)player)->StartTalk(*self, 1)
 *   src/func_ov072_02120450.cpp   declares `struct Player : Actor { int
 *                                 StartTalk(ActorBase &, bool); }` and calls
 *                                 the same method
 *
 * so both emit ?StartTalk@Player@@QAEHAAUActorBase@@_N@Z, which no linked TU
 * defines -- the matched body in src/ is the Itanium C name
 * _ZN6Player9StartTalkER9ActorBaseb, and every other caller in the port
 * (unmatched/Boo_StateTalk.c, unmatched/Ov085_Behaviors.cpp) spells it that
 * way. This is the Coffin closure addendum's exact shape: a spelling no
 * linked TU emits, bridged into the matched C-linkage body, which IS in the
 * map. See the bottom of port/unmatched/Bbh_MethodFaces.cpp for the pattern
 * and the same reasoning.
 *
 * NOT A BEHAVIOUR CHANGE: the face forwards, it does not reimplement.
 * Player::StartTalk is LIVE in this tree (hal/actor_vtables.cpp:437 records
 * the talk-entry session that made it so), so the yes-branch these two state
 * handlers test really enters ST_TALK.
 *
 * The parameter is mangled AAUActorBase@@ -- a reference to a STRUCT -- so
 * ActorBase is declared struct here to match what the two callers emit. This
 * TU includes no project headers on purpose: it must control the spelling of
 * both names exactly, and nothing else about either class matters to the
 * mangling.
 */
struct ActorBase;

extern "C" {
int _ZN6Player9StartTalkER9ActorBaseb(void *self, void *actor, int b);
}

struct Player { int StartTalk(ActorBase &a, bool b); };

int Player::StartTalk(ActorBase &a, bool b)
{ return _ZN6Player9StartTalkER9ActorBaseb(this, &a, b ? 1 : 0); }
