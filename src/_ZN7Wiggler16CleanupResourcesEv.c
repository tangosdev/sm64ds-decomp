/* _ZN7Wiggler16CleanupResourcesEv at 0x02112a5c
 *
 * Releases all five entries of each of the seven SharedFilePtr tables this
 * actor pulled, then returns 1.
 */

extern void _ZN13SharedFilePtr7ReleaseEv(void *self);

extern void *data_ov034_02113838[];
extern void *data_ov034_0211384c[];
extern void *data_ov034_02113860[];
extern void *data_ov034_02113874[];
extern void *data_ov034_02113888[];
extern void *data_ov034_0211389c[];
extern void *data_ov034_021138b0[];

int _ZN7Wiggler16CleanupResourcesEv(void)
{
    int i;
    for (i = 0; i < 5; i++) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov034_02113838[i]);
        _ZN13SharedFilePtr7ReleaseEv(data_ov034_0211384c[i]);
        _ZN13SharedFilePtr7ReleaseEv(data_ov034_02113860[i]);
        _ZN13SharedFilePtr7ReleaseEv(data_ov034_02113874[i]);
        _ZN13SharedFilePtr7ReleaseEv(data_ov034_02113888[i]);
        _ZN13SharedFilePtr7ReleaseEv(data_ov034_0211389c[i]);
        _ZN13SharedFilePtr7ReleaseEv(data_ov034_021138b0[i]);
    }
    return 1;
}
