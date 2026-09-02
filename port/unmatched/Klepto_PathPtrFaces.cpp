/* THE KLEPTO SEAT'S PathPtr MEMBER-CALL CLOSURE (actor 239, ov062).
 *
 * src/func_ov062_0211c2f4.cpp is a MATCHED handler (Klepto's e15c carried tick)
 * that declares its own local
 *
 *     struct PathPtr { int a; int b; PathPtr(); void FromID(u32 id);
 *                      void GetNode(Vector3 &out, u32 idx) const; };
 *
 * and constructs one on the stack, then calls path.FromID(...) and
 * path.GetNode(...). Under MSVC those are __thiscall members and mangle to
 *   ??0PathPtr@@QAE@XZ                       (the default constructor)
 *   ?FromID@PathPtr@@QAEXI@Z                 (FromID)
 *   ?GetNode@PathPtr@@QBEXAAUVector3@@I@Z    (const, void return)
 * The flat, matched bodies in this build carry the Itanium names
 * _ZN7PathPtrC1Ev / _ZN7PathPtr6FromIDEj / _ZNK7PathPtr7GetNodeER7Vector3j, so
 * nothing answers the member decorations until they are wired here.
 *
 * WHY THIS FILE EXISTS (the bug it fixes): the first Klepto seat bridged the
 * constructor and FromID with `#pragma comment(linker, "/alternatename:...")`,
 * which points the member name at the FLAT body. That is WRONG for exactly the
 * reason unmatched/ToxBox_ShadowFaces.cpp spells out: an MSVC member is
 * __thiscall and takes its receiver in ECX, while the flat body is cdecl and
 * reads its first argument off the stack. Aliasing one onto the other delivers
 * the receiver in the wrong place -- for FromID the "self" slot is read as the
 * id value and `*(int*)self = ...` stores through a near-null pointer, the
 * c0000005 the level-16 Klepto took on its first pathed frame (the same
 * 2026-08-07 "door-open crash" failure mode). The remedy is a REAL FORWARDER,
 * a genuine __thiscall member that MSVC decorates correctly and whose body
 * calls the flat cdecl symbol with an explicit self -- the ToxBox / RacingPenguin
 * PathPtr::GetNode recipe (that GetNode forwarder already lives in
 * unmatched/RacingPenguin_ShadowFaces.cpp and answers c2f4's GetNode call, so
 * only the constructor and FromID are added here).
 *
 * include/PathPtr.h is deliberately NOT included: it declares the real class,
 * and having both in one TU is a redefinition (the ToxBox lesson). The shadow
 * is spelled exactly as c2f4 spells it so the decorations match. Nothing here
 * changes what code runs; it only puts the receiver where __thiscall left it.
 */
struct PathPtr {
    int a;
    int b;
    PathPtr();
    void FromID(unsigned int id);
};

extern "C" void _ZN7PathPtrC1Ev(void *self);
extern "C" void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);

/* PORT_HOST_ABI: __thiscall receiver in ECX forwarded to the cdecl flat name. */
PathPtr::PathPtr()
{
    _ZN7PathPtrC1Ev(this);
}

/* PORT_HOST_ABI: __thiscall receiver in ECX forwarded to the cdecl flat name. */
void PathPtr::FromID(unsigned int id)
{
    _ZN7PathPtr6FromIDEj(this, id);
}
