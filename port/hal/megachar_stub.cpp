// UpdateKillByMegaChar: STUBBED for gate 9. The real body is the
// Mega-Mario squash check whose closure (RaycastLine methods,
// Actor::UpdatePos) belongs to the Player gates; with no mega char in
// play the real function takes its early-out, which is what 0 means.
extern "C" int _ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(
    void *, short, short, short, int)
{
    return 0;
}
