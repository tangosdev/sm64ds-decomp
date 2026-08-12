// @symbol func_ov027_02111770
/* recovered: shared common types, renamed to Class_Method, RTTI class fields named */
#include "daIDonketu_c.h"
// recovered name: daIDonketu_c_Behavior
/* recovered: shared common types, renamed to Class_Method */
/* daIDonketu_c::Behavior - recovered from vtable slot identity */
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(char* c, char* a, char* b, unsigned int d);
extern void _ZN5Actor14TriplePoofDustEv(char* c);
struct V3 { int x, y, z; };
extern void _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3h(char* c, char* sc, unsigned int u, struct V3* v, unsigned int j);
extern void _ZN9ActorBase18MarkForDestructionEv(char* c);
extern int func_ov064_02116d1c(char* c);
int func_ov027_02111770(char *c) {
    struct daIDonketu_c *self = (struct daIDonketu_c *)(void *)c;
    int r = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, c+0x174, c+0x110, 0);
    if (r != 0) {
        if (r == 2) {
            struct V3 v;
            _ZN5Actor14TriplePoofDustEv(c);
            v.x = self->unk_05c;
            v.y = self->unk_060;
            v.z = self->unk_064;
            v.y = v.y + 0x64000;
            _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3h(c, (char*)(c+0x3fb), (self->unk_3fa | 0x40) & 0xff, &v, 4);
            _ZN9ActorBase18MarkForDestructionEv(c);
        }
        return 1;
    }
    return func_ov064_02116d1c(c);
}
