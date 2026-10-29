//cpp
#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern "C" {
extern void func_ov007_020aec94(int);
extern char* data_ov007_0210342c;

/* Slot `arg` was tapped: in main state 0 it hands arg + 3 to
 * func_ov007_020aec94; in 3 it becomes the selected slot, in 2 the target;
 * in 7 the first tap selects, tapping the selection again clears it and any
 * other slot becomes the target; in 9 it selects only when nothing is. */
void func_ov007_020b6134(int arg){
    Ov007Work *G = (Ov007Work *)data_ov007_0210342c;
    switch (G->states[1]->currentState) {
    case 0:
        func_ov007_020aec94(arg + 3);
        break;
    case 3:
        G->selectedSlot = arg;
        break;
    case 2:
        G->targetSlot = arg;
        break;
    case 7: {
        int v = G->selectedSlot;
        if (v == -1) {
            G->selectedSlot = arg;
            return;
        }
        if (arg == v)
            G->selectedSlot = -1;
        else
            G->targetSlot = arg;
    }
    break;
  case 9:
      if (G->selectedSlot == -1)
          G->selectedSlot = arg;
      break;
  }
}
}
