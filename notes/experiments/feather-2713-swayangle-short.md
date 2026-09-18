# *(short*)&mSwayAngle increment vs plain u16 form (FEATHER-2713-01, PR #2713)

- tested_commit: f9c1e4609a52783e8a611ba2608225d6e7419aac (cleanup-wingfeather at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daFeather_c/daFeather_c.cpp:179` keeps
  the `*(short*)&mSwayAngle += 0x400` pun because the cartridge
  sign-extends it (ldrsh) while a plain `mSwayAngle += 0x400` on the
  u16 field zero-extends (ldrh).
- attempted_change (scratch-applied, then reverted):

```diff
-        *(short*)&mSwayAngle += 0x400;
-        u16 newv = *(volatile u16*)&mSwayAngle;
+        mSwayAngle += 0x400;
+        u16 newv = *(volatile u16*)&mSwayAngle;
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daFeather_c/daFeather_c.cpp \
  --func _ZN11daFeather_c8BehaviorEv --addr 0x020b2e9c --size 0x280 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 1 word(s) differ at +0x10c
  (f020d3e1 ldrsh vs b020d3e1 ldrh), size same 0x280.
- log (variant):

```text
TARGET _ZN11daFeather_c8BehaviorEv @ 0x020b2e9c size 0x280  bytes: 10402de910d04de25c129fe50040a0e10000d1e5010050e30100a0030000a013000050e30f00001a0400a0e10277fdeb081090e5000051e30200001aed2e00eb... (640 bytes)
  2004/b56: 1 word(s) differ

--- closest: 2004/b56 (1 differ) ---
   +0x00 | 10402de9 push {r4, lr}            | 10402de9 push {r4, lr}            | OK
   +0x04 | 10d04de2 sub sp, sp, #0x10        | 10d04de2 sub sp, sp, #0x10        | OK
   +0x08 | 5c129fe5 ldr r1, [pc, #0x25c]     | 5c129fe5 ldr r1, [pc, #0x25c]     | OK
   +0x0c | 0040a0e1 mov r4, r0               | 0040a0e1 mov r4, r0               | OK
   +0x10 | 0000d1e5 ldrb r0, [r1]            | 0000d1e5 ldrb r0, [r1]            | OK
   +0x14 | 010050e3 cmp r0, #1               | 010050e3 cmp r0, #1               | OK
   +0x18 | 0100a003 moveq r0, #1             | 0100a003 moveq r0, #1             | OK
   +0x1c | 0000a013 movne r0, #0             | 0000a013 movne r0, #0             | OK
   +0x20 | 000050e3 cmp r0, #0               | 000050e3 cmp r0, #0               | OK
   +0x24 | 0f00001a bne #0x44                | 0f00001a bne #0x44                | OK
   +0x28 | 0400a0e1 mov r0, r4               | 0400a0e1 mov r0, r4               | OK
   +0x2c | 0277fdeb bl #0xfff5dc10           | 000000eb bl #8                    | reloc (wildcard)
   +0x30 | 081090e5 ldr r1, [r0, #8]         | 081090e5 ldr r1, [r0, #8]         | OK
   +0x34 | 000051e3 cmp r1, #0               | 000051e3 cmp r1, #0               | OK
   +0x38 | 0200001a bne #0x10                | 0200001a bne #0x10                | OK
   +0x3c | ed2e00eb bl #0xbbbc               | 000000eb bl #8                    | reloc (wildcard)
   +0x40 | 000050e3 cmp r0, #0               | 000050e3 cmp r0, #0               | OK
   +0x44 | 0700000a beq #0x24                | 0700000a beq #0x24                | OK
   +0x48 | 0400a0e1 mov r0, r4               | 0400a0e1 mov r0, r4               | OK
   +0x4c | 9473fdeb bl #0xfff5ce58           | 000000eb bl #8                    | reloc (wildcard)
   +0x50 | 0400a0e1 mov r0, r4               | 0400a0e1 mov r0, r4               | OK
   +0x54 | 4b42feeb bl #0xfff90934           | 000000eb bl #8                    | reloc (wildcard)
   +0x58 | 10d08de2 add sp, sp, #0x10        | 10d08de2 add sp, sp, #0x10        | OK
   +0x5c | 0100a0e3 mov r0, #1               | 0100a0e3 mov r0, #1               | OK
   +0x60 | 1040bde8 pop {r4, lr}             | 1040bde8 pop {r4, lr}             | OK
   +0x64 | 1eff2fe1 bx lr                    | 1eff2fe1 bx lr                    | OK
   +0x68 | 641094e5 ldr r1, [r4, #0x64]      | 641094e5 ldr r1, [r4, #0x64]      | OK
   +0x6c | 0000a0e3 mov r0, #0               | 0000a0e3 mov r0, #0               | OK
   +0x70 | 00108de5 str r1, [sp]             | 00108de5 str r1, [sp]             | OK
   +0x74 | 04008de5 str r0, [sp, #4]         | 04008de5 str r0, [sp, #4]         | OK
   +0x78 | 08008de5 str r0, [sp, #8]         | 08008de5 str r0, [sp, #8]         | OK
   +0x7c | 800394e5 ldr r0, [r4, #0x380]     | 800394e5 ldr r0, [r4, #0x380]     | OK
   +0x80 | 5c2094e5 ldr r2, [r4, #0x5c]      | 5c2094e5 ldr r2, [r4, #0x5c]      | OK
   +0x84 | 603094e5 ldr r3, [r4, #0x60]      | 603094e5 ldr r3, [r4, #0x60]      | OK
   +0x88 | 4a10a0e3 mov r1, #0x4a            | 4a10a0e3 mov r1, #0x4a            | OK
   +0x8c | 94bffdeb bl #0xfff6fe58           | 000000eb bl #8                    | reloc (wildcard)
   +0x90 | 800384e5 str r0, [r4, #0x380]     | 800384e5 str r0, [r4, #0x380]     | OK
   +0x94 | 0400a0e1 mov r0, r4               | 0400a0e1 mov r0, r4               | OK
   +0x98 | 0010a0e3 mov r1, #0               | 0010a0e3 mov r1, #0               | OK
   +0x9c | 3c77fdeb bl #0xfff5dcf8           | 000000eb bl #8                    | reloc (wildcard)
   +0xa0 | 560f84e2 add r0, r4, #0x158       | 560f84e2 add r0, r4, #0x158       | OK
   +0xa4 | 2d15feeb bl #0xfff854bc           | 000000eb bl #8                    | reloc (wildcard)
   +0xa8 | 560f84e2 add r0, r4, #0x158       | 560f84e2 add r0, r4, #0x158       | OK
   +0xac | e609feeb bl #0xfff827a0           | 000000eb bl #8                    | reloc (wildcard)
   +0xb0 | 000050e3 cmp r0, #0               | 000050e3 cmp r0, #0               | OK
   +0xb4 | 1300000a beq #0x54                | 1300000a beq #0x54                | OK
   +0xb8 | df0f84e2 add r0, r4, #0x37c       | df0f84e2 add r0, r4, #0x37c       | OK
   +0xbc | 0010a0e3 mov r1, #0               | 0010a0e3 mov r1, #0               | OK
   +0xc0 | 5020a0e3 mov r2, #0x50            | 5020a0e3 mov r2, #0x50            | OK
   +0xc4 | dc1ffeeb bl #0xfff87f78           | 000000eb bl #8                    | reloc (wildcard)
   +0xc8 | a0219fe5 ldr r2, [pc, #0x1a0]     | a0219fe5 ldr r2, [pc, #0x1a0]     | OK
   +0xcc | 980084e2 add r0, r4, #0x98        | 980084e2 add r0, r4, #0x98        | OK
   +0xd0 | 0010a0e3 mov r1, #0               | 0010a0e3 mov r1, #0               | OK
   +0xd4 | b81ffeeb bl #0xfff87ee8           | 000000eb bl #8                    | reloc (wildcard)
   +0xd8 | e10f84e2 add r0, r4, #0x384       | e10f84e2 add r0, r4, #0x384       | OK
   +0xdc | 951ffeeb bl #0xfff87e5c           | 000000eb bl #8                    | reloc (wildcard)
   +0xe0 | 000050e3 cmp r0, #0               | 000050e3 cmp r0, #0               | OK
   +0xe4 | 1b00001a bne #0x74                | 1b00001a bne #0x74                | OK
   +0xe8 | 5c1094e5 ldr r1, [r4, #0x5c]      | 5c1094e5 ldr r1, [r4, #0x5c]      | OK
   +0xec | 602094e5 ldr r2, [r4, #0x60]      | 602094e5 ldr r2, [r4, #0x60]      | OK
   +0xf0 | 643094e5 ldr r3, [r4, #0x64]      | 643094e5 ldr r3, [r4, #0x64]      | OK
   +0xf4 | d200a0e3 mov r0, #0xd2            | d200a0e3 mov r0, #0xd2            | OK
   +0xf8 | bfbffdeb bl #0xfff6ff04           | 000000eb bl #8                    | reloc (wildcard)
   +0xfc | 0400a0e1 mov r0, r4               | 0400a0e1 mov r0, r4               | OK
   +0x100 | 2042feeb bl #0xfff90888           | 000000eb bl #8                    | reloc (wildcard)
   +0x104 | 130000ea b #0x54                  | 130000ea b #0x54                  | OK
   +0x108 | df3f84e2 add r3, r4, #0x37c       | df3f84e2 add r3, r4, #0x37c       | OK
   +0x10c | f020d3e1 ldrsh r2, [r3]           | b020d3e1 ldrh r2, [r3]            | <<< MISMATCH
   +0x110 | 030c84e2 add r0, r4, #0x300       | 030c84e2 add r0, r4, #0x300       | OK
   +0x114 | 58119fe5 ldr r1, [pc, #0x158]     | 58119fe5 ldr r1, [pc, #0x158]     | OK
   +0x118 | 012b82e2 add r2, r2, #0x400       | 012b82e2 add r2, r2, #0x400       | OK
   +0x11c | b020c3e1 strh r2, [r3]            | b020c3e1 strh r2, [r3]            | OK
   +0x120 | bc27d0e1 ldrh r2, [r0, #0x7c]     | bc27d0e1 ldrh r2, [r0, #0x7c]     | OK
   +0x124 | 783394e5 ldr r3, [r4, #0x378]     | 783394e5 ldr r3, [r4, #0x378]     | OK
   +0x128 | 020ba0e3 mov r0, #0x800           | 020ba0e3 mov r0, #0x800           | OK
   +0x12c | 4222a0e1 asr r2, r2, #4           | 4222a0e1 asr r2, r2, #4           | OK
   +0x130 | 8220a0e1 lsl r2, r2, #1           | 8220a0e1 lsl r2, r2, #1           | OK
   +0x134 | 012082e2 add r2, r2, #1           | 012082e2 add r2, r2, #1           | OK
   +0x138 | 8220a0e1 lsl r2, r2, #1           | 8220a0e1 lsl r2, r2, #1           | OK
   +0x13c | f21091e1 ldrsh r1, [r1, r2]       | f21091e1 ldrsh r1, [r1, r2]       | OK
   +0x140 | 9321c1e0 smull r2, r1, r3, r1     | 9321c1e0 smull r2, r1, r3, r1     | OK
   +0x144 | 002092e0 adds r2, r2, r0          | 002092e0 adds r2, r2, r0          | OK
   +0x148 | 0000a1e2 adc r0, r1, #0           | 0000a1e2 adc r0, r1, #0           | OK
   +0x14c | 2216a0e1 lsr r1, r2, #0xc         | 2216a0e1 lsr r1, r2, #0xc         | OK
   +0x150 | 001a81e1 orr r1, r1, r0, lsl #20  | 001a81e1 orr r1, r1, r0, lsl #20  | OK
   +0x154 | 981084e5 str r1, [r4, #0x98]      | 981084e5 str r1, [r4, #0x98]      | OK
   +0x158 | 18219fe5 ldr r2, [pc, #0x118]     | 18219fe5 ldr r2, [pc, #0x118]     | OK
   +0x15c | de0f84e2 add r0, r4, #0x378       | de0f84e2 add r0, r4, #0x378       | OK
   +0x160 | 0118a0e3 mov r1, #0x10000         | 0118a0e3 mov r1, #0x10000         | OK
   +0x164 | 941ffeeb bl #0xfff87e58           | 000000eb bl #8                    | reloc (wildcard)
   +0x168 | 030c84e2 add r0, r4, #0x300       | 030c84e2 add r0, r4, #0x300       | OK
   +0x16c | bc27d0e1 ldrh r2, [r0, #0x7c]     | bc27d0e1 ldrh r2, [r0, #0x7c]     | OK
   +0x170 | fc309fe5 ldr r3, [pc, #0xfc]      | fc309fe5 ldr r3, [pc, #0xfc]      | OK
   +0x174 | f0109fe5 ldr r1, [pc, #0xf0]      | f0109fe5 ldr r1, [pc, #0xf0]      | OK
   +0x178 | 4222a0e1 asr r2, r2, #4           | 4222a0e1 asr r2, r2, #4           | OK
   +0x17c | 0221a0e1 lsl r2, r2, #2           | 0221a0e1 lsl r2, r2, #2           | OK
   +0x180 | f22093e1 ldrsh r2, [r3, r2]       | f22093e1 ldrsh r2, [r3, r2]       | OK
   +0x184 | 012982e2 add r2, r2, #0x4000      | 012982e2 add r2, r2, #0x4000      | OK
   +0x188 | bc28c4e1 strh r2, [r4, #0x8c]     | bc28c4e1 strh r2, [r4, #0x8c]     | OK
   +0x18c | bc07d0e1 ldrh r0, [r0, #0x7c]     | bc07d0e1 ldrh r0, [r0, #0x7c]     | OK
   +0x190 | 4002a0e1 asr r0, r0, #4           | 4002a0e1 asr r0, r0, #4           | OK
   +0x194 | 8000a0e1 lsl r0, r0, #1           | 8000a0e1 lsl r0, r0, #1           | OK
   +0x198 | 010080e2 add r0, r0, #1           | 010080e2 add r0, r0, #1           | OK
   +0x19c | 8000a0e1 lsl r0, r0, #1           | 8000a0e1 lsl r0, r0, #1           | OK
   +0x1a0 | f00093e1 ldrsh r0, [r3, r0]       | f00093e1 ldrsh r0, [r3, r0]       | OK
   +0x1a4 | 8000a0e1 lsl r0, r0, #1           | 8000a0e1 lsl r0, r0, #1           | OK
   +0x1a8 | 060a40e2 sub r0, r0, #0x6000      | 060a40e2 sub r0, r0, #0x6000      | OK
   +0x1ac | b009c4e1 strh r0, [r4, #0x90]     | b009c4e1 strh r0, [r4, #0x90]     | OK
   +0x1b0 | 0000d1e5 ldrb r0, [r1]            | 0000d1e5 ldrb r0, [r1]            | OK
   +0x1b4 | 010050e3 cmp r0, #1               | 010050e3 cmp r0, #1               | OK
   +0x1b8 | 0100a003 moveq r0, #1             | 0100a003 moveq r0, #1             | OK
   +0x1bc | 0000a013 movne r0, #0             | 0000a013 movne r0, #0             | OK
   +0x1c0 | 000050e3 cmp r0, #0               | 000050e3 cmp r0, #0               | OK
   +0x1c4 | 0500001a bne #0x1c                | 0500001a bne #0x1c                | OK
   +0x1c8 | ac009fe5 ldr r0, [pc, #0xac]      | ac009fe5 ldr r0, [pc, #0xac]      | OK
   +0x1cc | 000090e5 ldr r0, [r0]             | 000090e5 ldr r0, [r0]             | OK
   +0x1d0 | 010c80e2 add r0, r0, #0x100       | 010c80e2 add r0, r0, #0x100       | OK
   +0x1d4 | fc07d0e1 ldrsh r0, [r0, #0x7c]    | fc07d0e1 ldrsh r0, [r0, #0x7c]    | OK
   +0x1d8 | 010980e2 add r0, r0, #0x4000      | 010980e2 add r0, r0, #0x4000      | OK
   +0x1dc | be08c4e1 strh r0, [r4, #0x8e]     | be08c4e1 strh r0, [r4, #0x8e]     | OK
   +0x1e0 | 480194e5 ldr r0, [r4, #0x148]     | 480194e5 ldr r0, [r4, #0x148]     | OK
   +0x1e4 | 000050e3 cmp r0, #0               | 000050e3 cmp r0, #0               | OK
   +0x1e8 | 1500000a beq #0x5c                | 1500000a beq #0x5c                | OK
   +0x1ec | ab77fdeb bl #0xfff5deb4           | 000000eb bl #8                    | reloc (wildcard)
   +0x1f0 | 000050e3 cmp r0, #0               | 000050e3 cmp r0, #0               | OK
   +0x1f4 | 1200000a beq #0x50                | 1200000a beq #0x50                | OK
   +0x1f8 | bc10d0e1 ldrh r1, [r0, #0xc]      | bc10d0e1 ldrh r1, [r0, #0xc]      | OK
   +0x1fc | bf0051e3 cmp r1, #0xbf            | bf0051e3 cmp r1, #0xbf            | OK
   +0x200 | 0110a003 moveq r1, #1             | 0110a003 moveq r1, #1             | OK
   +0x204 | 0010a013 movne r1, #0             | 0010a013 movne r1, #0             | OK
   +0x208 | 000051e3 cmp r1, #0               | 000051e3 cmp r1, #0               | OK
   +0x20c | 0c00000a beq #0x38                | 0c00000a beq #0x38                | OK
   +0x210 | 0110a0e3 mov r1, #1               | 0110a0e3 mov r1, #1               | OK
   +0x214 | c0b400eb bl #0x2d308              | 000000eb bl #8                    | reloc (wildcard)
   +0x218 | 5c1094e5 ldr r1, [r4, #0x5c]      | 5c1094e5 ldr r1, [r4, #0x5c]      | OK
   +0x21c | 602094e5 ldr r2, [r4, #0x60]      | 602094e5 ldr r2, [r4, #0x60]      | OK
   +0x220 | 643094e5 ldr r3, [r4, #0x64]      | 643094e5 ldr r3, [r4, #0x64]      | OK
   +0x224 | d200a0e3 mov r0, #0xd2            | d200a0e3 mov r0, #0xd2            | OK
   +0x228 | 73bffdeb bl #0xfff6fdd4           | 000000eb bl #8                    | reloc (wildcard)
   +0x22c | 0400a0e1 mov r0, r4               | 0400a0e1 mov r0, r4               | OK
   +0x230 | d441feeb bl #0xfff90758           | 000000eb bl #8                    | reloc (wildcard)
   +0x234 | 10d08de2 add sp, sp, #0x10        | 10d08de2 add sp, sp, #0x10        | OK
   +0x238 | 0100a0e3 mov r0, #1               | 0100a0e3 mov r0, #1               | OK
   +0x23c | 1040bde8 pop {r4, lr}             | 1040bde8 pop {r4, lr}             | OK
   +0x240 | 1eff2fe1 bx lr                    | 1eff2fe1 bx lr                    | OK
   +0x244 | 490f84e2 add r0, r4, #0x124       | 490f84e2 add r0, r4, #0x124       | OK
   +0x248 | ce87fdeb bl #0xfff61f40           | 000000eb bl #8                    | reloc (wildcard)
   +0x24c | 490f84e2 add r0, r4, #0x124       | 490f84e2 add r0, r4, #0x124       | OK
   +0x250 | bf87fdeb bl #0xfff61f04           | 000000eb bl #8                    | reloc (wildcard)
   +0x254 | 0400a0e1 mov r0, r4               | 0400a0e1 mov r0, r4               | OK
   +0x258 | d2feffeb bl #0xfffffb50           | 000000eb bl #8                    | reloc (wildcard)
   +0x25c | 0100a0e3 mov r0, #1               | 0100a0e3 mov r0, #1               | OK
   +0x260 | 10d08de2 add sp, sp, #0x10        | 10d08de2 add sp, sp, #0x10        | OK
   +0x264 | 1040bde8 pop {r4, lr}             | 1040bde8 pop {r4, lr}             | OK
   +0x268 | 1eff2fe1 bx lr                    | 1eff2fe1 bx lr                    | OK
   +0x26c | d8f20902 andeq pc, sb, #216, #4   | 00000000 andeq r0, r0, r0         | reloc (wildcard)
   +0x270 | 55050000 andeq r0, r0, r5, asr r5 | 55050000 andeq r0, r0, r5, asr r5 | OK
   +0x274 | 14220802 andeq r2, r8, #20, #4    | 00000000 andeq r0, r0, r0         | reloc (wildcard)
   +0x278 | 32030000 andeq r0, r0, r2, lsr r3 | 32030000 andeq r0, r0, r2, lsr r3 | OK
   +0x27c | 18f30902 andeq pc, sb, #24, #6    | 00000000 andeq r0, r0, r0         | reloc (wildcard)

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
