#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern void func_ov007_020bd9e4(int);
extern int *data_ov007_0210342c;

/* The 0x3c-byte record func_ov007_020b6a84 allocates; only the slot this
 * file names. +0x37 and +0x38 stay raw: they are bytes the callback at
 * Work +0x2c returns for queries 0x14/0x15 (func_ov007_020b066c) and
 * 0x16/0x17 (func_ov007_020b0834), and no reader says what they mean. */
typedef struct Ov007Settings {
    char pad_00[0x39];
    /* 0x39: func_ov007_020b7d94's kind, the window menu state 9 opens before
     * the choice goes through (func_ov007_020b0834); 0 = none
     * (func_ov007_020b066c) */
    unsigned char dialogKind;
    char pad_3a[2];
} Ov007Settings;

typedef char Ov007Settings_size_must_be_0x3c[sizeof(Ov007Settings) == 0x3c ? 1 : -1];

/* The extern keeps its int * spelling; the typed view is reached through a
 * cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

/* Records the menu choice and routes it: choice 9 with either settings flag
 * set, or 7 with +0x38 set, first opens window 1 or 2 through state 9;
 * otherwise choices 3..5 go to state 0xb and the rest to state 6. */
void func_ov007_020aec94(int r4) {
    Ov007Settings *settings = WORK->settings;
    int r5 = (*((unsigned char *)settings + 0x37) == 1);
    int r6 = (*((unsigned char *)settings + 0x38) == 1);
    WORK->pendingChoice = r4;
    func_ov007_020bd9e4(0x50);
    if (r4 == 9) {
        if (r5 != 0 || r6 != 0) {
            WORK->settings->dialogKind = 1;
            WORK->states[1]->requestedState = 9;
            return;
        }
    }
    if (r4 == 7) {
        if (r6 != 0) {
            WORK->settings->dialogKind = 2;
            WORK->states[1]->requestedState = 9;
            return;
        }
    }
    if ((unsigned int)(r4 - 3) <= 2) {
        WORK->states[1]->requestedState = 0xb;
    } else {
        WORK->states[1]->requestedState = 6;
    }
}
