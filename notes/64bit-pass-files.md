# 64-bit pass: file-image pointer slots

DS_P32 on file-image and DS-bus slots. mwccarm sees an empty macro, so the NDS type is still a plain pointer. No body revert.

## Files changed

- include/BMD_File.h
- include/private/BTP_File.h
- include/dBgW_Kc.h (KCL_File positions, normals, tris, octree only; dBgW_Kc::kclFile left a native pointer)
- include/LVL_Overlay.h (ObjTable::entries and ObjSubTable::entries only)
- include/nitro/fs/overlay.h (ram_address, sinit_init, sinit_init_end)
- src/engine/snd/SND_Bank.cpp (SNDWaveArcLink::waveArc, next, SNDWaveArc::topLink; bodies unchanged)
- src/engine/model/Model4.cpp (UpdateFileOffsets: host uintptr_t base, DS_IADDR on the three (int)slot adds, host skin walk of void * DS_P32)
- src/engine/model/dExtAnmTexPat_c_UpdateFileOffsets.cpp (host uintptr_t base only; REBASE unchanged)
- src/engine/collision/d_bg_w_kc.cpp (UpdateFileOffsets: (char *)&file + DS_IADDR(slot))

## Match results (2004/b56)

- _ZN17dExtSimpleModel_c17UpdateFileOffsetsER8BMD_File @ 0x020465fc size 0x2a4: MATCH
- _ZN15dExtAnmTexPat_c17UpdateFileOffsetsER8BTP_File @ 0x020468a0 size 0x10c: MATCH
- _ZN7dBgW_Kc17UpdateFileOffsetsER8KCL_File @ 0x02039760 size 0x34: MATCH
- SND_AssignWaveArc @ 0x0205b78c size 0xcc: MATCH

## Reverts

None.
