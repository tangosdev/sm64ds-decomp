//cpp
struct Vector3;
struct Actor { static Actor *FindWithID(unsigned int id); };
struct Sound { static void PlayBank3(unsigned int id, const Vector3 &v); };

extern "C" void func_ov029_021117ac(char *c)
{
    if (*(unsigned char *)(c + 0x15d) != 0)
        return;
    unsigned int id = *(unsigned int *)(c + 0x148);
    if (id == 0)
        return;
    char *a = (char *)Actor::FindWithID(id);
    if (a == 0)
        return;
    int b = (int)(*(unsigned short *)(a + 0xc) == 0xbf);
    if (b == 0)
        return;
    char *o = (char *)Actor::FindWithID(*(unsigned int *)(c + 0x158));
    int v = *(int *)(o + 0x334);
    if (v != *(int *)(o + 0x60))
        return;
    if (v == *(int *)(c + 0x60))
        return;
    *(unsigned char *)(c + 0x15d) = 1;
    *(int *)(o + 0x334) = *(int *)(c + 0x60);
    Sound::PlayBank3(0x63, *(Vector3 *)(c + 0x74));
}
