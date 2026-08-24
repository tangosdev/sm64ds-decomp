//cpp
// NONMATCHING: hand-written asm, not a C decompilation. Whole-function mnemonic
// transcription of compiled-C-shaped code (stmdb prolog, plain ldr/str/bl body, ldmia
// epilog - no C-inexpressible instruction), so the asm-primitive policy does not apply;
// does NOT count as matched. Reverts to a draft until someone reproduces the bytes from
// real C++ (fBase_c::fBase_c() - the inheritance chain and callees are already known).
//
// SYMBOL DISCREPANCY, DELIBERATELY NOT RESOLVED HERE: this file is named for C2 and
// config/arm9/symbols.txt attests only _ZN7fBase_cC2Ev at 0x02043dec, but the @symbol
// and the definition below say C1. src/_ZN8dActor_cC2Ev.cpp calls C2; src_tu's
// Actor.cpp calls C1. Nothing catches it because the NONMATCHING banner keeps the file
// out of eligible.py and out of config/arm9/delinks.txt. Whoever reproduces this body
// in real C++ has to settle the name at the same time.
// @symbol _ZN7fBase_cC1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "fBase_c.h"
extern "C" {
void _ZN7fBase_c9SceneNodeC1Ev(void);
int func_0203b438(void);
int func_02043810(void);
extern int data_020a4bb8;
}

extern "C" asm void* _ZN7fBase_cC1Ev(void* self) {
    // r4 = this, r5 = &sceneNode (this + 0x14).
    stmdb sp!, {r4, r5, lr}
    sub sp, sp, #4
    mov r4, r0
    ldr r1, =data_02099edc          // fBase_c vtable
    add r5, r4, #0x14
    mov r0, r5
    str r1, [r4]
    bl _ZN7fBase_c9SceneNodeC1Ev
    str r4, [r5, #0x10]             // sceneNode.owner = this
    // behavNode (0x28) and renderNode (0x38): zeroed, owner = this.
    add r0, r5, #0x14
    mov r2, #0
    str r2, [r0]
    str r2, [r0, #4]
    str r4, [r0, #8]
    strh r2, [r0, #0xc]
    strh r2, [r0, #0xe]
    add r0, r5, #0x24
    str r2, [r0]
    str r2, [r0, #4]
    str r4, [r0, #8]
    strh r2, [r0, #0xc]
    strh r2, [r0, #0xe]
    // uniqueID = post-incremented global; param1, actorID and 0x12 from the
    // spawn-parameter block the caller parked in data_020a4b48..data_020a4b6c.
    ldr r1, =data_02099e70
    ldr r0, =data_020a4b60
    ldr r2, [r1]
    str r2, [r4, #4]
    ldr r3, [r1]
    ldr r2, [r0]
    add r0, r3, #1
    str r0, [r1]
    str r2, [r4, #8]
    ldr r0, =data_020a4b54
    ldr r2, =data_020a4b64
    ldrh r1, [r0]
    ldr r0, =data_020a4b48
    strh r1, [r4, #0xc]
    ldrb r3, [r0]
    ldr r0, =data_020a4b6c
    mov r1, r5
    strb r3, [r4, #0x12]
    ldr r2, [r2]
    bl func_0203b438
    // Seed both process nodes' priorities from the actor info table entry.
    ldr r0, =data_020a4bb8
    ldrh r1, [r4, #0xc]
    ldr r0, [r0]
    add r2, r4, #0x28
    ldr ip, [r0, r1, lsl #2]
    add r1, r4, #0x38
    ldrh r3, [ip, #4]
    mov r0, r4
    strh r3, [r2, #0xc]
    strh r3, [r2, #0xe]
    ldrh r2, [ip, #6]
    strh r2, [r1, #0xc]
    strh r2, [r1, #0xe]
    bl func_02043810                // returns the parent object, or null
    cmp r0, #0
    beq Ldone
    // Inherit the parent's pause bits: parent 1|2 -> own 2, parent 4|8 -> own 8.
    ldrb r2, [r0, #0x13]
    ands r1, r2, #1
    bne Lset1
    ands r1, r2, #2
    beq Lcheck4
Lset1:
    add r2, r4, #0x13
    ldrb r1, [r2]
    orr r1, r1, #2
    strb r1, [r2]
Lcheck4:
    ldrb r1, [r0, #0x13]
    ands r0, r1, #4
    bne Lset2
    ands r0, r1, #8
    beq Ldone
Lset2:
    add r1, r4, #0x13
    ldrb r0, [r1]
    orr r0, r0, #8
    strb r0, [r1]
Ldone:
    mov r0, r4
    add sp, sp, #4
    ldmia sp!, {r4, r5, lr}
    bx lr
}
