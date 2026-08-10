/* port_msvc_compat.h -- MSVC-keyword shims for the non-MSVC (Linux) port build.
 *
 * The HAL and a couple of generated TUs spell MSVC calling-convention keywords
 * (__fastcall on the synthetic vtable-slot shims so `this` lands in the first
 * integer register exactly as __thiscall does; __thiscall/__stdcall on a few raw
 * function-pointer casts). GCC/Clang do not define those keywords, so on Linux
 * we map them to the equivalent function attributes.
 *
 * This header is FORCE-INCLUDED on the non-MSVC build only (CMake `-include`), so
 * every TU sees the shims without editing its include list. On MSVC it is never
 * included and the keywords keep their real meaning; the Windows build is
 * byte-for-byte unchanged.
 *
 * On 32-bit x86 the attributes are real conventions (fastcall = ecx/edx,
 * thiscall = ecx, stdcall = callee-cleanup). On any other arch there is no such
 * convention and they degrade to nothing -- the calls still happen, just with
 * the platform default convention, which is correct for a pure-C++ member call.
 */
#ifndef PORT_MSVC_COMPAT_H
#define PORT_MSVC_COMPAT_H

#ifndef _MSC_VER
#if defined(__GNUC__) && defined(__i386__)
#define __fastcall __attribute__((fastcall))
#define __thiscall __attribute__((thiscall))
#define __stdcall  __attribute__((stdcall))
#else
#define __fastcall
#define __thiscall
#define __stdcall
#endif
#endif /* !_MSC_VER */

/* ---- grouped-section placement -------------------------------------------
 * Several HAL TUs split one ROM struct across its ROM-named symbols and place
 * them in ONE section, in declaration order, so the linker lays them out
 * contiguously (each ROM offset equals the symbol's own size). MSVC spells this
 * __pragma(section(".grp$NNNN")) + __declspec(allocate(".grp$NNNN")); the $-suffix
 * sorts the fragments in name order within ".grp".
 *
 * PORT_GROUPED_DECL(fullsec, basesec): fullsec is the MSVC "$"-suffixed name,
 * basesec is the GCC section (the part before the '$', passed explicitly since a
 * macro cannot strip it). GCC merges same-named sections and the build's
 * -fno-toplevel-reorder preserves declaration order, reproducing the layout. Each
 * TU passes its OWN basesec so groups never interleave. Verify with *_layout_check.
 */
#ifdef _MSC_VER
#define PORT_GROUPED_DECL_A(fullsec, basesec, algn) \
    __pragma(section(fullsec, read, write)) \
    __declspec(allocate(fullsec)) __declspec(align(algn))
#else
#define PORT_GROUPED_DECL_A(fullsec, basesec, algn) \
    __attribute__((section(basesec), aligned(algn), used))
#endif
/* default alignment 4 (the common case) */
#define PORT_GROUPED_DECL(fullsec, basesec) \
    PORT_GROUPED_DECL_A(fullsec, basesec, 4)

#endif /* PORT_MSVC_COMPAT_H */
