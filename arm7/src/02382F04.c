/* storage 0x02382F04 runtime 0x037FAD9C size 0x120 (code 0x114 + 3-word pool) wram ARM.
 * IRQ dispatch loop: polls MMIO 0x4000184 (busy 0x4000 -> v2=-3, else lock and
 * test 0x100 -> v2=-4 exit, else latch word v from 0x4100000 -> v2=0), then
 * calls table unk_3807004[v.b.idx] as h(idx, rest, b5) when present; with no
 * handler it acks via 0x4000188 (v.w|0x20) unless b5 is set. The packed word
 * is a bitfield union: bitfield extracts codegen as literal LSL/LSR pairs
 * (an explicit (v<<27)>>27 folds to AND under -O4,s) and keep v spilled to
 * the stack slot. Under lock fn_02382AC4/fn_02382AD8. Three pool words (two
 * numeric MMIO bases + the table ABS32 reloc). */

extern int fn_02382AC4(void); /* =0x037FA95C lock */
extern int fn_02382AD8(int); /* =0x037FA970 unlock */
extern int unk_3807004[]; /* =0x03807004 */

typedef struct {
    unsigned int idx:5;
    unsigned int b5:1;
    unsigned int rest:26;
} irq_bits_t;

typedef union {
    unsigned int w;
    irq_bits_t b;
} irq_word_t;

typedef void (*irq_handler_t)(unsigned int, unsigned int, unsigned int);

void fn_02382F04(void) {
    irq_word_t v;
    int v2;
    while (1) {
        if (*(volatile unsigned short *)0x4000184 & 0x4000) {
            *(volatile unsigned short *)0x4000184 |= 0xC000;
            v2 = -3;
        } else {
            int lk = fn_02382AC4();
            if (*(volatile unsigned short *)0x4000184 & 0x100) {
                fn_02382AD8(lk);
                v2 = -4;
            } else {
                v.w = *(volatile unsigned int *)0x4100000;
                fn_02382AD8(lk);
                v2 = 0;
            }
        }
        if (v2 == -4)
            break;
        if (v2 == -3)
            continue;
        if (v.b.idx == 0)
            continue;
        {
            irq_handler_t h = (irq_handler_t)unk_3807004[v.b.idx];
            if (h != 0) {
                h(v.b.idx, v.b.rest, v.b.b5);
                continue;
            }
        }
        if (v.b.b5 != 0)
            continue;
        v.w |= 0x20;
        if (*(volatile unsigned short *)0x4000184 & 0x4000) {
            *(volatile unsigned short *)0x4000184 |= 0xC000;
            continue;
        }
        {
            int lk = fn_02382AC4();
            if (*(volatile unsigned short *)0x4000184 & 2) {
                fn_02382AD8(lk);
                continue;
            }
            *(volatile unsigned int *)0x4000188 = v.w;
            fn_02382AD8(lk);
        }
    }
}
