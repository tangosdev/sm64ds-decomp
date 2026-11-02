extern void TP_Init(void);
extern int TP_GetUserInfo(void *p);
extern void TP_SetCalibrateParam(void *p);
extern void TP_RequestSetStabilityAsync(int a, int b);
extern void TP_WaitBusy(int mask);
extern int TP_CheckError(int mask);
extern void Crash(void);
extern void TP_RequestAutoSamplingStartAsync(unsigned int a, unsigned int b, void *p,
                                             unsigned int d);
extern void *data_020a0de0;
extern void *data_020a0df8;

/* Touch-panel setup at boot (func_0201a054): TP_Init, TP_GetUserInfo and
   TP_SetCalibrateParam on the calibration data_020a0de0, then
   TP_RequestSetStabilityAsync(3, 30) and TP_RequestAutoSamplingStartAsync
   at V-count 0, four samples a frame, into the nine-entry ring
   data_020a0df8 that func_0203b9bc reads. Each request is waited out
   (TP_WaitBusy) and an error (TP_CheckError) crashes. */
void func_0203bbc0(void)
{
    TP_Init();
    TP_GetUserInfo(&data_020a0de0);
    TP_SetCalibrateParam(&data_020a0de0);
    TP_RequestSetStabilityAsync(3, 0x1e);
    TP_WaitBusy(8);
    if (TP_CheckError(8) != 0)
        Crash();
    TP_RequestAutoSamplingStartAsync(0, 4, &data_020a0df8, 9);
    TP_WaitBusy(2);
    if (TP_CheckError(2) != 0)
        Crash();
}
