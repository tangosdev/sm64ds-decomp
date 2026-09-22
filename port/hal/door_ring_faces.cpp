// The door ring's Camera faces, split from hal/method_faces.cpp because the
// defining TUs (src/_ZN6Camera10LookAtExitER8dActor_c.cpp,
// src/_ZN6Camera14GoBehindPlayerEj.cpp) declare a LOCAL Camera mirror rather
// than include/Camera.h, and this TU must mangle against the same shape
// without colliding with the shared header.
//
// Why these exist at all: the pair used to be direct link aliases in
// cxx_aliases.cpp, which bound cdecl Itanium-name callers straight onto
// __thiscall method bodies (and, for ChangeState, a __thiscall caller onto a
// cdecl body). Garbage `this` plus a 4-byte ESP imbalance per one-arg call;
// the RET-to-the-door-object crash of 2026-08-07. See the note in
// cxx_aliases.cpp's gate-22 block.

struct dActor_c;

struct Camera {
    struct State;
    void LookAtExit(dActor_c &actor);
    void ChangeState(State *st);
    void GoBehindPlayer(unsigned int j);
};

extern "C" {
void _ZN6Camera11ChangeStateEPNS_5StateE(void *self, void *st);

/* cdecl entry faces onto the __thiscall method definitions */
void _ZN6Camera14GoBehindPlayerEj(void *self, unsigned int j)
{ ((Camera *)self)->GoBehindPlayer(j); }
void _ZN6Camera10LookAtExitER8dActor_c(void *self, void *actor)
{ ((Camera *)self)->LookAtExit(*(dActor_c *)actor); }
}  /* extern "C" */

/* the reverse shim: method-form callers (LookAtExit's own body enters the
   exit-look state with this->ChangeState(...)) onto the C-linkage dispatcher
   the port hosts in hal/camera_bridges.cpp. */
void Camera::ChangeState(State *st)
{ _ZN6Camera11ChangeStateEPNS_5StateE(this, st); }
