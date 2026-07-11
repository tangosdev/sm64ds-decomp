extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);
extern void func_020580f0(void *this);
extern void func_0205807c(void *p);

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
        func_020580f0(q);
    }
    {
        int idx = (q->cap + q->head) % q->count;
        q->arr[idx] = val;
        {
            int *headp = (int*)(((long long)((char*)q + 0x10)) & 0xFFFFFFFFFFFFFFFFLL);
            *headp = *headp + 1;
        }
        func_0205807c((char*)q + 2);
        _ZN3IRQ7RestoreEj(saved);
        return 1;
    }
}
