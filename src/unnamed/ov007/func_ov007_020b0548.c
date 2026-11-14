#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern void func_ov007_020b1cf0(void);
extern void func_ov007_020b1224(void);
extern void func_ov007_020b1b74(void);
extern void func_ov007_020b1b70(void);
extern int func_ov007_020b1b4c(void);
extern void func_ov007_020b1718(void);
extern void func_ov007_020b166c(void);
extern void func_ov007_020b155c(void);
extern void func_ov007_020b11bc(void);
extern void func_ov007_020b10dc(void);
extern void func_ov007_020b0a20(void);
extern void func_ov007_020b0834(void);

/* Runs the handler for states[1]'s current state; state 7 just marks the
 * choice ready. */
void func_ov007_020b0548(void) {
    Ov007Work *w = WORK;
    switch (w->states[1]->currentState) {
    case 0:
        func_ov007_020b1cf0();
        func_ov007_020b1224();
        break;
    case 1:
        func_ov007_020b1b74();
        break;
    case 2:
        func_ov007_020b1b70();
        break;
    case 3:
        func_ov007_020b1b4c();
        break;
    case 4:
        func_ov007_020b1718();
        break;
    case 5:
        func_ov007_020b166c();
        break;
    case 6:
        func_ov007_020b155c();
        break;
    case 7:
        w->choiceReady = 1;
        break;
    case 8:
        func_ov007_020b11bc();
        func_ov007_020b1224();
        break;
    case 10:
        func_ov007_020b10dc();
        break;
    case 11:
        func_ov007_020b0a20();
        break;
    case 9:
        func_ov007_020b0834();
        break;
    default:
        break;
    }
}
