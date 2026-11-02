#include "private/arm9_wbt.h"

/* NitroSDK WBT: peer r2 replied to the running command (self->cmd) with
 * event r1. When the reply carries the command's counter, drop the peer from
 * target_bmp, copy event 3's sync sizes out of the peer's record and raise the
 * command's callback; once no target is left the command slot is cleared. */
void func_02064470(WBTWork *self, int r1, int r2) {
    WBTPeerRecord *e = &self->peer[r2];
    unsigned short mask = 1 << r2;

    self->cmd.peer_bmp = mask;
    if (self->cmd.my_cmd_counter != e->cmd_counter)
        return;
    self->cmd.peer_cmd_counter = e->cmd_counter;
    if ((self->cmd.target_bmp & mask) != 0) {
        self->cmd.target_bmp &= ~mask;

        switch (r1) {
        case 0:
        case 1:
        case 2:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            break;
        case 3:
            self->cmd.u.sync.num_of_list = e->num_of_list;
            self->cmd.u.sync.peer_packet_size = e->peer_packet_size;
            self->cmd.u.sync.my_packet_size = e->my_packet_size;
            break;
        }

        self->cmd.event = r1;
        if (self->cmd.callback != 0) {
            self->cmd.callback((char *)&self->cmd);
        }
    }
    if (self->cmd.target_bmp == 0)
        self->cmd.command = 0;
}
