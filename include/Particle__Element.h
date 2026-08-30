#ifndef PARTICLE__ELEMENT_H
#define PARTICLE__ELEMENT_H

#include "types.h"

namespace Particle {

/* One live particle in a System. The behavior ABI still passes this object as
 * char*, but all recovered behavior and callback methods agree on this layout. */
struct Element {
    Element *next;              /* 0x00 */
    Element *prev;              /* 0x04 */
    Vector3 basePosition;       /* 0x08 */
    Vector3 offset;             /* 0x14 */
    Vector3 velocity;           /* 0x20 */
    u16 lifetime;               /* 0x2c */
    u16 age;                    /* 0x2e */
};

typedef char Element_size_must_be_0x30[
    sizeof(Element) == 0x30 ? 1 : -1];

} // namespace Particle

#endif
