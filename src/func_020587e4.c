typedef unsigned int u32;
typedef unsigned short u16;

struct MsgQueue {
    u16 queueSend;      /* 0x00 */
    u16 queueReceive;   /* 0x02 */
    u32 *msgArray;      /* 0x04 */
    int msgCount;       /* 0x08 */
    int firstIndex;     /* 0x0c */
    int usedCount;      /* 0x10 */
};

u32 _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(u32 state);
void OS_SleepThread(u16 *self);
void OS_WakeupThread(u16 *self);

int func_020587e4(struct MsgQueue *self, u32 *msg, int flags) {
    u32 enabled = _ZN3IRQ7DisableEv();

    while (self->usedCount == 0) {
        if ((flags & 1) == 0) {
            _ZN3IRQ7RestoreEj(enabled);
            return 0;
        }
        OS_SleepThread(&self->queueReceive);
    }

    if (msg != 0) {
        *msg = self->msgArray[self->firstIndex];
    }
    self->firstIndex = (self->firstIndex + 1) % self->msgCount;
    *(int *)(((long long)(int)&self->usedCount)) -= 1;
    OS_WakeupThread(&self->queueSend);

    _ZN3IRQ7RestoreEj(enabled);
    return 1;
}
