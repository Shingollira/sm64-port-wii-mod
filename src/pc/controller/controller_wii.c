#ifdef TARGET_GX
#ifdef __wii__

#define Mtx __Mtx
#define Vtx __Vtx
#define guPerspectiveF __guPerspectiveF
#define guPerspective __guPerspective
#define guOrtho __guOrtho
#define guOrthoF __guOrthoF
#include <wiiuse/wpad.h>
#undef Mtx
#undef Vtx
#undef guPerspectiveF
#undef guPerspective
#undef guOrtho
#undef guOrthoF

#include <ultra64.h>
#include "controller_api.h"
#include "../configfile.h"
#include <ogcsys.h>

static int nunchuk_center_x = 0;
static int nunchuk_center_y = 0;
static bool nunchuk_calibrated = false;
static int exit_hold_timer = 0;
static uint32_t controller_wii_get_held(void)
{
    WPAD_ScanPads();
    return WPAD_ButtonsHeld(0);
}

static void controller_wii_init(void)
{
    PAD_Init();
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(WPAD_CHAN_0, 640, 480);
}

static void controller_wii_read(OSContPad *pad)
{
    PAD_ScanPads();
u16 gcHeld = PAD_ButtonsHeld(0);

    WPAD_ScanPads();

    uint32_t held = WPAD_ButtonsHeld(0);
    pad->button = 0;
    pad->stick_x = 0;
    pad->stick_y = 0;

    if (held & WPAD_BUTTON_HOME) {
        exit_hold_timer++;
        if (exit_hold_timer > 60) {
            SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
        }
    } else {
        exit_hold_timer = 0;
    }


    struct expansion_t data;
    WPAD_Expansion(WPAD_CHAN_0, &data);

    if (data.type == WPAD_EXP_NONE)
    {
        if (held & WPAD_BUTTON_2) pad->button |= A_BUTTON;
        if (held & WPAD_BUTTON_1) pad->button |= B_BUTTON;
        if (held & WPAD_BUTTON_PLUS) pad->button |= START_BUTTON;
        if (held & WPAD_BUTTON_B) pad->button |= Z_TRIG;
        if (held & WPAD_BUTTON_A) pad->button |= R_TRIG;

        int digital_speed = 64;

        if (held & WPAD_BUTTON_UP)    pad->stick_x = -digital_speed;
        if (held & WPAD_BUTTON_DOWN)  pad->stick_x =  digital_speed;
        if (held & WPAD_BUTTON_RIGHT) pad->stick_y =  digital_speed;
        if (held & WPAD_BUTTON_LEFT)  pad->stick_y = -digital_speed;
    }

    else if (data.type == WPAD_EXP_NUNCHUK)
    {
        if (held & WPAD_BUTTON_A) pad->button |= A_BUTTON;
        if (held & WPAD_BUTTON_B) pad->button |= B_BUTTON;
        if (held & WPAD_BUTTON_PLUS) pad->button |= START_BUTTON;
        if (held & WPAD_BUTTON_MINUS) pad->button |= L_TRIG;
        if (held & WPAD_NUNCHUK_BUTTON_Z) pad->button |= Z_TRIG;
        if (held & WPAD_NUNCHUK_BUTTON_C) pad->button |= R_TRIG;

        if (held & WPAD_BUTTON_UP)    pad->button |= U_CBUTTONS;
        if (held & WPAD_BUTTON_DOWN)  pad->button |= D_CBUTTONS;
        if (held & WPAD_BUTTON_LEFT)  pad->button |= L_CBUTTONS;
        if (held & WPAD_BUTTON_RIGHT) pad->button |= R_CBUTTONS;

        if (!nunchuk_calibrated) {
            nunchuk_center_x = data.nunchuk.js.pos.x;
            nunchuk_center_y = data.nunchuk.js.pos.y;
            nunchuk_calibrated = true;
        }

        int raw_x = data.nunchuk.js.pos.x - nunchuk_center_x;
        int raw_y = data.nunchuk.js.pos.y - nunchuk_center_y;

        int deadzone = 400;
        int magnitude = raw_x * raw_x + raw_y * raw_y;

        if (magnitude < deadzone) {
            raw_x = 0;
            raw_y = 0;
        }

        pad->stick_x = raw_x;
        pad->stick_y = raw_y;
    }

    else if (data.type == WPAD_EXP_CLASSIC)
    {
        if (held & WPAD_CLASSIC_BUTTON_A) pad->button |= A_BUTTON;
        if (held & WPAD_CLASSIC_BUTTON_B) pad->button |= B_BUTTON;
        if (held & WPAD_CLASSIC_BUTTON_PLUS) pad->button |= START_BUTTON;
        if (held & WPAD_CLASSIC_BUTTON_FULL_L) pad->button |= Z_TRIG;
        if (held & WPAD_CLASSIC_BUTTON_FULL_R) pad->button |= R_TRIG;

        pad->stick_x = 2 * (data.classic.ljs.pos.x - data.classic.ljs.center.x);
        pad->stick_y = 2 * (data.classic.ljs.pos.y - data.classic.ljs.center.y);

        s8 deadzone = 10;

        s8 ssx = data.classic.rjs.pos.x - data.classic.rjs.center.x;
        s8 ssy = data.classic.rjs.pos.y - data.classic.rjs.center.y;

        if (ssx > deadzone)  pad->button |= R_CBUTTONS;
        if (ssx < -deadzone) pad->button |= L_CBUTTONS;
        if (ssy > deadzone)  pad->button |= U_CBUTTONS;
        if (ssy < -deadzone) pad->button |= D_CBUTTONS;
    }
}

struct ControllerAPI controller_wii = {
    controller_wii_init,
    controller_wii_read
};

#endif
#endif
