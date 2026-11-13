/* The Trampoline / Trampoline2 Mario pools' module-level entry points.
 *
 * The scenes (d_s_mg_trampoline.cpp, d_s_mg_trampoline2.cpp) own the pools
 * and call these by the namespace names in symbols.txt; d_mg_trmpln2_mario.cpp
 * defines them. The Marios are dMgTrmpln3DMario_c (Trampoline, pool of 4) and
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

/* These entry points are C++ namespace functions (_ZN11ns_...), declared
 * next to their callers (decl_common.h and the two scene TUs). An extern "C"
 * declaration here would hide those usings and emit the unmangled name. */


#endif
