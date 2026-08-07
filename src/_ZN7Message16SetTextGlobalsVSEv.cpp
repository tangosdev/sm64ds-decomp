//cpp
// @symbol _ZN7Message16SetTextGlobalsVSEv
#include "Message.h"

/* Message::SetTextGlobalsVS() at 0x02032f04 -- static, no `this`.
 *
 * Splits the VS-mode text blob at 0x0209fd14 into the four pointers the renderer
 * reads. The blob's own header carries the offset from its second section to the
 * third, so only three of the four are fixed strides:
 *
 *     +0x00  header      -> data_0209fd04
 *     +0x20  section A   -> data_0209fcf8, and its word at +4 is the distance
 *                           from A to section B
 *     A + *(A+4)         -> data_0209fcfc
 *     +0x30              -> data_0209fd08
 *
 * The intermediate `r2`/`r3` locals are kept because the load of `*(r2 + 4)` has
 * to happen after r2 is formed and before r3 is stored; collapsing them into one
 * expression reorders the loads.
 */
extern "C" {
extern int data_0209fd14;   /* base of the VS text blob */
extern int data_0209fd04;
extern int data_0209fcf8;
extern int data_0209fcfc;
extern int data_0209fd08;
}

void Message::SetTextGlobalsVS()
{
    int ip = data_0209fd14;
    int r2 = ip + 0x20;
    data_0209fd04 = ip;
    data_0209fcf8 = r2;
    int r3 = r2 + *(int*)(r2 + 4);
    data_0209fcfc = r3;
    data_0209fd08 = ip + 0x30;
}
