/* menu_pause_art.cpp -- the pause menu's host pictures (run menu1, lane PAUSE3): the registry behind hostimg's extension ids 32..45.
   Run hunt8, lane UIMOD1: the pictures are files of the "HD UI Textures" mod (hd_ui/menu/pause3/<name>, see ui_art.h); this file holds
   only their names. Order is the id order of menu_pause_art_ids.h. */
#include "menu_hostimg.h"
#include "menu_pause_art_ids.h"

static const char *const k_pause_art[] = {
    "pause3/pk_resume.png", "pause3/pk_settings.png", "pause3/pk_exit.png", "pause3/pk_savequit.png", "pause3/pk_quitnosave.png",
    "pause3/pk_yesquit.png", "pause3/pk_nokeep.png", "pause3/title_blank.png", "pause3/tab_l.png", "pause3/tab_r.png",
    "pause3/star_got.png", "pause3/star_off.png", "pause3/coin_gold.png", "pause3/coin_red.png",
    "online/btn_online.png", "online/btn_adventure_online.png", "online/btn_vs.png", "online/btn_help.png", "online/btn_back.png", "online/btn_cancel.png", "online/btn_solo.png",
    "online/btn_wrench.png",
};

extern "C" int menu_ext_art_src(int id, const char **file)
{
    const int k = id - MENU_IMG_EXT_BASE;
    if (k < 0 || k >= (int)(sizeof k_pause_art / sizeof k_pause_art[0])) return 0;
    *file = k_pause_art[k];
    return 1;
}

extern "C" int menu_ext_art_is_plank(int id)
{
    return (id >= MENU_IMG_P_PK_RESUME && id <= MENU_IMG_P_PK_NOKEEP) || (id >= MENU_IMG_ONL_ONLINE && id <= MENU_IMG_ONL_SOLO);
}
