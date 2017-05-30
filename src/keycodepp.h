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

#pragma once

#define VK_LBUTTON      0x01    // Left mouse button
#define VK_RBUTTON      0x02    // Right mouse button
#define VK_CANCEL       0x03    // Control-break processing
#define VK_MBUTTON      0x04    // Middle mouse button (three-button mouse)
#define VK_XBUTTON1     0x05    // X1 mouse button
#define VK_XBUTTON2     0x06    // X2 mouse button
#define VK_BACK         0x08    // BACKSPACE key
#define VK_TAB          0x09    // TAB key
#define VK_CLEAR        0x0C    // CLEAR key
#define VK_RETURN       0x0D    // ENTER key
#define VK_SHIFT        0x10    // SHIFT key
#define VK_CONTROL      0x11    // CTRL key
#define VK_MENU         0x12    // ALT key
#define VK_PAUSE        0x13    // PAUSE key
#define VK_CAPITAL      0x14    // CAPS LOCK key
#define VK_KANA         0x15    // IME Kana mode
#define VK_HANGUEL      0x15    // IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
#define VK_HANGUL       0x15    // IME Hangul mode
#define VK_JUNJA        0x17    // IME Junja mode
#define VK_FINAL        0x18    // IME final mode
#define VK_HANJA        0x19    // IME Hanja mode
#define VK_KANJI        0x19    // IME Kanji mode
#define VK_ESCAPE       0x1B    // ESC key
#define VK_CONVERT      0x1C    // IME convert
#define VK_NONCONVERT   0x1D    // IME nonconvert
#define VK_ACCEPT       0x1E    // IME accept
#define VK_MODECHANGE   0x1F    // IME mode change request
#define VK_SPACE        0x20    // SPACEBAR
#define VK_PRIOR        0x21    // PAGE UP key
#define VK_NEXT         0x22    // PAGE DOWN key
#define VK_END          0x23    // END key
#define VK_HOME         0x24    // HOME key
#define VK_LEFT         0x25    // LEFT ARROW key
#define VK_UP           0x26    // UP ARROW key
#define VK_RIGHT        0x27    // RIGHT ARROW key
#define VK_DOWN         0x28    // DOWN ARROW key
#define VK_SELECT       0x29    // SELECT key
#define VK_PRINT        0x2A    // PRINT key
#define VK_EXECUTE      0x2B    // EXECUTE key
#define VK_SNAPSHOT     0x2C    // PRINT SCREEN key
#define VK_INSERT       0x2D    // INS key
#define VK_DELETE       0x2E    // DEL key
#define VK_HELP         0x2F    // HELP key
            // 0x30 0
            // 0x31 1
            // 0x32 2
            // 0x33 3
            // 0x34 4
            // 0x35 5
            // 0x36 6
            // 0x37 7
            // 0x38 8
            // 0x39 9
            // 0x41 A
            // 0x42 B
            // 0x43 C
            // 0x44 D
            // 0x45 E
            // 0x46 F
            // 0x47 G
            // 0x48 H
            // 0x49 I
            // 0x4A J
            // 0x4B K
            // 0x4C L
            // 0x4D M
            // 0x4E N
            // 0x4F O
            // 0x50 P
            // 0x51 Q
            // 0x52 R
            // 0x53 S
            // 0x54 T
            // 0x55 U
            // 0x56 V
            // 0x57 W
            // 0x58 X
            // 0x59 Y
            // 0x5A Z
#define VK_LWIN         0x5B
#define VK_RWIN         0x5C
#define VK_APPS         0x5D
#define VK_SLEEP        0x5F
#define VK_NUMPAD0      0x60
#define VK_NUMPAD1      0x61
#define VK_NUMPAD2      0x62
#define VK_NUMPAD3      0x63
#define VK_NUMPAD4      0x64
#define VK_NUMPAD5      0x65
#define VK_NUMPAD6      0x66
#define VK_NUMPAD7      0x67
#define VK_NUMPAD8      0x68
#define VK_NUMPAD9      0x69
#define VK_MULTIPLY     0x6A
#define VK_ADD          0x6B
#define VK_SEPARATOR    0x6C
#define VK_SUBTRACT     0x6D
#define VK_DECIMAL      0x6E
#define VK_DIVIDE       0x6F

#define VK_F1           0x70
#define VK_F2           0x71
#define VK_F3           0x72
#define VK_F4           0x73
#define VK_F5           0x74
#define VK_F6           0x75
#define VK_F7           0x76
#define VK_F8           0x77
#define VK_F9           0x78
#define VK_F10          0x79
#define VK_F11          0x7A
#define VK_F12          0x7B
#define VK_F13          0x7C
#define VK_F14          0x7D
#define VK_F15          0x7E
#define VK_F16          0x7F
#define VK_F17          0x80
#define VK_F18          0x81
#define VK_F19          0x82
#define VK_F20          0x83
#define VK_F21          0x84
#define VK_F22          0x85
#define VK_F23          0x86
#define VK_F24          0x87
#define VK_NUMLOCK      0x90
#define VK_SCROLL       0x91
#define VK_LSHIFT       0xA0
#define VK_RSHIFT       0xA1
#define VK_LCONTROL     0xA2
#define VK_RCONTROL     0xA3
#define VK_LMENU        0xA4
#define VK_RMENU        0xA5

#define VK_BROWSER_BACK         0xA6
#define VK_BROWSER_FORWARD      0xA7
#define VK_BROWSER_REFRESH      0xA8
#define VK_BROWSER_STOP         0xA9
#define VK_BROWSER_SEARCH       0xAA
#define VK_BROWSER_FAVORITES    0xAB
#define VK_BROWSER_HOME         0xAC
#define VK_VOLUME_MUTE          0xAD
#define VK_VOLUME_DOWN          0xAE
#define VK_VOLUME_UP            0xAF
#define VK_MEDIA_NEXT_TRACK     0xB0
#define VK_MEDIA_PREV_TRACK     0xB1
#define VK_MEDIA_STOP           0xB2
#define VK_MEDIA_PLAY_PAUSE     0xB3
#define VK_LAUNCH_MAIL          0xB4
#define VK_LAUNCH_MEDIA_SELECT  0xB5
#define VK_LAUNCH_APP1          0xB6
#define VK_LAUNCH_APP2          0xB7

#define VK_OEM_1        0xBA    // ';:'
#define VK_OEM_PLUS     0xBB    // '+'
#define VK_OEM_COMMA    0xBC    // ','
#define VK_OEM_MINUS    0xBD    // '-'
#define VK_OEM_PERIOD   0xBE    // '.'
#define VK_OEM_2        0xBF    // '/?'
#define VK_OEM_3        0xC0    // '`~'
#define VK_OEM_4        0xDB    // '[{'
#define VK_OEM_5        0xDC    // '\|'
#define VK_OEM_6        0xDD    // ']}'
#define VK_OEM_7        0xDE    // 'single-quote/double-quote' key
#define VK_OEM_8        0xDF    //
#define VK_OEM_102      0xE2    //
#define VK_PROCESSKEY   0xE5    // IME PROCESS key
#define VK_PACKET       0xE7    //
#define VK_ATTN         0xF6    // Attn key
#define VK_CRSEL        0xF7    // CrSel key
#define VK_EXSEL        0xF8    // ExSel key
#define VK_EREOF        0xF9    // Erase EOF key
#define VK_PLAY         0xFA    // Play key
#define VK_ZOOM         0xFB    // Zoom key
#define VK_NONAME       0xFC    // Reserved
#define VK_PA1          0xFD    // PA1 key
#define VK_OEM_CLEAR    0xFE    // Clear key
