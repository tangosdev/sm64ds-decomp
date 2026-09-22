// RUN LINKW WAVE 5 (lane w5-c): two Actor method spellings the hostgen'd
// func_ov070_0211f0a4 (FlyGuy's poof-coins-kill state) dispatches under its
// own local shadow declarations:
//
//     ?SmallPoofDust@dActor_c@@QAEXXZ
//     ?SpawnCoins@dActor_c@@QAEXABUVector3@@IHF@Z
//
// (dumpbin of the compiled TU; KillAndTrackInDeathTable's spelling is already
// defined by the matched method TU.) Both bodies exist as cdecl C functions
// (_ZN8dActor_c13SmallPoofDustEv / _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs,
// in today's map), so an /alternatename would be ABI-wrong -- __thiscall
// wanted, cdecl defined. These are real forwarding faces.
//
// A SHADOW STRUCT, not include/Actor.h: the real header declares
// SmallPoofDust but NOT SpawnCoins with this signature, and a method
// definition requires an in-class declaration. The shadow reproduces the
// hostgen'd TU's own declarations exactly, so the emitted decorations match
// by construction; this TU must never include a real Actor/Vector3 header
// (one definition per TU).
struct Vector3;
struct Actor {
    void SmallPoofDust();
    void SpawnCoins(Vector3 const &, unsigned int, int, short);
};

extern "C" {
void _ZN8dActor_c13SmallPoofDustEv(void *self);
void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const void *pos,
                                                 unsigned n, int speed,
                                                 short arg);
}

void Actor::SmallPoofDust()
{ _ZN8dActor_c13SmallPoofDustEv(this); }

void Actor::SpawnCoins(Vector3 const &pos, unsigned int n, int speed,
                       short arg)
{ _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, &pos, n, speed, arg); }
