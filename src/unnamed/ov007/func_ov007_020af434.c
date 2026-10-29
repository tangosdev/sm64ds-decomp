#include "Ov007Work.h"

extern void func_ov007_020b283c(void);
extern void func_ov007_020b76a0(void);
extern void func_ov007_020bcba8(int i);
extern void func_ov007_020bca40(int i);
extern void func_ov007_020bd1c0(int i);
extern void RequestScreenFade(int a, int b, int c);
extern void func_ov007_020aec00(void);
extern void func_ov007_020c1d78(int i);
extern char data_ov007_0210342c[];

/* The 0x38-byte BG layer record; scroll names as in func_ov007_020b131c,
 * only the slots this file clears. */
typedef struct Ov007BgLayer {
    char pad_00[0x20];
    int fineScrollX; /* 0x20: Q12 */
    int fineScrollY; /* 0x24 */
    int scrollX;     /* 0x28: Q12 */
    int scrollY;     /* 0x2c */
} Ov007BgLayer;

/* The manager's +0x74: the 2D display record func_ov007_020b7594
 * allocates; only its BG layer table. */
typedef struct Ov007Display {
    char pad_00[0x34];
    /* 0x34: eight func_ov007_020c95fc BG layers, each wrapped by
     * func_ov007_020adc08 into a layer slot */
    Ov007BgLayer **bgLayers;
} Ov007Display;

typedef char Ov007BgLayer_size_must_be_0x30[sizeof(Ov007BgLayer) == 0x30 ? 1 : -1];
typedef char Ov007Display_size_must_be_0x38[sizeof(Ov007Display) == 0x38 ? 1 : -1];

void func_ov007_020af434(void) {
    Ov007BgLayer **layers;
    Ov007BgLayer *a;
    (*(void (**)(int))(*(char **)data_ov007_0210342c + 0x2c))(0x12);
    func_ov007_020b283c();
    func_ov007_020b76a0();
    func_ov007_020bcba8(3);
    func_ov007_020bca40(3);
    func_ov007_020bd1c0(2);
    RequestScreenFade(1, 2, 0);
    func_ov007_020aec00();
    layers = (*(Ov007Work **)data_ov007_0210342c)->display->bgLayers;
    a = layers[6];
    a->scrollX = 0;
    a->scrollY = 0;
    a->fineScrollY = 0;
    a->fineScrollX = a->fineScrollY;
    a = layers[7];
    a->scrollX = 0;
    a->scrollY = 0;
    a->fineScrollY = 0;
    a->fineScrollX = a->fineScrollY;
    func_ov007_020c1d78(0);
}
