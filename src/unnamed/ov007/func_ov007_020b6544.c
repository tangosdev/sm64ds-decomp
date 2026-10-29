extern void func_ov007_020c1d8c(int i);
extern void func_ov007_020b20e8(void);
extern void func_ov007_020b4b5c(int a, int b);

/* data_ov007_0210342c: the overlay's manager; names as in
 * func_ov007_020b1718, func_ov007_020b1cf0, func_ov007_020b1fa4,
 * func_ov007_020b3360, func_ov007_020aec94 (pendingChoice),
 * func_ov007_020b0358 (pendingCapture) and func_ov007_020aed98
 * (chosenItemId). S keeps the extern's old spelling. Only the slots this
 * file touches. f0 and f14 keep their old names: no reader names them. */
struct S {
    int f0; /* 0x00 */
    char pad_04[0xc];
    /* 0x10: the choice func_ov007_020aec94 was last given;
     * func_ov007_020b0834 replays it once the state-9 window closes */
    int pendingChoice;
    int f14; /* 0x14 */
    /* 0x18: func_ov007_020b6eb4 starts a display capture while it is set */
    int pendingCapture;
    int pendingBgMode;  /* 0x1c: func_ov007_020b6eb4 hands it to func_ov007_020bd648 */
    int transitionMode; /* 0x20: func_ov007_020b1cf0 sets 1 for a stylus tap, 0 for a button */
    /* 0x24, medium confidence: the preset func_ov007_020b1fa4 last applied */
    int cameraPreset;
    char pad_28[8];
    void *cameras[2]; /* 0x30: func_ov007_020c023c's Ov007Camera */
    char pad_38[0xdc];
    int itemViews[0x18]; /* 0x114: func_ov007_020b693c, one per func_ov007_020b4c04 */
    char pad_174[0xc];
    /* 0x180: the id of the item chosen this frame, -1 = none */
    int chosenItemId;
};

typedef char S_chosenItemId_must_be_at_0x180[(int)&((struct S *)0)->chosenItemId == 0x180 ? 1 : -1];

extern struct S *data_ov007_0210342c;

/* Resets the manager's per-visit state, re-aims camera 1
 * (func_ov007_020b20e8) and puts every item view back at its home point
 * under it. */
void func_ov007_020b6544(void)
{
    func_ov007_020c1d8c(0);
    data_ov007_0210342c->pendingCapture = 0;
    data_ov007_0210342c->pendingBgMode = 1;
    data_ov007_0210342c->pendingChoice = 0;
    data_ov007_0210342c->f14 = 0;
    data_ov007_0210342c->f0 = 0;
    data_ov007_0210342c->chosenItemId = -1;
    data_ov007_0210342c->cameraPreset = 0;
    data_ov007_0210342c->transitionMode = 1;
    func_ov007_020b20e8();
    int i;
    for (i = 0; i < 0x18; i++) {
        func_ov007_020b4b5c(data_ov007_0210342c->itemViews[i],
                            (int)data_ov007_0210342c->cameras[1]);
    }
}
