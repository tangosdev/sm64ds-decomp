//cpp
#include "Ov007MenuState.h"
#include "Ov007Work.h"


extern "C" void func_ov007_020aec94(int);

/* Slot `arg` was tapped: in main state 0 it hands arg + 3 to
 * func_ov007_020aec94; in 3 it becomes the selected slot, in 2 the target;
 * in 7 the first tap selects, tapping the selection again clears it and any
 * other slot becomes the target; in 9 it selects only when nothing is. */
void func_ov007_020b6134(int arg){
    Ov007Work *w = WORK;
    switch (w->states[1]->currentState) {
    case 0:
        func_ov007_020aec94(arg + 3);
        break;
    case 3:
        w->selectedSlot = arg;
        break;
    case 2:
        w->targetSlot = arg;
        break;
    case 7: {
        int v = w->selectedSlot;
        if (v == -1) {
            w->selectedSlot = arg;
            return;
        }
        if (arg == v)
            w->selectedSlot = -1;
        else
            w->targetSlot = arg;
    }
    break;
  case 9:
      if (w->selectedSlot == -1)
          w->selectedSlot = arg;
      break;
  }
}

