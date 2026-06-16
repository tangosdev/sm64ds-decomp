extern int data_0208a174[];
extern void func_ov005_020c0030(int);
extern void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);
int func_ov005_020c1654(int c)
{
    if (data_0208a174[0] >= 0) {
        func_ov005_020c0030(c);
    }
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    return 1;
}
