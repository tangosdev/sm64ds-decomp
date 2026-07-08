//cpp
struct MinigameSaveData;
struct Obj { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual int m5(); };
extern "C" {
extern unsigned char data_0209b300;
extern Obj* SCENE_FADER;
extern MinigameSaveData data_0209cae4;
extern unsigned char data_0209f1d8;
}
namespace SaveData { void SaveMinigames(MinigameSaveData*); }
extern "C" void func_ov005_020bff4c(void)
{
    if (data_0209b300 != 1) return;
    if (SCENE_FADER->m5() == 0) return;
    SaveData::SaveMinigames(&data_0209cae4);
    data_0209b300 = 0;
    data_0209f1d8 = 0;
}
