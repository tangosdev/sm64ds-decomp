# 64-bit pass: asm functions

Each mwccarm `asm` function (and each inline `asm { }` block) under `src/` is wrapped in `#ifndef SM64DS_PLATFORM_PC`. The NDS branch is the original asm text, unchanged. The host branch is a declaration when `port/` already provides the symbol, when the routine is CP15/BIOS/startup/IRQ/context, or when the ARM calling convention is not a plain C signature. A C body is used only for a plain algorithm `port/` does not define.

`port/tools/host_frontier.py` treats a file as HAL-owned only when an `asm` definition (`^\s*asm\s+\w`) sits outside that guard. Asm lines inside the guard, through its `#endif`, are ignored. The `cl /Zs` command line is unchanged.

Files wrapped: 79.

## src/__fp_status.c

- `__fp_status` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/__ieee_status.c

- `__ieee_status` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/__rethrow.c

- `__rethrow` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/_dadd.c

- `_dadd` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_dmul.c

- `_dmul` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_dsub.c

- `_dsub` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_fadd.c

- `_fadd` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_ffix.c

- `_ffix` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_fflt.c

- `_fflt` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_ffltu.c

- `_ffltu` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_fp_init.c

- `_fp_init` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_fsub.c

- `_fsub` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_ll_sdiv.c

- `_ll_sdiv` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_ll_udiv.c

- `_ll_udiv` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_ll_ufrom_d.c

- `_ll_ufrom_d` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_s32_div_f.c

- `_s32_div_f` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_start.c

- `_start` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/_u32_div_f.c

- `_u32_div_f` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/_ull_mod.c

- `_ull_mod` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/cstd4.cpp

- `func_0206dab4` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.
- `<inline asm>` — host statement. inline bkpt; host no-op. port: not defined under port/.

## src/DecompressLZ16.c

- `DecompressLZ16` — declaration. BIOS/LZ decompressor; declaration only. port: port/hal/fs.cpp.

## src/do_autoload.c

- `do_autoload` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/engine/cp15/CP15.cpp

- `DC_StoreAll` — declaration. CP15 cache/MPU/TCM; declaration only. port: port/ntr/runtime.cpp.
- `DC_FlushAll` — declaration. CP15 cache/MPU/TCM; declaration only. port: port/ntr/runtime.cpp.
- `DC_InvalidateRange` — declaration. CP15 cache/MPU/TCM; declaration only. port: port/ntr/runtime.cpp.
- `DC_StoreRange` — declaration. CP15 cache/MPU/TCM; declaration only. port: port/ntr/runtime.cpp.
- `DC_FlushRange` — declaration. CP15 cache/MPU/TCM; declaration only. port: port/ntr/runtime.cpp.
- `DC_WaitWriteBufferEmpty` — declaration. CP15 cache/MPU/TCM; declaration only. port: port/ntr/runtime.cpp.
- `IC_InvalidateRange` — declaration. CP15 cache/MPU/TCM; declaration only. port: port/ntr/runtime.cpp.

## src/engine/cp15/CP154.cpp

- `func_0206da64` — declaration. CP15 cache/MPU/TCM; declaration only. port: not defined under port/.
- `func_0206da94` — declaration. CP15 cache/MPU/TCM; declaration only. port: not defined under port/.
- `func_0206da9c` — declaration. CP15 cache/MPU/TCM; declaration only. port: not defined under port/.
- `<inline asm>` — host statement. inline CP15 read; host stores 0 so the C function still returns. port: not defined under port/.
- `<inline asm>` — host statement. inline CP15 write; host no-op. port: not defined under port/.

## src/engine/cp15/CP15_ProtectionUnit.cpp

- `OS_EnableProtectionUnit` — declaration. CP15 cache/MPU/TCM; declaration only. port: not defined under port/.
- `OS_DisableProtectionUnit` — declaration. CP15 cache/MPU/TCM; declaration only. port: not defined under port/.

## src/engine/cp15/CP15_TCM.cpp

