#ifndef PARTICLE__SIMPLECALLBACK_H
#define PARTICLE__SIMPLECALLBACK_H
#include "dPa_c.h"

/* Compatibility spelling for the one method still enrolled under its imported
 * Particle::SimpleCallback assertion. ROM RTTI proves the class declaration in
 * dPa_c.h; remove this alias when that method's parameter name is verified. */
typedef dPa_c::level_c::simpleCallback_c Particle__SimpleCallback;

#endif
