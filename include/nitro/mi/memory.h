#ifndef NITRO_MI_MEMORY_H
#define NITRO_MI_MEMORY_H

/* NitroSDK's mi_memory.c (nitro/mi/memory.h), defined in
 * src/engine/mi/MI_Memory.c, with the SDK's signatures. The parameters stay
 * unnamed, because a named parameter in a declaration costs every C++
 * includer a compiler @NNN number. The SDK spells the sizes u32 and the fill
 * values u16/u32/u8; the plain types here are the same ones, so that the C
 * files that typedef u32 themselves can include this without types.h. */

#ifdef __cplusplus
extern "C" {
#endif

void MIi_CpuClear16(unsigned short, void *, unsigned int);
void MIi_CpuCopy16(const void *, void *, unsigned int);
void MIi_CpuClear32(unsigned int, void *, unsigned int);
void MIi_CpuCopy32(const void *, void *, unsigned int);
void MIi_CpuClearFast(unsigned int, void *, unsigned int);
void MIi_CpuCopyFast(const void *, void *, unsigned int);
void MI_Copy32B(const void *, void *);
void MI_Copy36B(const void *, void *);
void MI_Copy48B(const void *, void *);
void MI_CpuFill8(void *, unsigned char, unsigned int);
void MI_CpuCopy8(const void *, void *, unsigned int);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_MI_MEMORY_H */
