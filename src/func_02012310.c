// Return the active bank-2 sound handle to the timed-switch callers.
extern int Sound_PlayIfNotActive(int, int, int, int);

int func_02012310(int handle, int sound, int arg)
{
    return Sound_PlayIfNotActive(handle, 2, sound, arg);
}
