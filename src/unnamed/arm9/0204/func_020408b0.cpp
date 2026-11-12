//cpp
#include "Memory.h"
#include "private/wm_views.h"
#include "nitro/mb/mb.h"

extern "C" {

extern int WM_Init(void *buf, int ch);

extern unsigned short data_020a0f14;
extern int data_020a0f94;
extern unsigned short data_020a0f24;
extern unsigned short data_020a0f2c;
extern void *data_020a0f4c;
extern void *data_020a0f44;
extern void *data_020a0f48;
extern WMBssDesc *data_020a0f54;
extern unsigned short data_020a0f28;
extern WMDataSharingInfo *data_020a0f74;
extern WMDataSet *data_020a0f80;
extern unsigned int data_020a0f58;
extern void *data_020a0f60;
extern unsigned int data_020a0f64;
extern void *data_020a0f68;
/* The 0x40-byte WMParentParam template func_0203db64 keeps. */
extern unsigned char data_020a1064[];

/* Allocate the link's buffers. On a download-play child, rebuild the parent's WMBssDesc into data_020a0f54, keep its
   channel in data_020a0f28 (and in data_020a1064's channel) and step its tgid. */
void func_020408b0(unsigned short arg) {
    data_020a0f14 = arg;
    data_020a0f94 = 0;
    data_020a0f24 = 0;
    data_020a0f2c = 1;
    data_020a0f4c = Memory::Allocate(0x1300, 0x20);
    data_020a0f44 = Memory::Allocate(0x40, 0x20);
    data_020a0f48 = Memory::Allocate(0x20, 0x20);
    data_020a0f54 = (WMBssDesc *)Memory::Allocate(sizeof(WMBssDesc), 0x20);
    if (MB_IsMultiBootChild()) {
        MB_ReadMultiBootParentBssDesc(data_020a0f54, 0x200, 0x20, 0, 1);
        data_020a0f28 = data_020a0f54->channel;
        ++data_020a0f54->gameInfo.tgid;
    }
    data_020a0f74 = (WMDataSharingInfo *)Memory::Allocate(sizeof(WMDataSharingInfo), 0x20);
    data_020a0f80 = (WMDataSet *)Memory::Allocate(sizeof(WMDataSet), 0x20);
    data_020a0f58 = 0x220;
    data_020a0f60 = Memory::Allocate(0x220, 0x20);
    data_020a0f64 = 0x480;
    data_020a0f68 = Memory::Allocate(0x480, 0x20);
    ((WmParentParam *)data_020a1064)->channel = data_020a0f28;
    WM_Init(data_020a0f4c, 1);
}
}
