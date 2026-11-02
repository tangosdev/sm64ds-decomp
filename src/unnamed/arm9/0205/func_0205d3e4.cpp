//cpp
#pragma long_calls on
#include "types.h"
#include "IRQ.h"
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern "C" {

extern int FSi_SendCommand(FSFile *self, int a);
extern void OS_SleepThread(unsigned short *self);
extern int FSi_ExecuteSyncCommand(FSFile *);
extern int func_0205d688(void *, int, int, int);

void func_0205d4a0(FSFile *self, int a, int b) {
    if (self->command != 5) {
        ((unsigned short *)&self->arg.readfile.len)[0] = 0;
        ((unsigned short *)&self->arg.readfile.len)[1] = 0;
    }
    self->arg.readfile.dst = (void *)a;
    self->arg.readfile.len_org = b;
    FSi_SendCommand(self, 5);
}

int func_0205d3f4(FSFile *t) {
    int woken = 0;
    u32 irq = IRQ::Disable();
    int busy = (t->stat & 1) ? 1 : 0;
    if ((int)((long long)busy) != 0) {
        woken = (t->arc->list.next != t) ? 1 : 0;
        if (woken) {
            t->stat |= 4;
        }
        do {
            OS_SleepThread(&t->queue);
            busy = (t->stat & 1) ? 1 : 0;
        } while ((int)((long long)busy) != 0);
    }
    IRQ::Restore(irq);
    if (woken) {
        return FSi_ExecuteSyncCommand(t);
    }
    return t->error == 0;
}

void func_0205d3e4(void *a, int b, int c) {
    func_0205d688(a, b, c, 1);
}
}
