// GATE 32's READ-BACK for the direct actor spawn.
//
// THE SPAWN ITSELF IS NOT HERE ANY MORE. Gate 32 grew its own probe while the
// level-boot stream was still landing; that stream's hook (SM64DS_SPAWN_ACTOR,
// port_debug_spawn / port_debug_spawn_at / port_debug_spawn_env in
// hal/level_boot.cpp) is the one that survived the merge, and it is the better
// of the two: it fires inside the boot rather than on a frame number, it reuses
// LoadStandardObjects' own sequence counter, and it refuses a class whose
// overlay the current level does not load.
//
// What is left is the half that stream did not have: a read-back of what the
// spawned actors are DOING once the run is over. A class that spawns and then
// sits at the origin, or gets culled, or never reaches its own Render, looks
// identical in the census; it does not look identical here.
//
//   SM64DS_SPAWN_ACTOR=<id>[:<param>][@<area>][,...]   (hal/level_boot.cpp)
//
// One line per live instance of every id that env named: position, heading and
// the flags Actor::BeforeBehavior wrote, which is what says whether an actor is
// being culled rather than misbehaving.
#include <cstdio>
#include <cstdlib>

extern "C" {
extern int data_020a4b78[8];              /* the behaviour list */
const char *port_actor_class_name(unsigned id);
}

extern "C" void port_bob_spawn_report(void)
{
    const char *e = std::getenv("SM64DS_SPAWN_ACTOR");
    if (!e)
        return;
    while (*e) {
        char *end;
        unsigned id = (unsigned)std::strtoul(e, &end, 0);
        int n = 0;
        if (end == e)
            break;
        e = end;
        if (*e == ':')
            std::strtoul(e + 1, &end, 0), e = end;
        /* the @<area> suffix (hal/level_boot.cpp's spelling) -- skipped the
           same way, so a multi-entry list keeps parsing past it */
        if (*e == '@')
            std::strtol(e + 1, &end, 0), e = end;
        for (int *node = (int *)(size_t)data_020a4b78[0]; node && n < 4096;
             node = (int *)(size_t)node[1]) {
            const char *o = (const char *)(size_t)node[2];
            if (!o || *(const unsigned short *)(o + 0xc) != id)
                continue;
            ++n;
            std::printf("[spawned] %-16s at (%6d,%6d,%6d) yaw %04x "
                        "flags %08x area %d\n", port_actor_class_name(id),
                        *(const int *)(o + 0x5c) >> 12,
                        *(const int *)(o + 0x60) >> 12,
                        *(const int *)(o + 0x64) >> 12,
                        (unsigned short)*(const short *)(o + 0x8e),
                        *(const unsigned *)(o + 0xb0),
                        *(const signed char *)(o + 0xcc));
        }
        if (!n)
            std::printf("[spawned] %s (id %u): no live instance\n",
                        port_actor_class_name(id), id);
        if (*e == ',')
            ++e;
        else
            break;
    }
}
