// The game card's ROM read path: ROMCTRL, the command latch and the data port.
//
// Run linkfull, lane S4CARD: the first stage-4 hardware model. The ROM's own
// card driver (src/func_02060918.c and the fifteen TUs behind it, listed in
// port/slice_w28_card.txt) reads the cartridge by talking to three registers,
// and this file is the cartridge on the other end of them. Register semantics
// are from GBATEK ("DS Cartridge I/O Ports", "DS Cartridge Protocol"); no SDK
// source is reproduced.
//
//   0x040001A0  AUXSPICNT  16-bit. Bit 15 slot enable, bit 14 "transfer ready
//                          IRQ", bit 13 0 = ROM mode. src/func_02060d98.c
//                          writes its high byte as 0xC0 before every command.
//   0x040001A4  ROMCTRL    32-bit. Bit 31 = start (write) / busy (read), bits
//                          24-26 the block size (1 = 0x200 bytes), bit 23 =
//                          "a data word is ready" (read only).
//   0x040001A8  the eight command bytes, sent most significant first.
//   0x04100010  the data port: every 32-bit read hands out the next word of
//               the block. It is OUTSIDE the mapped I/O window (ntr/mmio.h),
//               so it only exists through the hook table in ntr/io.cpp.
//
// THE ONE COMMAND THIS GAME SENDS is B7h, "get data": bytes 1..4 are the ROM
// address, big-endian, and the cartridge answers with a block from there.
// GBATEK: an address below 0x8000 (the secure area and the header) is answered
// from 0x8000 + (addr & 0x1FF) instead. src/func_02060a64.c is the whole
// protocol on the ARM9 side:
//
//     func_02060d98((page >> 8) | 0xB7000000, page << 24);   // the command
//     ROMCTRL = cmd;                                          // bit 31: go
//     do { s = ROMCTRL; if (s & bit 23) buf[i++] = DATA; } while (s & bit 31);
//
// so a model that sets bits 31 and 23 on the start store, answers 128 data-port
// reads out of one 0x200-byte page and then clears both bits is the whole of the
// cartridge this driver ever sees. `cmd` is func_02060b64's
// (header +0x60 & ~0x07000000) | 0xA1000000: block size 1, 0x200 bytes. Other
// sizes are served too (4-KB pages wrap, per GBATEK) so a later caller gets the
// cartridge's answer rather than a refusal; nothing in this game sends one.
//
// THE PAGES COME FROM hal/fs_names.cpp's virtual ROM image
// (port_nitrofs_rom_read): the FNT, FAT and ARM9 overlay table the catalog
// already cuts out of the cartridge, and every FAT-ranged file out of
// extracted/dsd/files, with 0xFF where none of those lie. There is no 16 MB
// array: a page is assembled when it is asked for. port/tools/card_image_check.py
// compares every page a run served (SM64DS_CARD_TRACE=1) against a real .nds.
//
// AT THE END OF A BLOCK the model raises IF bit 19, "game card data transfer
// completion", when AUXSPICNT bit 14 asks for it, as the DS does. Nothing
// dispatches it on this path: IE bit 19 is only enabled by func_02060b64's DMA
// arm, which never runs while FS_Init's channel argument is -1 (the port's
// pre-main FS_Init; see hal/fs_names.cpp). IF bit 20 (IREQ_MC, card removal) is
// never raised. The CPU path runs on the ROM's own card thread, so a read costs
// two fiber switches and no host thread.
//
// WHAT IT COUNTS, for the lane's proof rows, printed at exit under
// SM64DS_CARD_TRACE (every block, with the page's FNV-1a 64 digest) or
// SM64DS_CARD_STATS (the summary line only): blocks served, words read,
// blocks cut short or overrun, data-port reads with no block in flight, status
// reads that found the busy bit up with no data ready (a spin), starts made
// while EXMEMCNT bit 11 gave the slot to the ARM7, IF-19 raises and how many of
// them IE would have let through, and commands other than B7h.
//
// None of this is savestate storage. A block starts and finishes inside one
// call of the ROM's own read loop, which never yields, so a frame boundary never
// finds a transfer in flight; the registers themselves live in the I/O window,
// which the savestate already captures.

#include <stdint.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ntr {
// ntr/io.cpp's hook table; the declarations are repeated here on purpose (the
// note above io_read says why).
typedef bool (*io_hook_read_fn)(uint32_t addr, unsigned width, uint64_t *value);
typedef void (*io_hook_write_fn)(uint32_t addr, uint64_t value, unsigned width);
bool io_hook_add(uint32_t lo, uint32_t hi, io_hook_read_fn rd, io_hook_write_fn wr);
}  // namespace ntr

