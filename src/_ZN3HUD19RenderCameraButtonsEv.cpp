//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct OamAttr;

class OAM {
public:
    static OamAttr CAM_BUTTON_L;
    static OamAttr CAM_BUTTON_L_PRESSED;
    static OamAttr CAM_BUTTON_R;
    static OamAttr CAM_BUTTON_R_PRESSED;
    static OamAttr S_CAM_BUTTON_L;
    static OamAttr S_CAM_BUTTON_L_PRESSED;
    static OamAttr S_CAM_BUTTON_R;
    static OamAttr S_CAM_BUTTON_R_PRESSED;
    static OamAttr CAM_ZOOM_BUTTON;
    static OamAttr CAM_ZOOM_BUTTON_PRESSED;
    static OamAttr S_CAM_ZOOM_BUTTON;
    static OamAttr S_CAM_ZOOM_BUTTON_PRESSED;
    static void RenderSub(OamAttr *attr, int x, int y, int a, int b);
};

class HUD {
public:
    static void RenderCameraButtons();
};

typedef struct { u8 pad[0x18]; } Rec18;
typedef struct { u8 pad[0x154]; u32 flags; } S154;

extern "C" {
extern u8 data_020a0e40;
extern u8 data_ov002_02111180;
extern S154 *data_0209f318;
extern Rec18 data_0209f4ae[];
extern int data_0208ee44;
extern u32 data_0209b454;
extern Rec18 data_0209f49c[];
extern u8 data_0209d660;
}

void HUD::RenderCameraButtons()
{
    int idx = data_020a0e40;
    u8 mode = *(u8 *)&data_0209f4ae[idx];
    S154 *p = data_0209f318;
    u8 t = data_ov002_02111180;

    if (t != 0)
        data_ov002_02111180 = t - data_0208ee44;

    if (data_0209b454 & 0x40000000)
        return;

    if (p->flags & 0x1000) {
        if (mode == 0) {
            if (*(u16 *)&data_0209f49c[idx] & 0x200) {
                OAM::RenderSub(&OAM::CAM_BUTTON_L_PRESSED, 0x19, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_L_PRESSED, 0xbf, 0xa5, -1, 1);
            } else {
                OAM::RenderSub(&OAM::CAM_BUTTON_L, 0x19, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_L, 0xbf, 0xa5, -1, 1);
            }
            if (*(u16 *)&data_0209f49c[data_020a0e40] & 0x100) {
                OAM::RenderSub(&OAM::CAM_BUTTON_R_PRESSED, 0x41, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_R_PRESSED, 0xe7, 0xa5, -1, 1);
            } else {
                OAM::RenderSub(&OAM::CAM_BUTTON_R, 0x41, 0xa5, -1, 1);
                OAM::RenderSub(&OAM::CAM_BUTTON_R, 0xe7, 0xa5, -1, 1);
            }
        } else if (mode == 2) {
            if (*(u16 *)&data_0209f49c[idx] & 0x200) {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_L_PRESSED, 0xbf, 0xad, -1, 1);
            } else {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_L, 0xbf, 0xad, -1, 1);
            }
            if (*(u16 *)&data_0209f49c[data_020a0e40] & 0x100) {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_R_PRESSED, 0xe7, 0xad, -1, 1);
            } else {
                OAM::RenderSub(&OAM::S_CAM_BUTTON_R, 0xe7, 0xad, -1, 1);
            }
        }
    }

    if (mode == 0)
        return;
    if (data_0209d660 != 0)
        return;

    int y = (mode == 1) ? 0xa5 : 0x85;
    if (mode == 1) {
        if (data_ov002_02111180 != 0) {
            OAM::RenderSub(&OAM::CAM_ZOOM_BUTTON_PRESSED, 0xe7, y, -1, 1);
            return;
        }
        OAM::RenderSub(&OAM::CAM_ZOOM_BUTTON, 0xe7, y, -1, 1);
        return;
    }
    if (data_ov002_02111180 != 0) {
        OAM::RenderSub(&OAM::S_CAM_ZOOM_BUTTON_PRESSED, 0xe7, y, -1, 1);
        return;
    }
    OAM::RenderSub(&OAM::S_CAM_ZOOM_BUTTON, 0xe7, y, -1, 1);
}
