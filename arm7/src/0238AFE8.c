// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x0238AFE8 size 0x34 (runtime 0x03802E80, wram)
// C tried by: AR57; asm taken from the AR57 job.
/* storage 0x0238AFE8 runtime 0x03802E80 linked 0x34 wram ARM.
 * SPI control at 0x04000138. ORR #0 keeps a bit that is already clear. */

asm void fn_0238AFE8(void) {
    MOV IP, #0x4000000
    ADD IP, IP, #0x138
    MOV R3, #2
d1:
    SUBS R3, R3, #1
    BNE d1
    LDRH R0, [IP]
    BIC R0, R0, #4
    ORR R0, R0, #0
    STRH R0, [IP]
    MOV R3, #2
d2:
    SUBS R3, R3, #1
    BNE d2
    BX LR
}
