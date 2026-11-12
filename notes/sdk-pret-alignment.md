# Nitro SDK file boundaries from pret's NitroSDK sources

Status: research note, 2026-10-29. Evidence for SDK TU folds in arm9 main.

## Method

pret/pokediamond carries the NitroSDK as per-file C (`arm9/lib/NitroSDK/src/*.c`,
with asm functions written as `asm void`). Each file compiles cleanly with
pokediamond's own pinned compiler (mwccarm 1.2/sp2p3, its `arm9/lib/NitroSDK/Makefile`
flags). That gives every SDK file's functions with sizes, in emitted section order.

The alignment matches each file's size sequence against the arm9 function list in
`config/arm9/symbols.txt`, address-ascending. It is a local alignment: an exact
size scores, a different size costs a little (SDK version drift), and a pret
function that is missing here costs almost nothing. Two things make such a gap
normal. pret's files hold only the functions Pokemon links, and this ROM holds only
the functions SM64DS links. An SM64DS function that pret doesn't have costs the
most, because it is the case that makes a boundary uncertain.

Diamond's SDK is about two years newer than ours, so some sizes drift. Function
order within a file has held everywhere it was checked except GX_g3x.c, where
SetFogTable and the 0x18 table loader after SetClearColor come in the other order.

## Screen on units already promoted

