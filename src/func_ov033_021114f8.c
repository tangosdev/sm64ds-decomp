extern int _ZN5Event6GetBitEj();
extern int _ZN5Sound15PlaySecretSoundEP5ActorPt();
extern void _ZN9ActorBase18MarkForDestructionEv();
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j();
extern void _ZN7Minimap19UpdateLevelSpecificEv();
extern void _ZN9Animation7AdvanceEv();
extern void _ZN8Platform21UpdateModelPosAndRotYEv();
extern void _ZN8Platform19UpdateClsnPosAndRotEv();

int func_ov033_021114f8(int *t)
{
    if (_ZN5Event6GetBitEj(0xe)) {
        if (t[0x18] <= t[0xcd]) {
            if (_ZN5Sound15PlaySecretSoundEP5ActorPt(t, (char*)t + 0x33c)) {
                _ZN9ActorBase18MarkForDestructionEv(t);
            }
        } else {
            int *p = (int*)(((int)t + 0x60) & 0xFFFFFFFFFFFFFFFFLL);
            *p -= 0x1000;
            t[0xce] = _ZN5Sound8PlayLongEjjjRK7Vector3j(t[0xce], 3, 0x96, (char*)t + 0x74, 0);
            if (t[0x18] <= t[0xcd]) {
                t[0x18] = t[0xcd];
                _ZN7Minimap19UpdateLevelSpecificEv();
            }
        }
    }
    t[0xcb] = 0x1000;
    _ZN9Animation7AdvanceEv((char*)t + 0x320);
    _ZN8Platform21UpdateModelPosAndRotYEv(t);
    _ZN8Platform19UpdateClsnPosAndRotEv(t);
    return 1;
}
