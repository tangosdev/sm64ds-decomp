#ifndef SM64DS_G2_H
#define SM64DS_G2_H

/* G2 / G2S -- the main and sub 2D engines' BG base-address getters (Nitro SDK
 * namespaces, not classes), defined in src/engine/gx/G2.cpp. Every getter
 * returns the VRAM address as a pointer, null where the layer has no such
 * base in the current mode; callers cast it to the element type they copy.
 * The main engine's BG0, BG1 and BG3
 * character getters keep C-linkage address names (func_02054efc,
 * func_02054ea8, func_02054d88) and are not declared here.
 *
 * decl_common.h no longer declares a data array named G2, which used to hide
 * this namespace in every translation unit that included it. */
namespace G2 {
void *GetBG0ScrPtr(void);
void *GetBG1ScrPtr(void);
void *GetBG2ScrPtr(void);
void *GetBG3ScrPtr(void);
void *GetBG2CharPtr(void);
}

namespace G2S {
void *GetBG0ScrPtr(void);
void *GetBG1ScrPtr(void);
void *GetBG2ScrPtr(void);
void *GetBG3ScrPtr(void);
void *GetBG0CharPtr(void);
void *GetBG1CharPtr(void);
void *GetBG2CharPtr(void);
void *GetBG3CharPtr(void);
}

#endif
