//cpp

namespace CP15 {
void DrainWriteBuffer(void);
void FlushAndInvalidateDataCache(unsigned int a, unsigned int b);
}
extern "C" {
extern void CpuCopy8(int src, int dst, unsigned int n);

extern int func_02060f60(void *o, int a, int b);
extern void func_0206081c(void *o);

/* The request block each chunk is handed over in (NitroSDK CARDiCommandArg). */
struct ChunkRequest {
    unsigned char pad_00[0xc];
    int mSrc;          /* 0x0c */
    int mDst;          /* 0x10 */
    unsigned int mLen; /* 0x14 */
};

/* The chunked backup writer's state (NitroSDK CARDiCommon). */
struct ChunkWriter {
    ChunkRequest *mRequest; /* 0x00 */
    unsigned char pad_04[0x14];
    int mSrc;                /* 0x18 */
    int mDst;                /* 0x1c */
    unsigned int mRemaining; /* 0x20 */
    unsigned char pad_24[0xbc];
    char mBuffer[0x100]; /* 0xe0 */
};

void func_0206062c(ChunkWriter *self) {
    char *buf = self->mBuffer;
    do {
        unsigned int n = self->mRemaining;
        int src = self->mSrc;
        if (n > 0x100) n = 0x100;
        CpuCopy8(src, (int)buf, n);
        CP15::FlushAndInvalidateDataCache((unsigned int)buf, n);
        CP15::DrainWriteBuffer();
        self->mRequest->mSrc = (int)buf;
        self->mRequest->mDst = self->mDst;
        self->mRequest->mLen = n;
        if (func_02060f60(self, 8, 0xa) == 0) break;
        if (func_02060f60(self, 9, 1) == 0) break;
        self->mSrc += n;
        self->mDst += n;
        self->mRemaining -= n;
    } while (self->mRemaining != 0);
    func_0206081c(self);
}
}
