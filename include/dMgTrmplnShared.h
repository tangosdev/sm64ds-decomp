/* The Trampoline / Trampoline2 Mario pools' module-level entry points.
 *
 * The scenes (d_s_mg_trampoline.cpp, d_s_mg_trampoline2.cpp) own the pools
 * and call these by their unmangled ROM names; d_mg_trmpln2_mario.cpp defines
 * them. The Marios are dMgTrmpln3DMario_c (Trampoline, pool of 4) and
 * dMgTrmpln2Mario_c (Trampoline2, pool of 5); the pool base is walked by byte
 * offset, so the entry points take it as void* / unsigned char*. The pool's
 * init / reset / draw family (and Trmp2Mario_SetTuning) is declared in
 * decl_common.h.
 *
 * Parameters stay unnamed on purpose: named ones renumber compiler @NNN
 * labels in this header's C++ consumers (the same effect decl_common.h
 * documents for func_ov007_020c3df4, where it refused ten actor TUs). */
#ifndef DMGTRMPLNSHARED_H
#define DMGTRMPLNSHARED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Trampoline2 (dMgTrmpln2Mario_c). */
int Trmp2Mario_Hide(void);
void Trmp2Mario_EnterAll(void);
void Trmp2Mario_MoveToStart(void);
void Trmp2Mario_PlaceAtStart(void);
void Trmp2Mario_UpdateAll(void);

/* Trampoline (dMgTrmpln3DMario_c). */
void Trmp3DMario_EnterWalkIn(char *);
int Trmp3DMario_IsOffstage(int *);
int Trmp3DMario_CountFinished(void);
void Trmp3DMario_StartAll(void);
void Trmp3DMario_UpdateAll(void);
void Trmp3DMario_SetTuning(int);
void Trmp3DMario_SetSpawnHeight(int);

#ifdef __cplusplus
}
#endif

#endif
