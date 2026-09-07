//cpp
// @symbol _ZN7Message10LoadTextVSEv
#include "Message.h"

/* Message::LoadTextVS() at 0x020349e0 -- static, no `this`.
 *
 * Points the VS-mode text blob at the table for the console's language, then hands
 * to SetTextGlobalsVS to split it into the four pointers the renderer reads.
 * The order is a descending chain rather than a switch, and the final `else` is the
 * default rather than a sixth language -- languages 5..2 have their own tables and
 * everything else falls back to 0x02092d3c.
 *
 * EVERY EXTERN HERE NEEDS `extern "C"`. This file was C until this commit, so its
 * declarations had C linkage for free. Migrating it to C++ silently gives them C++
 * linkage instead -- `GetOwnerLanguage` becomes `_Z16GetOwnerLanguagev`, which
 * exists nowhere -- and the failure lands at LINK time, not compile time.
 * build_pin.verify does NOT catch it: a call is a relocation, and match.compare
 * wildcards every relocated word (runbook-reference-repair.md section 1).
 */
extern "C" {
int GetOwnerLanguage(void);
extern int data_020937bc;   /* language 5 */
extern int data_0209325c;   /* language 4 */
extern int data_02093d7c;   /* language 3 */
extern int data_0209289c;   /* language 2 */
extern int data_02092d3c;   /* default */
extern int *data_0209fd14;  /* base of the selected VS text blob */
}

void Message::LoadTextVS()
{
    if (GetOwnerLanguage() == 5) {
        data_0209fd14 = &data_020937bc;
    } else if (GetOwnerLanguage() == 4) {
        data_0209fd14 = &data_0209325c;
    } else if (GetOwnerLanguage() == 3) {
        data_0209fd14 = &data_02093d7c;
    } else if (GetOwnerLanguage() == 2) {
        data_0209fd14 = &data_0209289c;
    } else {
        data_0209fd14 = &data_02092d3c;
    }
    Message::SetTextGlobalsVS();
}
