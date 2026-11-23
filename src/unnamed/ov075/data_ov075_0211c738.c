#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c738_Layout;
typedef char data_ov075_0211c738_size[sizeof(data_ov075_0211c738_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c740_Layout;
typedef char data_ov075_0211c740_size[sizeof(data_ov075_0211c740_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c748_Layout;
typedef char data_ov075_0211c748_size[sizeof(data_ov075_0211c748_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c750_Layout;
typedef char data_ov075_0211c750_size[sizeof(data_ov075_0211c750_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c758_Layout;
typedef char data_ov075_0211c758_size[sizeof(data_ov075_0211c758_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c760_Layout;
typedef char data_ov075_0211c760_size[sizeof(data_ov075_0211c760_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c768_Layout;
typedef char data_ov075_0211c768_size[sizeof(data_ov075_0211c768_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} data_ov075_0211c770_Layout;
typedef char data_ov075_0211c770_size[sizeof(data_ov075_0211c770_Layout) == 0x8 ? 1 : -1];

extern data_ov075_0211c738_Layout data_ov075_0211c738;
extern data_ov075_0211c740_Layout data_ov075_0211c740;
extern data_ov075_0211c748_Layout data_ov075_0211c748;
extern data_ov075_0211c750_Layout data_ov075_0211c750;
extern data_ov075_0211c758_Layout data_ov075_0211c758;
extern data_ov075_0211c760_Layout data_ov075_0211c760;
extern data_ov075_0211c768_Layout data_ov075_0211c768;
extern data_ov075_0211c770_Layout data_ov075_0211c770;
extern u8 _ZN10dScEntry_c23RenderCourseSelectTitleEv[];
extern u8 _ZN10dScEntry_c21UpdateControlSettingsEv[];
extern u8 _ZN10dScEntry_c10UpdateQuitEv[];
extern u8 _ZN10dScEntry_c17UpdateStepFadeOutEv[];
extern u8 _ZN10dScEntry_c20EnterControlSettingsEv[];
extern u8 _ZN10dScEntry_c16UpdatePlayerSyncEv[];
extern u8 _ZN10dScEntry_c17EnterCourseSelectEv[];
extern u8 _ZN10dScEntry_c19UpdateRematchPromptEv[];

#pragma define_section sName ".data.entry_sprite_gap3"                                             \
                             ".data.entry_sprite_gap3"                                             \
                             ".data.entry_sprite_gap3" far_data
#pragma section sName begin

data_ov075_0211c768_Layout data_ov075_0211c768 = {
    &_ZN10dScEntry_c17EnterCourseSelectEv[0],
    0x00000000,
};

data_ov075_0211c760_Layout data_ov075_0211c760 = {
    &_ZN10dScEntry_c16UpdatePlayerSyncEv[0],
    0x00000000,
};

data_ov075_0211c758_Layout data_ov075_0211c758 = {
    &_ZN10dScEntry_c20EnterControlSettingsEv[0],
    0x00000000,
};

data_ov075_0211c750_Layout data_ov075_0211c750 = {
    &_ZN10dScEntry_c17UpdateStepFadeOutEv[0],
    0x00000000,
};

data_ov075_0211c748_Layout data_ov075_0211c748 = {
    &_ZN10dScEntry_c10UpdateQuitEv[0],
    0x00000000,
};

data_ov075_0211c740_Layout data_ov075_0211c740 = {
    &_ZN10dScEntry_c21UpdateControlSettingsEv[0],
    0x00000000,
};

data_ov075_0211c738_Layout data_ov075_0211c738 = {
    &_ZN10dScEntry_c23RenderCourseSelectTitleEv[0],
    0x00000000,
};

data_ov075_0211c770_Layout data_ov075_0211c770 = {
    &_ZN10dScEntry_c19UpdateRematchPromptEv[0],
    0x00000000,
};
