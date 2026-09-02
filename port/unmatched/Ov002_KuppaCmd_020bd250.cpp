/* HOST COPY of src/func_ov002_020bd250.c -- cutscene-object command 9, the one
 * that knocks the player back.
 *
 * WHY IT IS HOSTED NOW, and it is the second time this exact thing happened.
 * hal/intro_kuppa_dispatch.cpp trapped this command with the note "needs
 * Player::Hurt (unhosted)" and left the trap loud on purpose. With the script
 * chain running, the opening reached it and the trap said so by name -- and, as
 * with command 4 one commit earlier, the premise had gone stale rather than the
 * command being exotic. All three of its callees are already in this binary:
 *
 *     _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj    slice_gate10.txt:610
 *                                                (hal/star_flow.cpp:585 and
 *                                                 unmatched/Book_WindowAnd
 *                                                 ReceiverFixes.cpp call it)
 *     Vec3_RotateYAndTranslate                   slice_gate10.txt:191
 *     Particle::System::NewSimple                slice_gate29.txt:83
 *
 * So hosting the command drags in nothing at all, and BOTH trapped commands
 * turned out to be reachable-and-already-available. That is worth recording as
 * a pattern rather than a coincidence: the two were declared unhosted while the
 * intro bit was force-set and nothing could reach them, and the declaration was
 * never re-checked once other lanes landed the subsystems.
 *
 * WHY A HOST COPY RATHER THAN THE SRC LINE. The matched TU reaches both C++
 * callees through LOCAL SHADOW CLASSES --
 *
 *     struct Player { void Hurt(const Vector3 &, unsigned, int, unsigned,
 *                               unsigned, unsigned); };
 *     namespace Particle { struct System {
 *         static void NewSimple(unsigned, int, int, int); }; }
 *
 * -- so MSVC mangles the calls as ?Hurt@Player@@QAEX... (a THISCALL member, so
 * `this` would ride in ecx) and ?NewSimple@System@Particle@@SAXIHHH@Z, while the
 * one real definition of each carries the Itanium name with C linkage. Spelling
 * them extern "C" and passing `self` explicitly is what
 * unmatched/Book_WindowAndReceiverFixes.cpp already does for this same Hurt.
 *
 * The body is the matched source line for line otherwise.
 */
#include "common.h"

extern "C" {

short ReadUnalignedShort(unsigned char *p);
void Vec3_RotateYAndTranslate(Vector3 *out, const Vector3 *base, int angle,
                              const Vector3 *off);
/* The real symbols, spelled the way the port's other callers spell them. A
   reference parameter and a pointer parameter are the same word on this ABI. */
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, const void *src,
                                            unsigned a, int b, unsigned c,
                                            unsigned d, unsigned e);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned id, int x, int y,
                                                    int z);

// PORT_HOST_ABI: matched TU's local shadow makes Player::Hurt a THISCALL member (this would ride in ecx); host copy spells it extern C and passes self explicitly
int func_ov002_020bd250(char *self, unsigned char *p)
{
    Vector3 out1, off1, out2, off2, v;
    *(short *)(self + 0x8e) = ReadUnalignedShort(p);
    off1.x = 0;
    off1.y = 0;
    off1.z = 0x50000;
    Vec3_RotateYAndTranslate(&out1, (Vector3 *)(self + 0x5c),
                             *(short *)(self + 0x8e), &off1);
    *(unsigned char *)(self + 0x6de) = 1;
    *(unsigned char *)(self + 0x6df) = 0;
    v.x = out1.x;
    v.y = out1.y;
    v.z = out1.z;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(self, &v, 0, 0xc000, 1, 0, 1);
    *(int *)(self + 0xa8) = 0x20000;
    *(int *)(self + 0x98) = 0xc000;
    if (*(int *)(self + 8) == 1) {
        off2.x = 0;
        off2.y = 0x64000;
        off2.z = 0x32000;
        Vec3_RotateYAndTranslate(&out2, (Vector3 *)(self + 0x5c),
                                 *(short *)(self + 0x8e), &off2);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, out2.x, out2.y,
                                                       out2.z);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, out2.x, out2.y,
                                                       out2.z);
    }
    return 1;
}

}
