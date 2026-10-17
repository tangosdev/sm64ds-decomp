/* Overlay 0 boot entry: resolve the asset paths into the persistent file-ID table.
 * InitializeAssetHandleTable is a descriptive name, not a recovered ROM name.
 */
extern int data_0209d574;
extern int data_020a6134[];
extern int data_ov000_020bd4b8[];

extern int func_0201a9ec(int *timer);
extern int func_02018a58(int count);
extern int func_02057e7c(int thread);
extern int func_02057e84(int thread, int priority);
extern int func_020189f0(int path);
extern void func_02018a40(unsigned handle, int fileId);
extern int func_0201a9fc(int *timer);
extern int func_0201a96c(int *timer, int suspended);

enum { ASSET_HANDLE_COUNT = 0x80a, ASSET_LOOKUP_PRIORITY = 0x1f };

void InitializeAssetHandleTable(void) {
    /* Suspend the timer callback and allocate the zeroed file-ID table. */
    int savedTimerState = func_0201a9ec(&data_0209d574);
    func_02018a58(ASSET_HANDLE_COUNT);

    /* The scheduler's word at offset 0x08 holds the current thread. */
    int currentThread = data_020a6134[2];
    int savedPriority = func_02057e7c(currentThread);
    func_02057e84(currentThread, ASSET_LOOKUP_PRIORITY);

    /* Resolve each path once so later loads can use numeric file IDs. */
    int handle;
    for (handle = 0; handle < ASSET_HANDLE_COUNT; handle++) {
        int fileId = func_020189f0(data_ov000_020bd4b8[handle]);
        if (fileId != 0) {
            func_02018a40(handle, fileId);
        }
    }

    func_02057e84(currentThread, savedPriority);

    /* Reset elapsed time before restoring the timer's previous state. */
    func_0201a9fc(&data_0209d574);
    func_0201a96c(&data_0209d574, savedTimerState);
}
