#ifndef DPA_C_H
#define DPA_C_H

#include "types.h"

/* The ROM's RTTI records at 0x0208f304 and 0x0208f360 prove both nested
 * class names and the single-inheritance edge below. Their address-point
 * vtables at 0x0208f3b4 and 0x0208f3c4 each contain two virtual slots.
 *
 * The callback bodies and dispatch ABI prove that their first explicit
 * parameter is a reference-sized Particle system object. `Particle::System`
 * is the established imported spelling, but unlike the callback class names
 * it is not RTTI-backed and remains an inferred exact-source spelling. */
namespace Particle { struct System; }

struct dPa_c {
    struct level_c {
        class callback_c {
        public:
            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

/* HOST ONLY, and it changes no ROM byte: mwccarm never sees this block.
 *
 * simpleCallback_c's fields stop at 0x006 and its sizeof rounds to 0x008, so
 * it has two bytes of tail padding. mwccarm follows the Itanium rule and lets
 * a derived class start its own fields in that padding, which is where the
 * cartridge puts scaleCallback_c::scale: 0x006, with velocity at 0x008 and
 * timer at 0x00a, and the whole class 0xc wide. MSVC will not reuse tail
 * padding, so unpatched it starts scale at sizeof(base) == 8 and the class
 * comes out 0x10 wide. Measured with cl /d1reportSingleClassLayout:
 *
 *     WITHOUT   class scaleCallback_c size(16)   8 | scale  10 | velocity
 *                                                12 | timer
 *     WITH      class scaleCallback_c size(12)   6 | scale   8 | velocity
 *                                                10 | timer
 *
 * Four bytes, four times over, because Particle::SysTracker embeds four
 * scaleCallback_c at 0x7b4, 0x7c4, 0x7d4 and 0x7e4 -- so unpatched the tracker
 * is 0x82c instead of the cartridge's 0x81c and every sub-object from 0x7c0 up
 * sits 4, 8, 12 or 16 bytes late. That matters because the callback bank is
 * reached by RAW OFFSET from src: func_02022864 writes the scale at
 * data_0209ee74 + 0x7ba and passes the callback at + 0x7b4, func_020227ec the
 * pair at + 0x7ca / + 0x7c4, func_02022774 + 0x7da / + 0x7d4, and
 * Particle::System::NewUnkCallback818 takes the one at + 0x818. Off by four,
 * those land on a neighbouring sub-object.
 *
 * pack(2) is not packing here, it caps alignment. A class's sizeof is its data
 * size rounded up to its alignment, so capping simpleCallback_c's alignment at
 * two makes sizeof equal its data size, 6, and MSVC then starts every derived
 * class's own fields on the cartridge's offset. Every member of the class is
 * already on a correctly aligned offset, so nothing inside it moves; the same
 * lever port/unmatched/dBgActor_c_TailPad_HostAbi.h uses one class up.
 *
 * The visible cost is that simpleCallback_c and the five derived classes that
 * add no field of their own report sizeof 6 on the host instead of 8. Nothing
 * depends on that: the only place in the tree that embeds any of these classes
 * is Particle::SysTracker, every one of those members is followed by a
 * 4-aligned field, and the offset asserts under the class list pin all thirty
 * of them plus the whole-object size. */
#ifdef SM64DS_PLATFORM_PC
#pragma pack(push, 2)
#endif
        class simpleCallback_c : public callback_c {
        public:
            s16 value;              /* 0x004 */
            simpleCallback_c();

            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };
#ifdef SM64DS_PLATFORM_PC
#pragma pack(pop)
#endif

