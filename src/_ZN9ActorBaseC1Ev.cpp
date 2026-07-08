//cpp
extern "C" {
void _ZN9ActorBase9SceneNodeC1Ev(void);
int func_0203b438(void);
int func_02043810(void);
extern int _ZTV9ActorBase;
extern int NEXT_UNIQUE_ACTOR_ID;
extern int ACTOR_PARAM1;
extern int ACTOR_ACTOR_ID;
extern int data_020a4b64;
extern int ACTOR_UNKNOWN12_ID;
extern int data_020a4b6c;
extern int ACTOR_SPAWN_INFO_TABLE_PTR;
}

extern "C" asm void* _ZN9ActorBaseC1Ev(void* self) {
    stmdb sp!, {r4, r5, lr}
    sub sp, sp, #4
    mov r4, r0
    ldr r1, =_ZTV9ActorBase
    add r5, r4, #0x14
    mov r0, r5
    str r1, [r4]
    bl _ZN9ActorBase9SceneNodeC1Ev
    str r4, [r5, #0x10]
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
    ldr r1, =NEXT_UNIQUE_ACTOR_ID
    ldr r0, =ACTOR_PARAM1
    ldr r2, [r1]
    str r2, [r4, #4]
    ldr r3, [r1]
    ldr r2, [r0]
    add r0, r3, #1
    str r0, [r1]
    str r2, [r4, #8]
    ldr r0, =ACTOR_ACTOR_ID
    ldr r2, =data_020a4b64
    ldrh r1, [r0]
    ldr r0, =ACTOR_UNKNOWN12_ID
    strh r1, [r4, #0xc]
    ldrb r3, [r0]
    ldr r0, =data_020a4b6c
    mov r1, r5
    strb r3, [r4, #0x12]
    ldr r2, [r2]
    bl func_0203b438
    ldr r0, =ACTOR_SPAWN_INFO_TABLE_PTR
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
    bl func_02043810
    cmp r0, #0
    beq Ldone
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