- `OS_EnableDTCM` — declaration. CP15 cache/MPU/TCM; declaration only. port: not defined under port/.
- `OS_GetDTCMAddress` — declaration. CP15 cache/MPU/TCM; declaration only. port: not defined under port/.

## src/engine/cp15/OS_SetProtectionRegion1.cpp

- `<inline asm>` — host statement. inline CP15 write; host no-op. port: not defined under port/.

## src/engine/mi/MI_Memory.c

- `MIi_CpuClear16` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: not defined under port/.
- `MIi_CpuCopy16` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: not defined under port/.
- `MIi_CpuClear32` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: port/hal/heap_globals.cpp.
- `MIi_CpuCopy32` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: port/hal/model_host.cpp.
- `MIi_CpuClearFast` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: not defined under port/.
- `MIi_CpuCopyFast` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: port/hal/model_host.cpp.
- `MI_Copy32B` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: not defined under port/.
- `MI_Copy36B` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: port/ntr/runtime.cpp.
- `MI_Copy48B` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: port/hal/actor_vtables.cpp.
- `MI_CpuFill8` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: not defined under port/.
- `MI_CpuCopy8` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: not defined under port/.

## src/engine/os/OS_Alarm.cpp

- `OSi_AlarmHandler` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/engine/os/OS_Exception.cpp

- `OSi_ExceptionHandler` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.
- `OSi_GetAndDisplayContext` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.
- `OSi_SetExContext` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.
- `<inline asm>` — host statement. inline CPSR switch; host no-op. port: not defined under port/.

## src/engine/os/OS_SpinLock.cpp

- `OS_UnLockCartridge` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.
- `OS_GetLockID` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.
- `OS_ReleaseLockID` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/GX_SendFifo48B.c

- `GX_SendFifo48B` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: port/ntr/runtime.cpp.

## src/GXi_NopClearFifo128_.c

- `GXi_NopClearFifo128_` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: port/ntr/runtime.cpp.

## src/init_cp15.c

- `init_cp15` — declaration. CP15 cache/MPU/TCM; declaration only. port: not defined under port/.

## src/IRQ3.cpp

- `OS_EnableInterrupts` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: port/ntr/runtime.cpp.
- `OS_DisableInterrupts` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: port/ntr/runtime.cpp.
- `OS_RestoreInterrupts` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: port/ntr/runtime.cpp.
- `OS_DisableInterrupts_IrqAndFiq` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.
- `OS_RestoreInterrupts_IrqAndFiq` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.
- `OS_GetProcMode` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.
- `OS_Halt` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/IRQHandler.cpp

- `OS_IrqHandler` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.
- `OS_IrqHandler_ThreadSwitch` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/MI_SwapWord.c

- `MI_SwapWord` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/MIi_UncompressBackward.c

- `MIi_UncompressBackward` — declaration. BIOS/LZ decompressor; declaration only. port: not defined under port/.

## src/MTX_Copy33To44_.c

- `MTX_Copy33To44_` — declaration. matrix primitive already defined under port/; no second body. port: port/hal/model_host.cpp.

## src/MTX_Identity33_.c

- `MTX_Identity33_` — C body. plain algorithm, not defined under port/. port: not defined under port/.

## src/MTX_Identity43_.c

- `MTX_Identity43_` — declaration. matrix primitive already defined under port/; no second body. port: port/hal/model_host.cpp.

## src/MTX_Identity44_.c

- `MTX_Identity44_` — C body. plain algorithm, not defined under port/. port: not defined under port/.

## src/MTX_RotX43_.c

- `MTX_RotX43_` — C body. plain algorithm, not defined under port/. port: not defined under port/.

## src/MTX_RotY33_.c

- `MTX_RotY33_` — C body. plain algorithm, not defined under port/. port: not defined under port/.

## src/MTX_RotY43_.c

- `MTX_RotY43_` — declaration. matrix primitive already defined under port/; no second body. port: port/hal/actor_vtables.cpp.

