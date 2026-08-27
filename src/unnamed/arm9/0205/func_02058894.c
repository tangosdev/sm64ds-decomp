extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);
extern void OS_SleepThread(void *this);
extern void OS_WakeupThread(void *p);

struct Q {
    char pad4[4];
    int *arr;      /* +4 */
    int count;     /* +8 */
    int cap;       /* +0xc */
    int head;      /* +0x10 */
};

int func_02058894(struct Q *q, int val, int flag)
{
    unsigned int saved = _ZN3IRQ7DisableEv();
    while (q->count <= q->head) {
        if ((flag & 1) == 0) {
            _ZN3IRQ7RestoreEj(saved);
            return 0;
        }
        OS_SleepThread(q);
    }
    {
        int idx = (q->cap + q->head) % q->count;
        q->arr[idx] = val;
        {
            int *headp = (int*)(((long long)((char*)q + 0x10)));
            *headp = *headp + 1;
        }
        OS_WakeupThread((char*)q + 2);
        _ZN3IRQ7RestoreEj(saved);
        return 1;
    }
}
