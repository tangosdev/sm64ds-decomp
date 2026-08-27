//cpp
struct Sound { static void PlayBank2_2D(unsigned int id); };
struct Animation { int Finished(); int WillHitFrame(int f) const; };
struct Model { void HideMaterial(int a, int b); void ShowMaterial(int a, int b); };

extern "C" void func_ov006_020c0df0(char *c)
{
    if (*(int *)(c + 0x268) == 0) {
        Sound::PlayBank2_2D(0x139);
        *(int *)(c + 0x268) = 1;
    }
    if (*(int *)(c + 0x7c) == *(int *)(c + 0x23c) && ((Animation *)(c + 0x6c))->Finished()) {
        ((Model *)(c + 0x1c))->HideMaterial(1, 0);
        ((Model *)(c + 0x1c))->ShowMaterial(2, 0);
        return;
    }
    if (((Animation *)(c + 0x6c))->WillHitFrame(0x14))
        Sound::PlayBank2_2D(0x156);
}
