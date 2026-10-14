extern void func_0201a754(int id);
extern int GetSceneOverlayID(int id);

extern int OVERLAY_75_ID;
extern unsigned char data_0209d4e0;
extern int data_0208ee4c;

void func_0201a614(int id)
{
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
