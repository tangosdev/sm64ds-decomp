#ifndef DPA_C_H
#define DPA_C_H

#include "types.h"

/* The ROM's RTTI records at 0x0208f304 and 0x0208f360 prove both nested
 * class names and the single-inheritance edge below. Their address-point
 * vtables at 0x0208f3b4 and 0x0208f3c4 each contain two virtual slots.
 *
 * The imported Particle::System parameter spelling is not RTTI-backed, so the
 * virtual parameters stay opaque here. Do not bind their method symbols from
 * this declaration until each body has proved its parameter types. */
struct dPa_c {
    struct level_c {
        class callback_c {
        public:
            virtual void SpawnParticles(void* system);
            virtual int OnUpdate(void* system, bool active);
        };

        class simpleCallback_c : public callback_c {
        public:
            s16 value;              /* 0x004 */
            simpleCallback_c();

            virtual void SpawnParticles(void* system);
            virtual int OnUpdate(void* system, bool active);
        };
    };
};

typedef char dPa_callback_c_size_must_be_0x4[
    sizeof(dPa_c::level_c::callback_c) == 0x4 ? 1 : -1];
typedef char dPa_simpleCallback_c_size_must_be_0x8[
    sizeof(dPa_c::level_c::simpleCallback_c) == 0x8 ? 1 : -1];

#endif
