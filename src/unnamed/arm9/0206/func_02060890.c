extern void func_0206002c(void);
extern void func_02061138(void);
extern void func_02060a64(void);
extern char data_020a8180[];
extern void *data_020a8780;

/* The card access state block data_020a8180 (NitroSDK CARDiCommon), the
 * fields this function touches; names as func_02060484.cpp spells them. */
typedef struct CardCommon {
    unsigned char pad_00[0x18];
    int mSrc;                /* 0x18 */
    int mDst;                /* 0x1c */
    unsigned int mRemaining; /* 0x20 */
    /* 0x24 -- the DMA channel transfers use (func_02060d1c indexes the DMA
     * registers by it); -1 for none */
    int mDma;
    /* 0x28 / 0x2c -- the completion callback and its argument (func_0206081c
     * calls it when a request ends) */
    int mCallback;
    int mCallbackArg;
    unsigned char pad_30[4];
    int mFlags; /* 0x34 -- CARD_STAT_* bits; 1 once initialized */
} CardCommon;

/* NitroSDK CARD_Init: once, mark the block initialized and clear the
 * request, set up the card thread and PXI callback (func_0206002c), then
 * install func_02060a64 (the ROM read routine) in data_020a8780 and call
 * func_02061138. */
void func_02060890(void)
{
    CardCommon *g = (CardCommon *)data_020a8180;
    if (g->mFlags != 0)
        return;
    g->mFlags = 1;
    g->mRemaining = 0;
    g->mDst = g->mRemaining;
    g->mSrc = g->mDst;
    g->mDma = -1;
    g->mCallback = 0;
    g->mCallbackArg = 0;
    func_0206002c();
    data_020a8780 = (void *)func_02060a64;
    func_02061138();
}
