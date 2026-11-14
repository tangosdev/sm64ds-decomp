#include "MWCPlusLib.h"

extern int func_02071844(ActionIterator *info, int pc);
extern struct EHIndexEntry *func_02072ed0(struct EHIndexEntry *base, int count, unsigned int key);
extern unsigned char *func_0207180c(unsigned char *p);
extern unsigned char *func_02071a50(unsigned char *p, int *out);

/* EH runtime: find the action table that covers the code address pc. The
   index entries {start, size with an inline flag in bit 0, table} are
   searched with func_02072ed0. The table is stored inline in the entry
   when bit 0 is set, otherwise it is a pointer. After the header
   (func_0207180c) come {gap, length, action offset} varint records. The
   record covering pc sets info->mAction to the table plus its action
   offset; the table and action pointers stay 0 when nothing covers it. */
void func_02072dac(int pc, ActionIterator *info) {
    struct EHIndexEntry *e;
    unsigned char *p;
    int gap, length, actionOffset;
    unsigned int end;
    unsigned int acc;
    unsigned int tmp;

    info->mActionTable = 0;
    info->mAction = 0;
    if (!func_02071844(info, pc))
        return;
    e = func_02072ed0(info->indexBegin, info->indexEnd - info->indexBegin, (unsigned int)pc);
    if (!e)
        return;
    if (e->sizeFlags & 1)
        info->mActionTable = (unsigned char *)e + 8;
    else
        info->mActionTable = (unsigned char *)e->table;
    info->mFuncStart = e->start;
    end = (unsigned int)(pc - e->start);
    p = func_0207180c(info->mActionTable);
    acc = 0;
    do {
        p = func_02071a50(p, &gap);
        if (gap == 0)
            return;
        p = func_02071a50(p, &length);
        p = func_02071a50(p, &actionOffset);
        tmp = acc + (unsigned int)gap;
        if (end < tmp)
            return;
        acc = tmp + (unsigned int)length;
    } while (end > acc);
    info->mAction = info->mActionTable + actionOffset;
}
