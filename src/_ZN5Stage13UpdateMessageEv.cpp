//cpp

typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;

extern u8 data_0209d660;
extern u8 data_0209d654;
extern s16 data_0209d6d4;
extern u8 data_0209d67c;
extern u8 data_0209d670;
extern s32 data_0208ee44;

struct Message {
    static bool UpdateWindow();
    static void Update();
    static void DisplaySaving(u16);
};
struct SaveData {
    static void SaveCurrentFile();
};
struct Stage {
    void UpdateMessage();
};

bool Message::UpdateWindow() { return 0; }
void Message::Update() {}
void Message::DisplaySaving(u16) {}
void SaveData::SaveCurrentFile() {}

void Stage::UpdateMessage() {
    if (data_0209d660 == 0) return;
    
    if (Message::UpdateWindow()) {
        if (data_0209d654 == 0) {
            Message::Update();
            return;
        }
        
        if (data_0209d6d4 == 0x295) {
            if (data_0209d67c == 0x3c) {
                SaveData::SaveCurrentFile();
            }
            if (data_0209d67c != 0) {
                data_0209d67c -= data_0208ee44;
                if (data_0209d67c == 0) {
                    data_0209d670 = 1;
                }
            }
            return;
        }
        
        if (data_0209d67c != 0) {
            data_0209d67c -= data_0208ee44;
            if (data_0209d67c == 0) {
                data_0209d670 = 1;
            }
        }
        return;
    }
    
    if (data_0209d654 != 0) {
        if (data_0209d660 != 0) return;
        if (data_0209d6d4 != 0x295) {
            data_0209d654 = 0;
            return;
        }
        Message::DisplaySaving(0x296);
    }
}