// hal/fs_names.cpp: `len` bytes of the virtual ROM image at absolute ROM offset
// `addr`, 0xFF where no cartridge-derived source covers them.
extern "C" void port_nitrofs_rom_read(uint32_t addr, uint32_t len, unsigned char *out);

namespace {

constexpr uint32_t REG_AUXSPICNT = 0x040001A0u;
constexpr uint32_t REG_ROMCTRL   = 0x040001A4u;
constexpr uint32_t REG_CARDCMD   = 0x040001A8u;
constexpr uint32_t REG_CARDDATA  = 0x04100010u;
constexpr uint32_t REG_EXMEMCNT  = 0x04000204u;
constexpr uint32_t REG_IE        = 0x04000210u;
constexpr uint32_t REG_IF        = 0x04000214u;

constexpr uint32_t ROMCTRL_BUSY  = 0x80000000u;   // bit 31
constexpr uint32_t ROMCTRL_DRQ   = 0x00800000u;   // bit 23
constexpr uint16_t AUXSPI_IRQ    = 0x4000u;       // AUXSPICNT bit 14
constexpr uint16_t EXMEM_ARM7    = 0x0800u;       // EXMEMCNT bit 11: the ARM7 owns the slot
constexpr uint32_t IRQ_CARD_DONE = 1u << 19;      // IF / IE bit 19
constexpr unsigned kMaxBlock     = 0x4000u;       // block size 6

inline volatile uint32_t &reg32(uint32_t a) {
    return *reinterpret_cast<volatile uint32_t *>(static_cast<uintptr_t>(a));
}
inline volatile uint16_t &reg16(uint32_t a) {
    return *reinterpret_cast<volatile uint16_t *>(static_cast<uintptr_t>(a));
}
inline volatile uint8_t &reg8(uint32_t a) {
    return *reinterpret_cast<volatile uint8_t *>(static_cast<uintptr_t>(a));
}

struct Block {
    bool active;
    uint32_t addr;                // the ROM address the block was read from
    uint32_t words, done;         // words in the block, words handed out
    unsigned char data[kMaxBlock];
};
Block g_blk;

struct Stats {
    unsigned long long blocks, words, cut_short, stray_reads, spins, arm7_slot;
    unsigned long long if19, if19_ie, other_cmds, odd_width, data_writes;
    unsigned long long per_block_ok;   // 0x200-byte blocks whose 128 words all went out
};
Stats g_st;

int g_trace = -1;   // 0 off, 1 summary only, 2 every block too
int trace_level() {
    if (g_trace < 0) {
        const char *t = std::getenv("SM64DS_CARD_TRACE");
        const char *s = std::getenv("SM64DS_CARD_STATS");
        g_trace = (t && *t && std::strcmp(t, "0")) ? 2
                : (s && *s && std::strcmp(s, "0")) ? 1 : 0;
    }
    return g_trace;
}

uint64_t fnv1a64(const unsigned char *p, size_t n) {
    uint64_t h = 0xcbf29ce484222325ull;
    for (size_t i = 0; i < n; ++i) {
        h ^= p[i];
        h *= 0x100000001b3ull;
    }
    return h;
}

void report() {
    std::fprintf(stderr,
                 "[card] summary: blocks=%llu words=%llu words_per_block=%s "
                 "cut_short=%llu stray_data_reads=%llu busy_spins=%llu "
                 "arm7_slot_starts=%llu if19_raised=%llu if19_ie_enabled=%llu "
                 "other_commands=%llu odd_width_reads=%llu data_port_writes=%llu\n",
                 g_st.blocks, g_st.words,
                 g_st.per_block_ok == g_st.blocks ? "128 (every block)" : "MIXED",
                 g_st.cut_short, g_st.stray_reads, g_st.spins, g_st.arm7_slot,
                 g_st.if19, g_st.if19_ie, g_st.other_cmds, g_st.odd_width,
                 g_st.data_writes);
    std::fflush(stderr);
}

// The block's last word has gone out: drop busy and data-ready, and raise the
// completion interrupt the way AUXSPICNT asks.
void finish_block() {
    if (g_blk.done == g_blk.words && g_blk.words == 0x200u / 4u) ++g_st.per_block_ok;
    g_blk.active = false;
    reg32(REG_ROMCTRL) &= ~(ROMCTRL_BUSY | ROMCTRL_DRQ);
    if (reg16(REG_AUXSPICNT) & AUXSPI_IRQ) {
        reg32(REG_IF) |= IRQ_CARD_DONE;
        ++g_st.if19;
        if (reg32(REG_IE) & IRQ_CARD_DONE) ++g_st.if19_ie;
    }
}

// A store with bit 31 up: latch the eight command bytes and fetch the block.
void start_block(uint32_t ctrl) {
    if (g_blk.active) ++g_st.cut_short;   // a new start over an unfinished block
    unsigned char cmd[8];
    for (unsigned i = 0; i < 8; ++i) cmd[i] = reg8(REG_CARDCMD + i);
    if (reg16(REG_EXMEMCNT) & EXMEM_ARM7) ++g_st.arm7_slot;

    const unsigned code = (ctrl >> 24) & 7u;
    const uint32_t bytes = code == 0 ? 0u : code == 7 ? 4u : (0x100u << code);
    uint32_t addr = (uint32_t)cmd[1] << 24 | (uint32_t)cmd[2] << 16 |
                    (uint32_t)cmd[3] << 8 | cmd[4];
    if (cmd[0] == 0xB7) {
        if (addr < 0x8000u) addr = 0x8000u + (addr & 0x1FFu);
        // One block never leaves its 4-KB page: past the end it wraps.
        uint32_t off = addr & 0xFFFu;
        const uint32_t page = addr & ~0xFFFu;
        for (uint32_t got = 0; got < bytes;) {
            const uint32_t n = bytes - got < 0x1000u - off ? bytes - got : 0x1000u - off;
            port_nitrofs_rom_read(page + off, n, g_blk.data + got);
            got += n;
            off = 0;
        }
    } else {
        ++g_st.other_cmds;
        std::memset(g_blk.data, 0xFF, bytes);
        if (g_st.other_cmds == 1)
            std::fprintf(stderr, "[card] command %02x%02x%02x%02x%02x%02x%02x%02x is not "
                         "B7h; answered with 0xFF (this game sends B7h only)\n",
                         cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5], cmd[6], cmd[7]);
    }
    ++g_st.blocks;
    g_blk.addr = addr;
    g_blk.words = bytes / 4u;
    g_blk.done = 0;
    g_blk.active = g_blk.words != 0;
    if (trace_level() >= 2)
        std::fprintf(stderr, "[card] B7 %08x +%x fnv %016llx\n", (unsigned)addr,
                     (unsigned)bytes, (unsigned long long)fnv1a64(g_blk.data, bytes));
    if (g_blk.active)
        reg32(REG_ROMCTRL) |= ROMCTRL_BUSY | ROMCTRL_DRQ;
    else
        finish_block();
}