## src/MTX_RotZ33_.c

- `MTX_RotZ33_` — C body. plain algorithm, not defined under port/. port: not defined under port/.

## src/MTX_RotZ43_.c

- `MTX_RotZ43_` — C body. plain algorithm, not defined under port/. port: not defined under port/.

## src/MTX_Scale43_.c

- `MTX_Scale43_` — declaration. matrix primitive already defined under port/; no second body. port: port/hal/model_host.cpp.

## src/OS_InitContext.c

- `OS_InitContext` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/OS_LoadContext.c

- `OS_LoadContext` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/OS_SaveContext.c

- `OS_SaveContext` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/OS_SpinWait.c

- `OS_SpinWait` — C body. plain algorithm, not defined under port/. port: not defined under port/.

## src/OSi_DoBoot.c

- `OSi_DoBoot` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/SVC_CpuSet.c

- `SVC_CpuSet` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: port/ntr/runtime.cpp.

## src/SVC_CpuSetFast.c

- `SVC_CpuSetFast` — declaration. memory primitive; declaration so the HAL keeps the symbol. port: port/ntr/runtime.cpp.

## src/SVC_Div.c

- `SVC_Div` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/SVC_DivRem.c

- `SVC_DivRem` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/SVC_GetCRC16.c

- `SVC_GetCRC16` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/SVC_Halt.c

- `SVC_Halt` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: port/ntr/runtime.cpp.

## src/SVC_IsMmemExpanded.c

- `SVC_IsMmemExpanded` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/SVC_SoftReset.c

- `SVC_SoftReset` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/SVC_Sqrt.c

- `SVC_Sqrt` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/SVC_UncompressHuffmanFromDevice.c

- `SVC_UncompressHuffmanFromDevice` — declaration. BIOS/LZ decompressor; declaration only. port: not defined under port/.

## src/SVC_UncompressLZ16FromDevice.c

- `SVC_UncompressLZ16FromDevice` — declaration. BIOS/LZ decompressor; declaration only. port: port/ntr/lz77.cpp.

## src/SVC_UncompressLZ8.c

- `SVC_UncompressLZ8` — declaration. BIOS/LZ decompressor; declaration only. port: port/ntr/lz77.cpp.

## src/SVC_UncompressRL16FromDevice.c

- `SVC_UncompressRL16FromDevice` — declaration. BIOS/LZ decompressor; declaration only. port: not defined under port/.

## src/SVC_UncompressRL8.c

- `SVC_UncompressRL8` — declaration. BIOS/LZ decompressor; declaration only. port: not defined under port/.

## src/SVC_UnpackBits.c

- `SVC_UnpackBits` — declaration. BIOS/LZ decompressor; declaration only. port: not defined under port/.

## src/SVC_WaitByLoop.c

- `SVC_WaitByLoop` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: port/ntr/runtime.cpp.

## src/SVC_WaitIntr.c

- `SVC_WaitIntr` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: port/ntr/runtime.cpp.

## src/SVC_WaitVBlankIntr.c

- `SVC_WaitVBlankIntr` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: port/ntr/runtime.cpp.

## src/sys_exit.c

- `sys_exit` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/sys_readc.c

- `sys_readc` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/sys_writec.c

- `sys_writec` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/unnamed/arm9/01ff/func_01ff97d8.c

- `func_01ff97d8` — declaration. calling convention or algorithm is not a plain C body; declaration only. port: not defined under port/.

## src/unnamed/arm9/0205/func_02059d98.c

- `func_02059d98` — C body. plain algorithm, not defined under port/. port: not defined under port/.

## src/unnamed/arm9/0206/func_0206d9cc.cpp

- `func_0206d9cc` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/unnamed/arm9/0206/func_0206da18.cpp

- `func_0206da18` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.

## src/unnamed/arm9/0207/func_02071790.c

- `func_02071790` — declaration. startup, IRQ, context, exception, swi, or swp; declaration only. port: not defined under port/.
