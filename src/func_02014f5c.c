#include "types.h"
// func_02014f5c - find player by ID and call some player function
struct Actor {
    void* vtable;
    u32 uniqueID;
    u32 param1;
    u16 actorID;
};

struct Player {
    void* vtable;
};

extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);
extern void func_020caf98(struct Player* player, struct Actor* actor);

#define PLAYER_ACTOR_ID 0xbf

void func_02014f5c(struct Actor* self, u32 id) {
    struct Actor* player;
    u32 isPlayer;

    player = _ZN5Actor10FindWithIDEj(id);
    if (!player) return;

    isPlayer = (player->actorID == PLAYER_ACTOR_ID) ? 1 : 0;
    if (!isPlayer) return;

    func_020caf98((struct Player*)player, self);
}