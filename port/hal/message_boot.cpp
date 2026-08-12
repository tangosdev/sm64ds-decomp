// The message DATA layer, host side.
//
// WHAT THIS REPLACES. Until now hal/level_boot.cpp seated data_0209d70c on a
// zeroed 0x28-byte stand-in so func_0201f32c's own bounds check
//
//     if (*(u16 *)((char *)data_0209d70c + 8) <= (u16)arg0) return;
//
// declined every message id (count = 0). That was a null-deref guard, not a
// real archive. This file loads the real bank instead.
//
// WHAT THE REAL LOAD IS. Two matched functions do it on the DS and both are
// trivial:
//
//   LoadMessageBankForLanguage (func_0201fe08): LoadFile(asset) by language,
//     stash the decompressed base in gMessageBankBase (data_0209d6fc), then
//     ParseMessageBankSections.
//   ParseMessageBankSections (func_0201cb2c): pin the section pointers off the
//     base -- info = base+0x20, entries = base+0x30, data = info + info.size.
//
// They are NOT in any port slice, and linking them would drag in two globals
// nothing downstream reads (gMessageBankHeader/data_0209d6e8 and
// gMessageDataSection/data_0209d6ec are WRITTEN by the parser but read by no
// matched TU, and neither is in hal/auto_bss). So this is a HOST COPY of the
// two: the same arithmetic, writing only the four globals the message code
// actually reads back -- data_0209d6fc (base), data_0209d708 (entries),
// data_0209d70c (info header) and, for completeness, the two dead ones too so
// a future TU that does read them finds them right.
//
// THE FILE. build/assets resolves asset 0x0431 (English; GetOwnerLanguage is 0
// on host) to extracted/dsd/files/data/message/msg_data_eng.bin. That file is
// LZ77-compressed on disk; the port's LoadFile decompresses it to a 0x16640
// image whose header is exactly MessageBankHeader (0x20) + MessageInfoSection
// (magic "1FNI", sectionSize at +4, messageCount 0x2C7 = 711 at +8). See
// include/MessageBank.h for the layout and the byte-level confirmation.
//
// After this runs, func_0201f32c's bounds check passes for id < 711 and its
// matched body runs for real. The text ITSELF is a glyph-index byte stream
// (0xfd/0xfe/0xff control codes, everything else an AddChar glyph index, see
// src/func_0201b7cc.c) -- not ASCII -- so port_message_dump below logs the id,
// the resolved text address, the raw entry, and a hex+printable dump of the
// entry's bytes rather than pretending to decode a string.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "MessageBank.h"

extern "C" {

/* The port's file loader (hal/level_boot.cpp): LoadFile(handle) -> decompressed
   bytes, cached one SharedFilePtr per handle. This is the same entry the
   matched LoadMessageBankForLanguage calls. */
void *LoadFile(int handle);
/* hal/level_boot.cpp: keep this handle's image across level teardowns */
void port_loadfile_pin_persistent(int handle);
int GetOwnerLanguage(void);

/* The bank globals. All five are hal/auto_bss int[8] except the two the parser
   writes but nothing reads, which are declared here and defined at the bottom
   of this file so the host copy is complete without touching auto_bss. */
extern int data_0209d6fc[];   /* gMessageBankBase   */
extern int data_0209d708[];   /* gMessageEntries    */
extern int data_0209d70c[];   /* gMessageInfoSection (the header func_0201f32c reads) */

/* Written by the real parser, read by nothing matched. Defined below so the
   host copy stores everything the ROM parser does. */
int data_0209d6e8;            /* gMessageBankHeader */
int data_0209d6ec;            /* gMessageDataSection */

void port_message_dump(int id); /* defined below */

} /* extern "C" */

/* The five western banks, by GetOwnerLanguage (0 eng, 2 frn, 3 gmn, 4 itl,
   5 spn). Same mapping LoadMessageBankForLanguage encodes. */
