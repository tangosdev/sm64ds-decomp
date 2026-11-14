extern unsigned char data_0209d4e0;
extern int data_0208ee4c;
extern void func_0201a798(int id);
extern int func_0203d7b8(void);
extern int GetSceneOverlayID(int id);
extern void func_0201a754(int id);
extern int OVERLAY_75_ID;

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
