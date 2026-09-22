// TEMPORARY PROBE: open a dialogue box headlessly.
//
// This is a test rig, not game behaviour. There is no in-world caller of
// Player::ShowMessage2 that fires within a few seconds of a headless castle /
// BoB boot (the sign's read-state Main is unmatched and held in a parallel
// lane, and a Bob-omb Buddy talk needs the player to walk up and press A),
// so to prove the pump + compositor on a fixed frame this asks the box to
// open directly.
//
// SM64DS_PROBE_MESSAGE=<id>  -- on a chosen frame, call Player::ShowMessage2
// on the local player with message <id> (bare "1" uses a default sample id).
// The caller (walk_window) decides the frame. Delete this file and its one
// call site when a real caller drives the box.
#include <cstdio>
#include <cstdlib>

extern "C" {

/* Player::ShowMessage(ActorBase *actor, u32 msg, const Vector3 *pos, u8 d,
   u8 e). Matched src _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh.cpp: it
   calls ShowMessage2 and then sets mStateWork (Player+0x6e5) = 1. That flag is
   what routes St_Talk_Main's box open to func_0201f32c(mAttachOffsetY) -- i.e.
   func_0201f32c(the raw message id stored at Player+0x688) -- opening the box
   on engine A with a plain text id. ShowMessage2 alone (mStateWork=0) instead
   takes func_0201fc88, which remaps the id through the OBJECT-message table
   (for signs/NPCs that speak by object id); a raw text id is not that. So the
   probe uses ShowMessage. */
int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
    void *thiz, void *actor, unsigned msg, const void *pos, unsigned char d,
    unsigned char e);

/* Returns the message id the probe wants, or -1 if the probe is off. */
int port_probe_message_id(void)
{
    const char *p = std::getenv("SM64DS_PROBE_MESSAGE");
    if (!p || !*p)
        return -1;
    int id = std::atoi(p);
    if (id <= 0)
        id = 0x2a;   /* a BoB tutorial-band id: in-range, plain text */
    return id;
}

/* Ask the box to open on `player` with message `id`. The player is passed as
   its own talk actor -- a probe does not have a real NPC, and ShowMessage2
   only stores the pointer and ORs a visibility bit through it, both of which
   the player itself satisfies. Returns ShowMessage2's own result. */
int port_probe_message_fire(void *player, int id)
{
    int r = _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
        player, player, (unsigned)id, 0, 0, 0);
    std::fprintf(stderr, "[probe] ShowMessage(id=%d) returned %d\n", id, r);
    return r;
}

} /* extern "C" */
