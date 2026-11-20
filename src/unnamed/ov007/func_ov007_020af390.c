#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern void func_ov007_020b01f4(void);
extern void func_ov007_020b01a4(void);
extern void func_ov007_020afc44(void);
extern void func_ov007_020af84c(void);
extern void func_ov007_020af59c(void);
extern void func_ov007_020af4dc(void);
extern void func_ov007_020beeb0(void);

/* Runs the handler for states[1]'s current state; state 12 just marks the
 * choice ready. Then func_ov007_020beeb0. */
void func_ov007_020af390(void)
{
    Ov007Work *w = gMenuWork;
    short state = w->states[1]->currentState;

    switch (state) {
    case 0:
        func_ov007_020b01f4();
        break;
    case 1:
        func_ov007_020b01a4();
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        func_ov007_020afc44();
        break;
    case 7:
    case 8:
        func_ov007_020af84c();
        break;
    case 9:
    case 10:
        func_ov007_020af59c();
        break;
    case 11:
        func_ov007_020af4dc();
        break;
    case 12:
        w->choiceReady = 1;
        break;
    }
    func_ov007_020beeb0();
}
