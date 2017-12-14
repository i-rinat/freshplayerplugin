/*
 * Copyright © 2013-2017  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "keycodeconvert.h"
#include "keycodepp.h"

#define XK_3270

#include <X11/XF86keysym.h>
#include <X11/keysym.h>
#include <ppapi/c/ppb_input_event.h>

int
xkeycode_to_pp_keycode(int xkeycode)
{
    switch (xkeycode) {
    case XK_Break:
        return VK_CANCEL;
    case XK_BackSpace:
        return VK_BACK;
    case XK_Tab:
    case XK_ISO_Left_Tab:
        return VK_TAB;
    case XK_Clear:
        return VK_CLEAR;
    case XK_Return:
    case XK_KP_Enter:
    case XK_ISO_Enter:
    case XK_3270_Enter:
        return VK_RETURN;
    case XK_Pause:
        return VK_PAUSE;
    case XK_Caps_Lock:
        return VK_CAPITAL;
    case XK_Kana_Lock:
    case XK_Kana_Shift:
        return VK_KANA;
    case XK_Hangul:
        return VK_HANGUL;
    case XK_Hangul_Hanja:
        return VK_HANJA;
    case XK_Kanji:
        return VK_KANJI;
    case XK_Escape:
        return VK_ESCAPE;
    case XK_space:
        return VK_SPACE;
    case XK_Page_Up:
        return VK_PRIOR;
    case XK_Page_Down:
        return VK_NEXT;
    case XK_End:
        return VK_END;
    case XK_Home:
        return VK_HOME;
    case XK_Left:
        return VK_LEFT;
    case XK_Up:
        return VK_UP;
    case XK_Right:
        return VK_RIGHT;
    case XK_Down:
        return VK_DOWN;
    case XK_Select:
        return VK_SELECT;
    case XK_Print:
        return VK_PRINT;
    case XK_Execute:
        return VK_EXECUTE;
    case XK_Sys_Req:
        return VK_SNAPSHOT;
    case XK_Insert:
        return VK_INSERT;
    case XK_Delete:
        return VK_DELETE;
    case XK_Help:
        return VK_HELP;
    // digits
    case XK_1:
    case XK_exclam:
        return '1';
    case XK_2:
    case XK_at:
        return '2';
    case XK_3:
    case XK_numbersign:
        return '3';
    case XK_4:
    case XK_dollar:
        return '4';
    case XK_5:
    case XK_percent:
        return '5';
    case XK_6:
    case XK_asciicircum:
        return '6';
    case XK_7:
    case XK_ampersand:
        return '7';
    case XK_8:
    case XK_asterisk:
        return '8';
    case XK_9:
    case XK_parenleft:
        return '9';
    case XK_0:
    case XK_parenright:
        return '0';
    // letters
    case XK_a:
    case XK_A:
        return 'A';
    case XK_b:
    case XK_B:
        return 'B';
    case XK_c:
    case XK_C:
        return 'C';
    case XK_d:
    case XK_D:
        return 'D';
    case XK_e:
    case XK_E:
        return 'E';
    case XK_f:
    case XK_F:
        return 'F';
    case XK_g:
    case XK_G:
        return 'G';
    case XK_h:
    case XK_H:
        return 'H';
    case XK_i:
    case XK_I:
        return 'I';
    case XK_j:
    case XK_J:
        return 'J';
    case XK_k:
    case XK_K:
        return 'K';
    case XK_l:
    case XK_L:
        return 'L';
    case XK_m:
    case XK_M:
        return 'M';
    case XK_n:
    case XK_N:
        return 'N';
    case XK_o:
    case XK_O:
        return 'O';
    case XK_p:
    case XK_P:
        return 'P';
    case XK_q:
    case XK_Q:
        return 'Q';
    case XK_r:
    case XK_R:
        return 'R';
    case XK_s:
    case XK_S:
        return 'S';
    case XK_t:
    case XK_T:
        return 'T';
    case XK_u:
    case XK_U:
        return 'U';
    case XK_v:
    case XK_V:
        return 'V';
    case XK_w:
    case XK_W:
        return 'W';
    case XK_x:
    case XK_X:
        return 'X';
    case XK_y:
    case XK_Y:
        return 'Y';
    case XK_z:
    case XK_Z:
        return 'Z';

    case XK_Super_L:
        return VK_LWIN;
    case XK_Super_R:
        return VK_RWIN;
    case XK_Menu:
        return VK_APPS;
    case XK_KP_0:
        return VK_NUMPAD0;
    case XK_KP_1:
        return VK_NUMPAD1;
    case XK_KP_2:
        return VK_NUMPAD2;
    case XK_KP_3:
        return VK_NUMPAD3;
    case XK_KP_4:
        return VK_NUMPAD4;
    case XK_KP_5:
        return VK_NUMPAD5;
    case XK_KP_6:
        return VK_NUMPAD6;
    case XK_KP_7:
        return VK_NUMPAD7;
    case XK_KP_8:
        return VK_NUMPAD8;
    case XK_KP_9:
        return VK_NUMPAD9;
    case XK_KP_Multiply:
        return VK_MULTIPLY;
    case XK_KP_Add:
        return VK_ADD;
    case XK_KP_Separator:
        return VK_SEPARATOR;
    case XK_KP_Subtract:
        return VK_SUBTRACT;
    case XK_KP_Decimal:
        return VK_DECIMAL;
    case XK_KP_Divide:
        return VK_DIVIDE;

    case XK_F1:
        return VK_F1;
    case XK_F2:
        return VK_F2;
    case XK_F3:
        return VK_F3;
    case XK_F4:
        return VK_F4;
    case XK_F5:
        return VK_F5;
    case XK_F6:
        return VK_F6;
    case XK_F7:
        return VK_F7;
    case XK_F8:
        return VK_F8;
    case XK_F9:
        return VK_F9;
    case XK_F10:
        return VK_F10;
    case XK_F11:
        return VK_F11;
    case XK_F12:
        return VK_F12;
    case XK_F13:
        return VK_F13;
    case XK_F14:
        return VK_F14;
    case XK_F15:
        return VK_F15;
    case XK_F16:
        return VK_F16;
    case XK_F17:
        return VK_F17;
    case XK_F18:
        return VK_F18;
    case XK_F19:
        return VK_F19;
    case XK_F20:
        return VK_F20;
    case XK_F21:
        return VK_F21;
    case XK_F22:
        return VK_F22;
    case XK_F23:
        return VK_F23;
    case XK_F24:
        return VK_F24;
    case XK_Num_Lock:
        return VK_NUMLOCK;
    case XK_Scroll_Lock:
        return VK_SCROLL;
    case XK_Shift_L:
    case XK_Shift_R:
        return VK_SHIFT;
    case XK_Control_L:
    case XK_Control_R:
        return VK_CONTROL;
    case XK_Meta_L:
    case XK_Meta_R:
    case XK_Alt_L:
    case XK_Alt_R:
        return VK_MENU;
    case XF86XK_Back:
        return VK_BROWSER_BACK;
    case XF86XK_Forward:
        return VK_BROWSER_FORWARD;
    case XF86XK_Refresh:
        return VK_BROWSER_REFRESH;
    case XF86XK_Stop:
        return VK_BROWSER_STOP;
    case XF86XK_Search:
        return VK_BROWSER_SEARCH;
    case XF86XK_Favorites:
        return VK_BROWSER_FAVORITES;
    case XF86XK_HomePage:
        return VK_BROWSER_HOME;
    case XF86XK_AudioMute:
        return VK_VOLUME_MUTE;
    case XF86XK_AudioLowerVolume:
        return VK_VOLUME_DOWN;
    case XF86XK_AudioRaiseVolume:
        return VK_VOLUME_UP;
    case XF86XK_AudioNext:
        return VK_MEDIA_NEXT_TRACK;
    case XF86XK_AudioPrev:
        return VK_MEDIA_PREV_TRACK;
    case XF86XK_AudioStop:
        return VK_MEDIA_STOP;
    case XF86XK_AudioPlay:
        return VK_MEDIA_PLAY_PAUSE;
    case XF86XK_Mail:
        return VK_LAUNCH_MAIL;
    case XF86XK_LaunchA:
        return VK_LAUNCH_APP1;
    case XF86XK_LaunchB:
        return VK_LAUNCH_APP2;

    case XK_semicolon:
    case XK_colon:
        return VK_OEM_1;
    case XK_plus:
    case XK_equal:
        return VK_OEM_PLUS;
    case XK_comma:
    case XK_less:
        return VK_OEM_COMMA;
    case XK_minus:
    case XK_underscore:
        return VK_OEM_MINUS;
    case XK_period:
    case XK_greater:
        return VK_OEM_PERIOD;
    case XK_slash:
    case XK_question:
        return VK_OEM_2;
    case XK_quoteleft:
    case XK_asciitilde:
        return VK_OEM_3;
    case XK_bracketleft:
    case XK_braceleft:
        return VK_OEM_4;
    case XK_backslash:
    case XK_bar:
        return VK_OEM_5;
    case XK_bracketright:
    case XK_braceright:
        return VK_OEM_6;
    case XK_quoteright:
    case XK_quotedbl:
        return VK_OEM_7;
    case XK_3270_Attn:
        return VK_ATTN;
    case XK_3270_CursorSelect:
        return VK_CRSEL;
    case XK_3270_ExSelect:
        return VK_EXSEL;
    case XK_3270_EraseEOF:
        return VK_EREOF;
    case XK_3270_Play:
        return VK_PLAY;
    case XK_3270_PA1:
        return VK_PA1;
    default:
        return 0;
    }

}

int
get_left_right_pp_flag(int xkeycode)
{
    switch (xkeycode) {

    case XK_Shift_L:
    case XK_Control_L:
    case XK_Meta_L:
    case XK_Alt_L:
    case XK_Super_L:
        return PP_INPUTEVENT_MODIFIER_ISLEFT;

    case XK_Shift_R:
    case XK_Control_R:
    case XK_Meta_R:
    case XK_Alt_R:
    case XK_Super_R:
        return PP_INPUTEVENT_MODIFIER_ISRIGHT;

    default:
        return 0;
    }
}
