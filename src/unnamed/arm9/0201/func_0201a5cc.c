#include "types.h"

extern void LoadOverlay(int id);
extern void UnloadOverlay(int id);
extern int OVERLAY_2_ID;
extern int OVERLAY_3_ID;
extern int OVERLAY_4_ID;
extern int OVERLAY_5_ID;
extern int OVERLAY_6_ID;
extern int OVERLAY_7_ID;
extern int OVERLAY_75_ID;
extern unsigned char data_0209d4e0;
extern int data_0208ee4c;
extern int func_0203d7b8(void);

int func_0201a694(int actorID);
void func_0201a614(int actorID);
void func_0201a754(int id);
void func_0201a798(int id);
int GetSceneOverlayID(int id);
int IsMinigameActorID(unsigned int id);

int IsMinigameActorID(unsigned int id) {
    if (id >= 0x169 && id <= 0x186)
        return 1;
    return 0;
}

int GetSceneOverlayID(int id) {
    if (IsMinigameActorID(id))
        return (int)&OVERLAY_6_ID;

    switch (id) {
    case 3:
    case 6:
    case 7:
        return (int)&OVERLAY_2_ID;
    case 5:
        return (int)&OVERLAY_5_ID;
    case 1:
        return (int)&OVERLAY_7_ID;
    case 2:
    case 4:
    case 8:
        return (int)&OVERLAY_3_ID;
    case 0x168:
        break;
    }
    return -1;
}

/* Load overlay id, if any (-1 is none); overlay 6 loads overlay 4 first. */
void func_0201a798(int id) {
    if (id == -1)
        return;
    if (id == (int)&OVERLAY_6_ID)
        LoadOverlay((int)&OVERLAY_4_ID);
    LoadOverlay(id);
}

/* Unload overlay id, if any (-1 is none); overlay 6 takes overlay 4 out
   with it. */
void func_0201a754(int id) {
    if (id == -1)
        return;
    UnloadOverlay(id);
    if (id != (int)&OVERLAY_6_ID)
        return;
    UnloadOverlay((int)&OVERLAY_4_ID);
}

/* The create hook func_0201a5cc installs: before process id is created
   (func_02043060), load its scene overlay (GetSceneOverlayID), unless it
   is the one the delete hook kept loaded, which is taken over instead.
   The overlay kept before it is unloaded. Id 6 also loads overlay 75
   once. A failed load (func_0203d7b8) returns 3, which stops the create;
   else 2. */
int func_0201a694(int id) {
    int ov;
    int old;

    if ((id == 6) && (data_0209d4e0 == 0)) {
        func_0201a798((int)&OVERLAY_75_ID);
        if (func_0203d7b8()) {
            return 3;
        }
        data_0209d4e0 = 1;
    }
    ov = GetSceneOverlayID(id);
    if (ov != (-1)) {
        old = data_0208ee4c;
        if (ov == old) {
            data_0208ee4c = -1;
        } else {
            if (old != (-1)) {
                func_0201a754(old);
                data_0208ee4c = -1;
            }
            func_0201a798(ov);
            if (func_0203d7b8()) {
                return 3;
            }
        }
    }
    return 2;
}

/* The delete hook func_0201a5cc installs (func_0204302c, after a process
   is cleaned up): keep the process's scene overlay loaded in data_0208ee4c
   for the next create hook, unloading the one kept before. Deleting id 1
   also unloads overlay 75. */
void func_0201a614(int id) {
    if (id == 1 && data_0209d4e0 != 0) {
        func_0201a754((int)&OVERLAY_75_ID);
        data_0209d4e0 = 0;
    }
    id = GetSceneOverlayID(id);
    if (id == -1) {
        return;
    }
    if (data_0208ee4c != -1) {
        func_0201a754(data_0208ee4c);
    }
    data_0208ee4c = id;
}

/* Run both create hooks for one scene ID. */
void func_0201a5f8(int actorID) {
    func_0201a694(actorID);
    func_0201a614(actorID);
}

void func_0201a5cc(void) {
    extern int (*data_020a4b58)(int actorID);
    extern void (*data_020a4b5c)(int actorID);
    data_020a4b58 = func_0201a694;
    data_020a4b5c = func_0201a614;
}
