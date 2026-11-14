//cpp
/* OS exceptions (the Nitro SDK's os_exception.c), arm9 .text
 * 0x020593f4..0x02059624.
 *
 * pret/pokediamond's NitroSDK OS_exception.c defines, in this order:
 * OS_InitException, OSi_ExceptionHandler (asm), OSi_GetAndDisplayContext
 * (asm), OSi_SetExContext (asm) and OSi_DisplayExContext. This older SDK
 * also has OS_SetUserExceptionHandler after OS_InitException.
 *
 * mwccarm emits an asm function where it stands and defers the C bodies,
 * which come out in reverse. So the three asm functions open the run, and
 * OSi_DisplayExContext, OS_SetUserExceptionHandler and OS_InitException
 * follow.
 *
 * The three asm functions read their frame through ip and switch processor
 * modes; the SDK writes them as asm, and they stay asm here.
 */
#include "types.h"
#include "nitro/hw/mmap_tcm.h"
#include "nitro/os/protectionUnit.h"
#include "nitro/os/exception.h"
#include "nitro/os/emulator.h"

/* The debugger's exception vector slot in the shared work area. A debugger
 * that owns exceptions leaves a main-RAM handler address there. */
#define HW_EXCP_VECTOR_BUF_FOR_DEBUGGER 0x027ffd9c



extern OSExceptionHandler data_020a63a0;
 /* OSi_UserExceptionHandler */
extern void *data_020a63a4;
              /* OSi_UserExceptionHandlerArg */
extern u32 data_020a63a8;
                /* OSi_DebuggerHandler */
extern u32 data_020a63ac;
                /* OSi_ExContext */

void OSi_ExceptionHandler(void);
void OSi_GetAndDisplayContext(void);
void OSi_SetExContext(void);
void OSi_DisplayExContext(void);

void OS_InitException(void) {
    u32 debugger = *(u32 *)HW_EXCP_VECTOR_BUF_FOR_DEBUGGER;
    if (debugger >= 0x2600000 && debugger < 0x2800000)
        data_020a63a8 = debugger;
    else
        data_020a63a8 = 0;
    if (data_020a63a8 == 0 || (OS_GetConsoleType() & OS_CONSOLE_ISDEBUGGER) == 0) {
        /* HW_EXCP_VECTOR_BUF is in DTCM, so the store goes through the
         * linker's DTCM base: a pool word of base + 0x3000, then #0xfdc. */
        *(void **)HW_EXCP_VECTOR_BUF_FOR_DEBUGGER = (void *)OSi_ExceptionHandler;
        *(void **)HW_EXCP_VECTOR_BUF = (void *)OSi_ExceptionHandler;
    }
    data_020a63a0 = 0;
}

void OS_SetUserExceptionHandler(OSExceptionHandler handler, void *arg) {
    data_020a63a0 = handler;
    data_020a63a4 = arg;
}

/* Run the debugger's handler if there is one. Otherwise save r0-r3, sp and
 * lr below the end of ITCM, record the context on an abort (mode 0x17) or an
 * undefined instruction (0x1b), and halt. */
asm void OSi_ExceptionHandler(void) {
    ldr ip, =data_020a63a8
    ldr ip, [ip]
    cmp ip, #0
    movne lr, pc
    bxne ip
    ldr ip, =0x2000000
    stmdb ip!, {r0, r1, r2, r3, sp, lr}
    and r0, sp, #1
    mov sp, ip
    mrs r1, cpsr
    and r1, r1, #0x1f
    teq r1, #0x17
    bne Lnot_abt
    bl OSi_GetAndDisplayContext
    b Lafter
Lnot_abt:
    teq r1, #0x1b
    bne Lafter
    bl OSi_GetAndDisplayContext
Lafter:
    ldr ip, =data_020a63a8
    ldr ip, [ip]
    cmp ip, #0
Lspin1:
    beq Lspin1
Lspin2:
    mov r0, r0
    b Lspin2
    ldmia sp!, {r0, r1, r2, r3, ip, lr}
    mov sp, ip
    bx lr
}

/* Pushes lr alone and calls out with no stack pad, which mwccarm never
 * emits for C: the SDK writes this one as asm. */
asm void OSi_GetAndDisplayContext(void) {
    stmdb sp!, {lr}
    bl OSi_SetExContext
    bl OSi_DisplayExContext
    ldmia sp!, {lr}
    bx lr
}

/* Copy the faulting context into OSi_ExContext: the registers saved at ip,
 * r4-r11, the CP15 and exception-mode words, and the banked sp, lr and spsr
 * of the mode that faulted. */
asm void OSi_SetExContext(void) {
    ldr r1, =data_020a63ac
    mrs r2, cpsr
    str r2, [r1, #0x74]
    str r0, [r1, #0x6c]
    ldr r0, [ip]
    str r0, [r1, #4]
    ldr r0, [ip, #4]
    str r0, [r1, #8]
    ldr r0, [ip, #8]
    str r0, [r1, #0xc]
    ldr r0, [ip, #0xc]
    str r0, [r1, #0x10]
    ldr r2, [ip, #0x10]
    bic r2, r2, #1
    add r0, r1, #0x14
    stmia r0, {r4, r5, r6, r7, r8, r9, r10, r11}
    str ip, [r1, #0x70]
    ldr r0, [r2]
    str r0, [r1, #0x64]
    ldr r3, [r2, #4]
    str r3, [r1]
    ldr r0, [r2, #8]
    str r0, [r1, #0x34]
    ldr r0, [r2, #0xc]
    str r0, [r1, #0x40]
    mrs r0, cpsr
    orr r3, r3, #0x80
    bic r3, r3, #0x20
    msr cpsr_fsxc, r3
    str sp, [r1, #0x38]
    str lr, [r1, #0x3c]
    mrs r2, spsr
    str r2, [r1, #0x7c]
    msr cpsr_fsxc, r0
    bx lr
}

/* Call the user's exception handler in system mode (0x9f) on the current
 * stack, with the protection unit on. */
void OSi_DisplayExContext(void) {
    if (data_020a63a0 == 0)
        return;
    asm {
        mov r0, sp
        ldr r1, =0x9f
        msr cpsr_fsxc, r1
        mov sp, r0
    }
    OS_EnableProtectionUnit();
    data_020a63a0((u32)&data_020a63ac, data_020a63a4);
    OS_DisableProtectionUnit();
}