        class scaleCallback_c : public simpleCallback_c {
        public:
            /* Descriptive names; the offsets and uses are body-proven. */
            s16 scale;              /* 0x006 - reuses base tail padding */
            s16 velocity;           /* 0x008 */
            u8 timer;               /* 0x00a */
            scaleCallback_c();

            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class edStarKiraCallback_c : public simpleCallback_c {
        public:
            struct trackingRecordStorage_c {
                u32 words[3];
            };

            /* 64 independently initialized 0xc-byte tracking records. Their
             * historical element type name is not present in ROM RTTI. */
            trackingRecordStorage_c trackingRecords[0x40]; /* 0x008 */
            s32 unk308;                                   /* 0x308 */
            s16 unk30c;                                   /* 0x30c */

            edStarKiraCallback_c();

            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class checkYoganCallback_c : public simpleCallback_c {
        public:
            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class fitWaterSimpleCallback_c : public simpleCallback_c {
        public:
            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class fitWaterCallback_c : public callback_c {
        public:
            /* Descriptive name; the offset and water-height use are body-proven. */
            s32 waterOffset;         /* 0x004 */

            fitWaterCallback_c() : waterOffset(0x3000) {}

            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class checkWaterCallback_c : public callback_c {
        public:
            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class bubbleCallback_c : public simpleCallback_c {
        public:
            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class splashCallback_c : public simpleCallback_c {
        public:
            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class clipCallback_c : public callback_c {
        public:
            /* Zero expires every active particle; nonzero clips them. */
            u8 enabled;             /* 0x004 */

            clipCallback_c() : enabled(1) {}

            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

        class cleanParticleCallback_c : public callback_c {
        public:
            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool done);
        };

        class checkWaterRippleCallback_c : public callback_c {
        public:
            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };
    };
};

#ifdef SM64DS_PLATFORM_PC
/* HOST layout, pinned to the cartridge's own offsets. These are the classes
   the pack(2) block above exists for, and a static_assert is a check no size
   census can miss: MSVC refuses the build the moment one of them moves. The
   ROM offsets come from the bodies that write the fields by raw offset off
   data_0209ee74 -- src/func_02022864.c writes + 0x7ba and + 0x7bc into the
   scaleCallback_c at + 0x7b4, so scale is + 0x006 and velocity + 0x008. */
ROM_OFFSET_ASSERT(dPa_c::level_c::simpleCallback_c, value, 0x004);
ROM_OFFSET_ASSERT(dPa_c::level_c::scaleCallback_c, scale, 0x006);
ROM_OFFSET_ASSERT(dPa_c::level_c::scaleCallback_c, velocity, 0x008);
ROM_OFFSET_ASSERT(dPa_c::level_c::scaleCallback_c, timer, 0x00a);
ROM_OFFSET_ASSERT(dPa_c::level_c::edStarKiraCallback_c, trackingRecords, 0x008);
ROM_OFFSET_ASSERT(dPa_c::level_c::edStarKiraCallback_c, unk308, 0x308);
ROM_OFFSET_ASSERT(dPa_c::level_c::edStarKiraCallback_c, unk30c, 0x30c);
ROM_OFFSET_ASSERT(dPa_c::level_c::fitWaterCallback_c, waterOffset, 0x004);
ROM_OFFSET_ASSERT(dPa_c::level_c::clipCallback_c, enabled, 0x004);
static_assert(sizeof(dPa_c::level_c::callback_c) == 0x4,
              "dPa_c::level_c::callback_c is not the ROM's 0x4 on the host");
static_assert(sizeof(dPa_c::level_c::scaleCallback_c) == 0xc,
              "dPa_c::level_c::scaleCallback_c is not the ROM's 0xc on the host");
static_assert(sizeof(dPa_c::level_c::edStarKiraCallback_c) == 0x310,
              "dPa_c::level_c::edStarKiraCallback_c is not the ROM's 0x310 on the host");
static_assert(sizeof(dPa_c::level_c::fitWaterCallback_c) == 0x8,
              "dPa_c::level_c::fitWaterCallback_c is not the ROM's 0x8 on the host");
static_assert(sizeof(dPa_c::level_c::clipCallback_c) == 0x8,
              "dPa_c::level_c::clipCallback_c is not the ROM's 0x8 on the host");
/* The six classes NOT listed above -- simpleCallback_c itself and the five
   that add no field of their own (checkYoganCallback_c, fitWaterSimpleCallback_c,
   bubbleCallback_c, splashCallback_c and the base's own checkWaterCallback_c
   pair) -- are 6 rather than 8 on the host by design, because dropping that
   tail is what puts scaleCallback_c::scale on 0x006. Their PLACEMENTS are what
   matter and include/Particle__SysTracker.h pins every one of them. */
#endif

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dPa_callback_c_size_must_be_0x4[
    sizeof(dPa_c::level_c::callback_c) == 0x4 ? 1 : -1];
typedef char dPa_simpleCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::simpleCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_scaleCallback_c_size_must_be_0xc[
    sizeof(dPa_c::level_c::scaleCallback_c) == 0xc ? 1 : -1];
typedef char dPa_edStarKiraCallback_c_size_must_be_0x310[
    sizeof(dPa_c::level_c::edStarKiraCallback_c) == 0x310 ? 1 : -1];
typedef char dPa_checkYoganCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::checkYoganCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_fitWaterSimpleCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::fitWaterSimpleCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_fitWaterCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::fitWaterCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_checkWaterCallback_c_size_must_be_0x4[
    sizeof(dPa_c::level_c::checkWaterCallback_c) == 0x4 ? 1 : -1];
typedef char dPa_bubbleCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::bubbleCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_splashCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::splashCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_clipCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::clipCallback_c) == 0x8 ? 1 : -1];
typedef char dPa_cleanParticleCallback_c_size_must_be_0x4[
    sizeof(dPa_c::level_c::cleanParticleCallback_c) == 0x4 ? 1 : -1];
typedef char dPa_checkWaterRippleCallback_c_size_must_be_0x4[
    sizeof(dPa_c::level_c::checkWaterRippleCallback_c) == 0x4 ? 1 : -1];
#endif

#endif
