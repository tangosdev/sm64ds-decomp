#include "types.h"
// func_02014f5c - find player by ID and call some player function
struct dActor_c {
    void* vtable;
    u32 uniqueID;
    u32 param1;
    u16 actorID;
};

struct Player {
    void* vtable;
};

extern struct dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
extern void func_ov002_020caf98(struct Player* player, struct dActor_c* actor);

#define PLAYER_ACTOR_ID 0xbf

void func_02014f5c(struct dActor_c* self, u32 id) {
    struct dActor_c* player;
    u32 isPlayer;

    player = _ZN8dActor_c10FindWithIDEj(id);
    if (!player) return;

    isPlayer = (player->actorID == PLAYER_ACTOR_ID) ? 1 : 0;
    if (!isPlayer) return;

    func_ov002_020caf98((struct Player*)player, self);
}