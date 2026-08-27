typedef struct {
    unsigned char pad[0x18];
    unsigned int src;
    unsigned int dst;
    unsigned int size;
} CardXferState;

extern CardXferState data_020a8180;

extern void func_02060d98(unsigned int a, unsigned int b);
extern int func_02060ebc(char *self);

int func_02060a64(char *self) {
    CardXferState *g = &data_020a8180;
    unsigned int *buf;
    unsigned int *altBuf = (unsigned int *)(self + 0x20);
    volatile unsigned int *romctrl = (volatile unsigned int *)0x40001A4;
    volatile unsigned int *fifoData = (volatile unsigned int *)0x4100010;
    unsigned int mask = (unsigned int)-0x200;

    for (;;) {
        unsigned int src = g->src;
        unsigned int aligned = src & mask;
        unsigned int i;
        unsigned int status;
        unsigned int cmd;

        if (aligned == src && (g->dst & 3) == 0 && g->size >= 0x200) {
            buf = (unsigned int *)g->dst;
        } else {
            buf = altBuf;
            *(unsigned int *)(self + 8) = aligned;
        }

        func_02060d98((aligned >> 8) | 0xb7000000, aligned << 0x18);

        cmd = *(unsigned int *)(self + 4);
        i = 0;
        *romctrl = cmd;
        do {
            status = *romctrl;
            if (status & 0x800000) {
                unsigned int val = *fifoData;
                if (i < 0x200) {
                    buf[i] = val;
                    i++;
                }
            }
        } while (status & 0x80000000);

        if (buf == (unsigned int *)*(volatile unsigned int *)&data_020a8180.dst) {
            data_020a8180.src += 0x200;
            data_020a8180.dst += 0x200;
            data_020a8180.size -= 0x200;
            if (data_020a8180.size == 0) return data_020a8180.size;
        } else {
            int rc = func_02060ebc(self);
            if (rc == 0) return rc;
        }
    }
}
