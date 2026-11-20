#include "Ov007MenuState.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"

extern void func_ov007_020bd9e4(int);

/* Records the menu choice and routes it: choice 9 after either read
 * failure, or 7 after the minigame one, first opens window 1 or 2 through
 * state 9; otherwise choices 3..5 go to state 0xb and the rest to state 6. */
void func_ov007_020aec94(int choice) {
    Ov007Settings *settings = sMenuWork->settings;
    int filesFailed = (settings->filesFailed == 1);
    int minigamesFailed = (settings->minigamesFailed == 1);
    sMenuWork->pendingChoice = choice;
    func_ov007_020bd9e4(80);
    if (choice == 9) {
        if (filesFailed != 0 || minigamesFailed != 0) {
            sMenuWork->settings->dialogKind = 1;
            sMenuWork->states[1]->requestedState = 9;
            return;
        }
    }
    if (choice == 7) {
        if (minigamesFailed != 0) {
            sMenuWork->settings->dialogKind = 2;
            sMenuWork->states[1]->requestedState = 9;
            return;
        }
    }
    if ((unsigned int)(choice - 3) <= 2) {
        sMenuWork->states[1]->requestedState = 0xb;
    } else {
        sMenuWork->states[1]->requestedState = 6;
    }
}
