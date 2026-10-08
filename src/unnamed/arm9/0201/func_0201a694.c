
extern unsigned char data_0209d4e0;
extern int data_0208ee4c;
extern void func_0201a798(int id);
extern int func_0203d7b8(void);
extern int GetSceneOverlayID(int id);
extern void func_0201a754(int id);
extern int overlay_75;

int func_0201a694(int id)
{
    int ov;
    int old;

    if ((id == 6) && (data_0209d4e0 == 0)) {
        func_0201a798((int)&overlay_75);
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