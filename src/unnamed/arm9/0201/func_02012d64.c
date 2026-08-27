/* func_02012d64 at 0x02012d64
 * Advances the music volume toward a target using ApproachLinear,
 * then updates the NDS sound hardware with the new volume.
 */

typedef int Fix12i;
typedef signed char s8;
typedef int s32;

extern int _Z14ApproachLinearRiii(Fix12i *val, Fix12i target, Fix12i step); /* 0x0203ae58 */
extern void func_02013524(void *fileRef, s32 vol, s32 mode);                /* 0x02013524 */

extern s8   data_0208e42c;   /* 0x0208e42c */
extern Fix12i data_0209b494;   /* 0x0209b494 */
extern Fix12i data_0209b490; /* 0x0209b490 */
extern void *data_0209b4a0; /* data at 0x0209b4a0 */

void func_02012d64(void)
{
    _Z14ApproachLinearRiii(&data_0209b490,
                           (Fix12i)((s32)data_0208e42c << 12),
                           data_0209b494);
    func_02013524(&data_0209b4a0,
                  data_0209b490 >> 12,
                  0);
}
