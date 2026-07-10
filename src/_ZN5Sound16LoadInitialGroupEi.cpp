//cpp
class Sound {
public:
    static void LoadInitialGroup(int group);
    static void LoadGroupAndSetBank(int group, int bank);
};

void Sound::LoadInitialGroup(int group)
{
    LoadGroupAndSetBank(group, 0x20);
}
