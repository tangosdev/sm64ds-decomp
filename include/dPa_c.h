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

        class simpleCallback_c : public callback_c {
        public:
            s16 value;              /* 0x004 */
            simpleCallback_c();

            virtual void SpawnParticles(Particle::System& system);
            virtual int OnUpdate(Particle::System& system, bool active);
        };

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
