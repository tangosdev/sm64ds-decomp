/* Particle::SysTracker, as seen by SysTracker::Initialise and
 * SysTracker::Update -- the head of the object only.
 *
 * THIS IS THE THIRD DECLARATION OF THIS CLASS. include/Particle.h models the
 * whole 0x81c-byte object (spelled `Particle` there for historical reasons)
 * and include/Stage.h carries a fourth-wall copy of that same list as
 * Particle::SysTracker, embedded at Stage+0x50. Merging the three is its own
 * change with its own blast radius -- Stage.h's copy has to stay a local
 * declaration because Stage embeds the object by value and needs the
 * declared-never-defined destructor, and one file that opens
 * `namespace Particle { ... }` cannot include a header declaring a struct of
 * that name. What CAN be shared without moving a byte is the field names,
 * and that is what this file now does: the three fields below are spelled
 * exactly as include/Particle.h and include/Stage.h spell them, so a reader
 * who has met one has met all three.
 *
 * Only the head is declared here because only the head is reached: the two
 * including files touch 0x000, 0x004 and 0x008 and nothing else. Declaring
 * the 0x747-byte tail would add a claim neither of them evidences.
 *
 * Per-field evidence: notes/system-provenance.md. In brief --
 *   0x000 mResourceFile: Initialise stores either data_02075f14 in place or a
 *         heap buffer it fills with DecompressLZ16, then hands it to the
 *         manager via func_0204a17c; ~SysTracker frees it only in the second
 *         case.
 *   0x004 mManager: the Particle::Manager. Initialise allocates it with
 *         func_0204a4c8 and pokes 0x8000 into its +0x30; Update passes it
 *         straight to func_02049f58.
 *   0x008 mContents: Particle::SysTracker::Contents, a real nested class --
 *         System::FromUniqueID calls
 *         Particle::SysTracker::Contents::FindData(this + 8, uniqueID), and
 *         Update runs func_02021bec on the same address.
 *
 * Field NAMES cannot change codegen; the types below are the ones
 * include/Particle.h already carries, and both including functions still
 * reproduce byte-exact under 2004/b56. */
#ifndef PARTICLE__SYSTRACKER_H
#define PARTICLE__SYSTRACKER_H
#include "types.h"

struct Particle__SysTracker {
    void *mResourceFile;    /* 0x000 */
    void *mManager;         /* 0x004 */
    u8   mContents;         /* 0x008 */
};

#endif