| pret file | aligned run | promoted TU | agrees? |
|---|---|---|---|
| GX.c | 0x02053a90.. (7 exact, GX_Init drifts 0x184 to 0x15c) | arm9/GX_Init | yes, both edges |
| GX_bgcnt.c | 0x02054d38..0x020551f0, 16/16 exact | arm9/GX_BgCnt | yes, both edges |
| GX_vramcnt.c | 0x02053ea0..0x02054d38, 37 exact + 1 drift | arm9/GX_VramCnt | yes, both edges: the 3 functions below (0x02053e1c..0x02053ea0) are gx_vramcnt.c's GX_GetSizeOfX_, GetSizeOfTex and GetSizeOfTexPltt, which pret's file leaves out (Pokemon doesn't link them); RushRE's NitroSDK gx_vramctrl.c ends with them |
| OS_system.c | 0x02059d08..0x02059d98, 8/8 | arm9/IRQ3 | yes; IRQ3 leaves out OS_SpinWait (src/OS_SpinWait.c, the asm hatch) on purpose |
| OS_tcm.c, OS_protectionUnit.c | 1 and 2 exact | arm9/CP15_TCM, arm9/CP15_ProtectionUnit | yes |
| OS_cache.c | 0x02058bdc.., 5 exact + 1 drift | arm9/CP15 | yes |
| OS_irqTable.c + OS_interrupt.c | 0x02056c40..0x02056d64, then ..0x02057014 | arm9/OS_IrqTable + arm9/OS_Interrupt | fixed: arm9/IRQ held both files; split at 0x02056d64 (OS_InitIrqTable, at 0x02057000, is 0x14 here) |
| GX_load2d.c + GX_load3d.c | 0x02055dec..0x020568cc, then ..0x02056c1c | arm9/GXS + arm9/GX | fixed: the edge was the namespace change at 0x02056870; GX_LoadBGPltt moved to GXS, edge now 0x020568cc |
| GX_g2.c | func_020551f0 + the 3 G2x members | arm9/G2x | partial: G2x_ChangeBlendBrightness_ (func_020551f0) is outside it |
| GX_g3x.c | 0x020553c0..0x02055998 | arm9/G3X (0x02055454..0x02055998, 13 functions) + GXi_NopClearFifo128_.c | fixed: G3X holds the whole file except GXi_NopClearFifo128_ (0x020553c0, asm), which stays its own source because port/tools/hostgen.py skips any file with an asm block and the port hostgens G3X.cpp |

Where a promoted boundary came from TU evidence, the alignment reproduces it.
Where it came from a namespace (GXS/GX, G2x, G3X), it doesn't. Call-graph and
.rodata order failed this same screen; this method has an outside reference to
check against, and they don't.

## Unfolded shard runs that align

| pret file | run | result |
|---|---|---|
| MI_memory.c | 0x0205a448..0x0205a74c | 11/11 exact, 2 pret functions dropped by the linker: folded as arm9/MI_Memory |
| FX_mtx33.c | 0x020524f0..0x020527c0 | 6 exact, 1 extra 0x3c function here (func_02052514); MTX_RotX33_ (0x02052550) and MTX_RotY33_ (0x0205256c) named from pret's asm, the hand-written Thumb bodies byte-identical; FX_mtx22.c's MTX_Rot22_ (0x020524e4) sits just before the run |
| OS_spinLock.c | 0x02057014..0x02057410 | 18 of 19: 17 exact, OSi_DoLockByWord 0x54 to 0x58, OS_LockCartridge dropped by the linker, 1 extra (OSi_WaitByLoop, out of line here): folded as arm9/OS_SpinLock |
| OS_printf.c | 0x02057410..0x02057e30 | 6 of 7, each identified by body against pret's pokediamond OS_printf.c (OS_VSNPrintf, OS_VSPrintf, OS_SPrintf, string_put_string, string_fill_char, string_put_char), in its source order reversed; OS_SNPrintf dropped by the linker: folded as arm9/OS_Printf |
| OS_thread.c | 0x02057e34..0x02058568 | 17 of 34: 11 exact, 6 drift from an older SDK (16-bit thread-id queues and a 16-entry id table: OSi_GetUnusedThreadId 0x18 to 0x30, OS_InitThread 0x15c to 0x180, OS_ExitThread 0x2c to 0x58, OS_SleepThread 0x54 to 0x68, OS_WakeupThread 0x80 to 0x74, and OS_RescheduleThread 0xc8 carrying pret's OSi_RescheduleThread body), in pret's source order reversed; 1 extra (OS_SetThreadStackWarningOffset); the rest absent or dropped by the linker: folded as arm9/OS_Thread |
| OS_emulator.c | 0x0205865c..0x020587e4 | 4 of 4, each identified by body against pret's pmd-sky os_emulator.c (OSi_IsRunOnDebugger, OSi_GetDeviceType, OS_GetConsoleType, OS_IsRunOnEmulator), in its source order reversed; OS_IsRunOnEmulator returns FALSE as in pret's pokediamond, while OS_GetConsoleType keeps the full detection: folded as arm9/OS_Emulator |
| OS_message.c | 0x020587e4..0x02058960 | 3 of 5, in pret's source order reversed; OS_JamMessage and OS_ReadMessage dropped by the linker: folded as arm9/OS_Message |
| OS_mutex.c | 0x02058960..0x02058bb0 | 7 of 7 plus 1, in pret's source order reversed, from an older SDK (16-bit thread-id queues, OS_TryLockMutex does not enqueue, OSi_EnqueueTail links after the head); 1 extra (func_02058960, the held-list unlink pret has in OS_thread.c): folded as arm9/OS_Mutex |
| OS_arena.c | 0x02058cd0..0x02059040 | 9 of 10: 6 exact, 3 drift from an older SDK (OS_InitArenaEx 0x80 to 0x60, OS_GetInitArenaHi 0x130 to 0xac, OS_GetInitArenaLo 0xec to 0x74), in pret's source order reversed; OS_AllocFromArenaHi dropped by the linker: folded as arm9/OS_Arena |
| OS_alloc.c | 0x02059040..0x020593a8 | 5/5 exact, in pret's source order reversed, plus OS_InitAlloc (0x02059040) by body, which pret leaves out: folded as arm9/OS_Alloc |
| OS_exception.c | 0x020593f4..0x02059624 | 5 of 5 plus 1: 4 exact, OS_InitException 0x70 to 0x90 (it also tests OS_GetConsoleType), 1 extra (OS_SetUserExceptionHandler, not in pret's revision); the three asm functions first, then the C bodies reversed: folded as arm9/OS_Exception |
| OS_tick.c | 0x02059640..0x02059824 | 4 of 5, all exact, in pret's source order reversed; OS_IsTickAvailable dropped by the linker: folded as arm9/OS_Tick |
| OS_alarm.c | 0x02059824..0x02059cb4 | 9 of 9: 8 exact, OSi_ArrangeTimer 0x118 to 0x11c; asm OSi_AlarmHandler first, then the C bodies reversed: folded as arm9/OS_Alarm |
| OS_reset.c | 0x02059dd4..0x02059eb0 | 3 of 9, each identified by body against pret's pokediamond OS_reset.c (OSi_SendToPxi, OSi_CommonCallback, OS_InitReset), in its source order reversed; OS_ResetSystem, OSi_DoBoot, OSi_ReloadRomData and OSReadROMArea are ITCM code (src/OSi_ReloadRomData.c): folded as arm9/OS_Reset |
| OS_ownerInfo.c | 0x02059eb0..0x02059f48 | 2 of 3, each identified by body against pret's pokediamond OS_ownerInfo.c (OS_GetOwnerInfo, OS_GetMacAddress), in its source order reversed; OS_GetOwnerRtcOffset dropped by the linker: folded as arm9/OS_OwnerInfo |
| MI_dma.c | 0x02059f58..0x0205a21c | 7, each identified by body against pret's pokediamond MI_dma.c (MI_StopDma, MI_WaitDma, MI_DmaCopy32Async, MI_DmaFill32Async, MI_DmaCopy16, MI_DmaCopy32, MI_DmaFill32), in its source order reversed; from an older SDK (MI_WaitDma only spins on the enable bit, MI_StopDma has no DMA0 workaround); the MIi_DmaSetParams family is in ITCM and the rest of the file is dropped by the linker: folded as arm9/MI_Dma |
| MI_dma_gxcommand.c | 0x0205a21c..0x0205a448 | 3, each identified by body against pret's pokediamond MI_dma_gxcommand.c (MIi_DMACallback, MIi_FIFOCallback, MI_SendGXCommandAsync), in its source order reversed; MI_SendGXCommand is in ITCM; from an older SDK (no MIi_CheckDma0SourceAddress, and MIi_GXDmaParams.isBusy is not volatile); MI_SendGXCommandAsyncFast is not in the ROM: folded as arm9/MI_DmaGXCommand |
| SND_main.c | 0x0205a82c..0x0205a86c | 1 of 1: SND_Init by body (behind a once-only flag, SND_CommandInit then SND_AlarmInit; no sound mutex in this SDK); renamed, src/SND_Init.c |
| SND_interface.c | 0x0205a86c..0x0205adf8 | 28 functions, each identified by body (a PushCommand_impl call with the command's arguments, or SNDi_SetPlayerParam/SNDi_SetTrackParam with a field's offset and size), in pret's source order reversed; from an older SDK (no SND_PrepareSeq or SND_StartPreparedSeq, command numbers below pret's: SND_PauseSeq is 2, SND_SetOutputSelector 0x15, the invalidates 0x1b..0x1d); 4 of them pret's file does not define (SND_SetTrackModDepth and SND_SetTrackModSpeed, in its header's order; SND_SetTrackMute and SNDi_SetSurroundDecay, where it leaves them as comments): folded as arm9/SND_Interface |
| SND_command.c | 0x0205adf8..0x0205b470 | 15 of 15, identified by body, in pret's source order reversed; from an older SDK (no interrupt masking around the lists, no OS_SpinWait on a full FIFO, no SND_CMD_FLAG_IMMEDIATE, no IsCommandAvailable: SND_AllocCommand and InitPXI test OS_IsRunOnEmulator): folded as arm9/SND_Command |
| SND_alarm.c | 0x0205b470..0x0205b554 | 4 of 4, identified by body, in pret's source order reversed: folded as arm9/SND_Alarm |
| SND_work.c | 0x0205b554..0x0205b63c | 3 of 5, identified by body, in pret's source order reversed; SND_GetChannelStatus and SND_GetPlayerTickCounter dropped by the linker; each reader invalidates the whole shared work block: folded as arm9/SND_Work |
| SND_util.c | 0x0205b63c..0x0205b6b0 | 1: SND_CalcChannelVolume by body (the -723..0 clamp and the decibel table); renamed, src/SND_CalcChannelVolume.c |
| FS_command_default.c | 0x0205bbdc..0x0205c5e4 | 12 of 12: the nine commands are fsi_default_command's nine entries in FS_COMMAND order, the three helpers identified by body; from an older SDK (FSi_ReadTable returns nothing, FSi_ReadFileCommand and FSi_WriteFileCommand do not return the archive callback's result, 16-bit thread-id queues, the helpers in the order FSi_ReadTable, FSi_SeekDirDirect, FSi_StrNICmp), in that source order reversed: folded as arm9/FS_CommandDefault |
| FS_command.c | 0x0205c5e4..0x0205c7c4 | 2 of 2 (FSi_TranslateCommand, FSi_ReleaseCommand), identified by body, in pret's source order reversed; FSi_TranslateCommand falls back to fsi_default_command when the archive's own proc declines: folded as arm9/FS_Command |
| FS_archive.c | 0x0205c7c4..0x0205d368 | 20 of 20, each identified by body, in pret's source order reversed except FS_FindArchive (just above FSi_NextCommand here, after FS_InitArchive in pret) and FS_NotifyArchiveAsyncEnd (between FSi_SendCommand and FS_InitArchive here, last in pret); from an older SDK (no OPERATING bit or suspend test in FSi_SendCommand, no SUSPENDING test before FSi_NextCommand starts a command, no FS_ARCHIVE_FLAG_REGISTER from FS_RegisterArchiveName, no read checks in FS_LoadArchiveTables, no resume in FS_UnloadArchive, 16-bit thread-id queues): folded as arm9/FS_Archive |
| FS_rom.c | 0x0205d94c..0x0205db88 | 6 of 9, each identified by body, in pret's source order reversed; from an older SDK (no multiboot-child branch in FSi_InitRom, so FSi_ReadDummyCallback and FSi_EmptyArchiveProc are absent; no FS_SetDefaultDMA; no CARD_IsPulledOut test in FSi_OnRomReadDone): folded as arm9/FS_Rom |
| SPI_pm.s (asm only) | 0x0205f5c0..0x0205ff70 | 24 of pmd-sky's 37, each identified by body against pret's pokediamond and pret's pmd-sky; from an older SDK (no sleep-callback lists, no PMi_WriteRegister/PMi_SetAmp, no LED-pattern read; PM_GoSleepMode, PMi_SetLCDPower and PM_ForceToPowerOffAsync without the backlight, amplifier and LCD steps; register replies for commands 0x70..0x73), in pret's pokediamond address order: folded as arm9/PM |
| CARD_common.c | 0x0205ff70..0x020602bc | 9 of 14: 5 exact, 4 drift from an older SDK (CARDi_InitCommon 0x100 to 0xb4, CARDi_UnlockResource 0xa4 to 0xa8, CARDi_LockResource 0x9c to 0xa0, CARDi_SetTask 0x48 to 0x94), in pret's source order reversed; the other 5 dropped by the linker: folded as arm9/CARD_Common |
| CARD_rom.c | 0x02060890..0x02060a64 | 4 renamed by body (CARD_Init, CARD_WaitRomAsync, CARDi_ReadRom, CARDi_ReadRomSyncCore), not folded; the order is not pret's reversed (CARD_Init sits below CARD_WaitRomAsync here) |
| mb_parent.c (asm only) | 0x02065eb8..0x020672d0 | 19 functions: 18 identified by body against pret's pmd-sky, and MB_CommGetChildrenNumber (not in pmd-sky) by body as the SDK's public mb.h count of accepted children; from an older SDK (no any_recv_bitmap test in MBi_calc_sendblock, no block cache: MBi_CommParentSendBlock reads straight from the file, pmd-sky's MBi_ReloadCache is absent), in pmd-sky's order reversed: folded as arm9/MB_Parent |
| mb_wm_base.c (not in pret) | 0x0206879c..0x0206a318 | 23 functions, aligned by body against the BW2 decomp's reconstruction of the file and the HeartGold fork's mb_comm_params.c: 7 carry the SDK name on a statement-for-statement match, 16 are older versions of SDK functions and keep address names; in the reconstruction's order reversed, except MBi_SetMaxScanTime and func_0206881c, which sit just above MBi_IsStarted: folded as arm9/MB_WmBase |
| CTRDG_common.c | 0x0206a318..0x0206a600 | 7 of 24: 6 exact, CTRDG_IsExisting 0x11c to 0x120, in pret's source order reversed; the other 17 dropped by the linker: folded as arm9/CTRDG_Common |
| CTRDG_proc.s (asm only) | 0x0206a600..0x0206a928 | 5 of 6, in the same address order as pret's asm: CTRDGi_CallbackForInitModuleInfo and CTRDG_Init statement for statement, the other three older versions of CTRDG_TerminateForPulledOut, CTRDGi_PulledOutCallback and CTRDGi_InitModuleInfo; CTRDGi_CallbackForSetPhi absent, and the .bss is pret's in pret's order without the two words only it and isOnTerminate use: folded as arm9/CTRDG_Proc |

The single-function SDK files sit between these runs: GX_state.c's GX_InitGXState
is src/GX_InitGXState.c (0x02053d9c, 0x80), and MI_swap.c's MI_SwapWord is src/MI_SwapWord.c (0x0205a74c).
OS_init.c's OS_Init is src/OS_Init.cpp (0x02058c84). It makes pret's calls in pret's order, which names
each callee: PXI_init.c's PXI_Init (src/PXI_Init.c, 0x0205b858), MI_wram.c's MI_SetWramBank
(src/MI_SetWramBank.c), OS_valarm.c's OS_InitVAlarm (src/OS_InitVAlarm.cpp), OS_reset.c's
OS_InitReset (arm9/OS_Reset), CTRDG_Init (src/engine/ctrdg/CTRDG_Proc.cpp) and PM_Init
(src/engine/spi/PM.c).
OS_timer.c's OSi_SetTimerReserved is src/OSi_SetTimerReserved.c (0x02059624, 0x1c), between the
OS_exception.c and OS_tick.c runs.
OS_reset.c's OSi_SendToPxi and OSi_CommonCallback (0x02059dd4, 0x02059e04) sit just below OS_InitReset
in pret's order reversed, and the three are folded as arm9/OS_Reset.
