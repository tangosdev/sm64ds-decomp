//cpp
// @symbol _ZN11ShadowModelC1Ev
/* recovered: real C++ constructor
 *
 * An empty body; the three null stores after the vptr are exactly the member
 * init list -- mat at +0xc and the live-list links prev/next at +0x20/+0x24,
 * the same links ~ShadowModel unlinks. The base call and vptr store are what
 * `ShadowModel : ModelBase` with a declared-not-defined base constructor
 * already means. Recipe documented in full in _ZN5ModelC1Ev.cpp.
 */
#include "ShadowModel.h"

ShadowModel::ShadowModel() : mat(0), prev(0), next(0) {}
