/* storage 0x023858AC runtime 0x037FD744 size 0x80 wram ARM. SND channel unlink
 * (ChannelCallback tail): flag at +0x22, next at +0x50, head at +0x3C.
 * Struct fields defeat the head-address CSE; walk is SDK ChannelCallback shape. */

extern void fn_023843C0(int a1);

struct Node { char p[0x22]; unsigned char flag; char q[0x2D]; struct Node *next; };
struct Track { char p[0x3C]; struct Node *head; };

void fn_023858AC(struct Node *a1, int a2, struct Track *a3) {
    struct Node *cur;
    struct Node *nxt;
    if (a2 == 1) {
        a1->flag = 1;
        fn_023843C0((int)a1);
    } else if (a2 == 2) {
        a1->flag = 0;
        fn_023843C0((int)a1);
    }
    if (a3->head == a1) {
        a3->head = a1->next;
    } else {
        cur = a3->head;
        while ((nxt = cur->next) != 0) {
            if (nxt == a1) {
                cur->next = a1->next;
                return;
            }
            cur = nxt;
        }
    }
}
