typedef int Bool;

struct Scene;

extern Bool _ZN5Scene19ResetFadersAndSoundEv(struct Scene* self);
extern void _ZN5Scene20Initialise3dGraphicsEv(void);

Bool func_ov006_0210a664(struct Scene* self)
{
    if (!_ZN5Scene19ResetFadersAndSoundEv(self))
        return 0;
    _ZN5Scene20Initialise3dGraphicsEv();
    return 1;
}