static int port_message_asset_for_language(void)
{
    switch (GetOwnerLanguage()) {
    case 5: return MESSAGE_BANK_SPN_ASSET;
    case 4: return MESSAGE_BANK_ITL_ASSET;
    case 3: return MESSAGE_BANK_GMN_ASSET;
    case 2: return MESSAGE_BANK_FRN_ASSET;
    default: return MESSAGE_BANK_ENG_ASSET;
    }
}

/* HOST COPY of LoadMessageBankForLanguage (func_0201fe08) +
   ParseMessageBankSections (func_0201cb2c). Loads the real bank and pins the
   section pointers. Returns 1 on success, 0 if the file did not load. */
extern "C" int port_message_bank_load(void)
{
    int asset = port_message_asset_for_language();
    char *base = (char *)LoadFile(asset);
    if (!base) {
        std::fprintf(stderr, "[msg] LoadFile(0x%x) returned no bytes; the "
                     "message bank did not load\n", asset);
        return 0;
    }
    /* The bank is a global, not a level's file: it is loaded once here and its
       section pointers stay pinned in the globals below for the whole run. Pin
       its LoadFile slot so the per-level teardown (port_level_release_files)
       does not free it on the first level change and leave the message system
       on a freed block. */
    port_loadfile_pin_persistent(asset);

    /* Parse: the exact arithmetic ParseMessageBankSections does. */
    char *info = base + 0x20;
    char *entries = base + 0x30;
    char *data = info + *(int *)(info + 4);

    data_0209d6fc[0] = (int)(size_t)base;    /* gMessageBankBase */
    data_0209d6e8 = (int)(size_t)base;       /* gMessageBankHeader */
    data_0209d70c[0] = (int)(size_t)info;    /* gMessageInfoSection */
    data_0209d708[0] = (int)(size_t)entries; /* gMessageEntries */
    data_0209d6ec = (int)(size_t)data;       /* gMessageDataSection */

    {
        const MessageInfoSection *is = (const MessageInfoSection *)info;
        std::printf("[msg] bank loaded: asset 0x%x, base %p, %u messages, "
                    "info magic '%.4s', section size 0x%x\n",
                    asset, (void *)base, (unsigned)is->messageCount,
                    is->magic, (unsigned)is->sectionSize);
    }

    /* SM64DS_MSG_DUMP=1 (or a comma list of ids) dumps message content at load
       so content retrieval is demonstrable without reaching a sign headless.
       Bare "1" dumps a fixed sample; a list dumps exactly those ids. */
    if (const char *d = std::getenv("SM64DS_MSG_DUMP")) {
        void (*dump)(int) = port_message_dump;
        if (std::strchr(d, ',') || (d[0] >= '0' && d[0] <= '9' &&
                                    std::atoi(d) > 1)) {
            const char *p = d;
            while (*p) {
                dump(std::atoi(p));
                const char *c = std::strchr(p, ',');
                if (!c) break;
                p = c + 1;
            }
        } else if (*d && *d != '0') {
            /* a representative sample: message 0, the BoB tutorial band, and a
               couple mid-bank ids, just to show the resolver walking entries */
            static const int sample[] = {0, 1, 2, 8, 0x2a, 0x83, 100, 500};
            for (unsigned i = 0; i < sizeof sample / sizeof sample[0]; ++i)
                dump(sample[i]);
        }
    }
    return 1;
}

/* Resolve the on-image text address for message `id`, the same way every
   Message::Display* does:
     text = base + 0x28 + infoSection.sectionSize + entry[id].textOffset
   entry[id] is data_0209d708 + id*8. Returns 0 if the bank is not loaded or id
   is out of range. Also hands back the entry pointer. */