// ROMCTRL, after the store latched. Only a store that reached the top byte can
// have written bit 31.
void romctrl_write(uint32_t addr, uint64_t, unsigned width) {
    if (addr + width <= REG_ROMCTRL + 3) return;
    const uint32_t ctrl = reg32(REG_ROMCTRL);
    if (ctrl & ROMCTRL_BUSY) start_block(ctrl);
}

// ROMCTRL reads are the latch's; this only counts a status read that finds a
// block in flight with no word ready, which the driver would spin on.
bool romctrl_read(uint32_t, unsigned, uint64_t *) {
    const uint32_t ctrl = reg32(REG_ROMCTRL);
    if ((ctrl & ROMCTRL_BUSY) && !(ctrl & ROMCTRL_DRQ)) ++g_st.spins;
    return false;
}

bool data_read(uint32_t, unsigned width, uint64_t *value) {
    if (width != 4) ++g_st.odd_width;
    if (!g_blk.active) {
        ++g_st.stray_reads;
        *value = 0xFFFFFFFFu;
        return true;
    }
    uint32_t w;
    std::memcpy(&w, g_blk.data + 4u * g_blk.done, 4);
    ++g_blk.done;
    ++g_st.words;
    if (g_blk.done == g_blk.words) finish_block();
    *value = w;
    return true;
}

// A store to the data port is a card WRITE, which no command of this game makes.
void data_write(uint32_t, uint64_t, unsigned) { ++g_st.data_writes; }

struct CardRegister {
    CardRegister() {
        ntr::io_hook_add(REG_ROMCTRL, REG_ROMCTRL + 4, romctrl_read, romctrl_write);
        ntr::io_hook_add(REG_CARDDATA, REG_CARDDATA + 4, data_read, data_write);
        if (trace_level() > 0) std::atexit(report);
    }
};
CardRegister g_card_register;

}  // namespace
