extern void FS_Init(int);
extern void func_02017e60(void);

/* Bring up the file system at boot (func_0201a054): FS_Init is
   NitroSDK's FS_Init (once only, guarded by data_020a8058), here with DMA
   channel 2, then every loaded-overlay slot is freed. */
void func_02018aa4(void) {
    FS_Init(2);
    func_02017e60();
}
