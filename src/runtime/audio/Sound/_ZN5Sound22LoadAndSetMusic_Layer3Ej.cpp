//cpp
extern "C" {
extern void func_02011dcc(int player, unsigned int musicId);
extern unsigned int data_0208e444;
extern int data_0209b4a0[];
}

class Sound {
public:
    static void LoadAndSetMusic_Layer3(unsigned int musicId);
};

void Sound::LoadAndSetMusic_Layer3(unsigned int musicId)
{
    data_0208e444 = musicId;
    func_02011dcc((int)data_0209b4a0, musicId);
}
