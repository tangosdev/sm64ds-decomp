//cpp
extern "C" {
extern void _ZN5Scene19ResetFadersAndSoundEv(void);
}

struct Stage {
    void BeforeInitResources(void);
};

void Stage::BeforeInitResources(void)
{
_ZN5Scene19ResetFadersAndSoundEv();

}
