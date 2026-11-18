/* Model-system initialisation flag.
 * data_0208e738 is the word d_ext_model's destructor path and d_a_kpa3_bg
 * test for zero before tearing the model cache down.
 * Consumers: src/engine/model/d_ext_model.cpp, src/game/actors/d_a_kpa3_bg.cpp.
 * ROM 0x0208e738..0x0208e73c, 4 bytes, 1 configured symbol.
 *
 * Order: mwcc emits one .data section per initialized object and the linker
 * concatenates them in declaration order, so the definitions below are in the
 * order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208e738: 4 bytes. */
int data_0208e738 = 0x00000001;
