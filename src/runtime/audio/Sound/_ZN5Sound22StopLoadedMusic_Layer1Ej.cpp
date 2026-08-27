//cpp
extern "C" {
extern void func_0204fa2c(int player, unsigned int arg);
extern unsigned int data_0209b4ac;
extern int data_0209b4a0[];
}

class Sound {
public:
    static void StopLoadedMusic_Layer1(unsigned int arg);
};

void Sound::StopLoadedMusic_Layer1(unsigned int arg)
{
    data_0209b4ac = ~0u;
    func_0204fa2c((int)data_0209b4a0, arg);
}
