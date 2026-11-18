/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e504..0x0208e538, 52 bytes, 1 configured symbols.
 */
#include "types.h"

typedef struct {
    char c[52];
} Buf52;

/* 0x0208e504: 52 bytes. */
Buf52 data_0208e504 = {"CPSR00R01R02R03R04R05R06R07R08R09R10R11R12SP LR PC "};