static const unsigned char *port_message_text_addr(int id,
                                                   const MessageEntry **entry_out)
{
    const char *info = (const char *)(size_t)data_0209d70c[0];
    const char *base = (const char *)(size_t)data_0209d6fc[0];
    const char *entries = (const char *)(size_t)data_0209d708[0];
    if (!info || !base || !entries)
        return 0;
    const MessageInfoSection *is = (const MessageInfoSection *)info;
    if (id < 0 || (unsigned)id >= is->messageCount)
        return 0;
    const MessageEntry *e = (const MessageEntry *)(entries + id * 8);
    if (entry_out) *entry_out = e;
    /* base + 0x28 + sectionSize + textOffset (func_0201f32c line 91-92, the
       Display* line data_0209d6f4 = base + 0x28 + info[1] + entry->textOffset). */
    return (const unsigned char *)(base + 0x28 + is->sectionSize + e->textOffset);
}

/* Log the requested message id and its resolved text bytes. The text is a
   glyph-index stream, not ASCII (see the file header), so this dumps the raw
   entry and the entry's bytes up to the 0xff terminator as hex plus whatever
   is printable ASCII, which is enough to prove the right entry resolved and
   see any embedded control codes. */
extern "C" void port_message_dump(int id)
{
    const MessageEntry *e = 0;
    const unsigned char *t = port_message_text_addr(id, &e);
    if (!t) {
        const char *info = (const char *)(size_t)data_0209d70c[0];
        if (!info) {
            std::fprintf(stderr, "[msg] dump id %d: no bank loaded\n", id);
        } else {
            unsigned n = ((const MessageInfoSection *)info)->messageCount;
            std::fprintf(stderr, "[msg] dump id %d: out of range (%u messages)\n",
                         id, n);
        }
        return;
    }

    std::fprintf(stderr, "[msg] id %d: entry textOffset=0x%x info=%02x %02x "
                 "%02x %02x  text@%p\n", id, (unsigned)e->textOffset,
                 e->info[0], e->info[1], e->info[2], e->info[3], (const void *)t);

    /* Walk to the 0xff terminator, capped, printing hex and DECODED text. The
       encoding is not ASCII: AddChar (src/_ZN7Message7AddCharEc.cpp) turns each
       byte into a font tile index. The Latin glyph run is contiguous --
       upper A..Z = 0x0a..0x23, lower a..z = 0x2d..0x46, space = 0x4d, digits
       0x00..0x09 -- so the common letters decode cleanly; anything outside that
       is shown as {hh}. Control codes: 0xfd newline, 0xfe embedded-function
       (MSG_GEN_TEXT_FUNCS, see the note in the report), 0xff end. This is a
       decode aid, not the font's authority -- the font file is. */
    enum { CAP = 160 };
    char hex[CAP * 3 + 1];
    char txt[CAP * 4 + 1];
    int n = 0, tp = 0;
    for (; n < CAP; ++n) {
        unsigned char c = t[n];
        std::snprintf(hex + n * 3, 4, "%02x ", c);
        if (c == 0xff) { txt[tp++] = '\0'; ++n; break; }
        else if (c == 0xfd) { txt[tp++] = '\\'; txt[tp++] = 'n'; }
        else if (c == 0xfe) { tp += std::snprintf(txt + tp, 5, "[FN]"); }
        else if (c >= 0x0a && c <= 0x23) txt[tp++] = (char)('A' + (c - 0x0a));
        else if (c >= 0x2d && c <= 0x46) txt[tp++] = (char)('a' + (c - 0x2d));
        else if (c <= 0x09) txt[tp++] = (char)('0' + c);
        else if (c == 0x4d) txt[tp++] = ' ';
        else tp += std::snprintf(txt + tp, 5, "{%02x}", c);
        if (tp > CAP * 4 - 6) break;
    }
    hex[n * 3] = 0;
    txt[tp < CAP * 4 ? tp : CAP * 4] = 0;
    std::fprintf(stderr, "[msg]   bytes (%d): %s\n", n, hex);
    std::fprintf(stderr, "[msg]   text : \"%s\"\n", txt);
}
