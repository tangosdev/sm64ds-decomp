#ifndef NITRO_OS_PROTECTIONUNIT_H
#define NITRO_OS_PROTECTIONUNIT_H

/*
 * The protection unit switch of the Nitro SDK's os_protectionUnit.c, under
 * the SDK's own names (pret/pokediamond
 * arm9/lib/NitroSDK/include/OS_protectionUnit.h). The bodies are hand-written
 * asm, in src/engine/cp15/CP15_ProtectionUnit.cpp: each sets or clears bit 0
 * of the CP15 control register c1.
 */

#ifdef __cplusplus
extern "C" {
#endif

void OS_EnableProtectionUnit(void);
void OS_DisableProtectionUnit(void);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_PROTECTIONUNIT_H */
