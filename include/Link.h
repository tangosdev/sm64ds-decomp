#ifndef LINK_H
#define LINK_H

#include "types.h"

/* The wireless link's status word, gLinkStatusFlags. Its low byte holds
 * the error bits: 0x9f is what ClearLinkErrors clears and 0x9d what
 * callers test for a failed link. Above them sit status bits, among them
 * 0x2000, held while a link step waits for the other consoles. The
 * settings bytes each console posts are LinkSettings.h's.
 *
 * The names are coined; parameters are unnamed as in LinkSettings.h. */
#ifdef __cplusplus
extern "C" {
#endif

u16 GetLinkStatus(u16); /* (mask) the bits of gLinkStatusFlags mask selects */
void ClearLinkErrors(void);

#ifdef __cplusplus
}
#endif

#endif
