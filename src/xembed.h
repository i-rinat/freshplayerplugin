#ifndef FPP_XEMBED_H
#define FPP_XEMBED_H

// See http://standards.freedesktop.org/xembed-spec/xembed-spec-latest.html for details

/* Flags for _XEMBED_INFO */
#define XEMBED_MAPPED               (1 << 0)

/* XEMBED messages */
#define XEMBED_EMBEDDED_NOTIFY      0
#define XEMBED_WINDOW_ACTIVATE      1
#define XEMBED_WINDOW_DEACTIVATE    2
#define XEMBED_REQUEST_FOCUS        3
#define XEMBED_FOCUS_IN             4
#define XEMBED_FOCUS_OUT            5
#define XEMBED_FOCUS_NEXT           6
#define XEMBED_FOCUS_PREV           7

/* 8-9 were used for XEMBED_GRAB_KEY/XEMBED_UNGRAB_KEY */
#define XEMBED_MODALITY_ON              10
#define XEMBED_MODALITY_OFF             11
#define XEMBED_REGISTER_ACCELERATOR     12
#define XEMBED_UNREGISTER_ACCELERATOR   13
#define XEMBED_ACTIVATE_ACCELERATOR     14

/* Details for XEMBED_FOCUS_IN: */
#define XEMBED_FOCUS_CURRENT        0
#define XEMBED_FOCUS_FIRST          1
#define XEMBED_FOCUS_LAST           2

/* Modifiers field for XEMBED_REGISTER_ACCELERATOR */
#define XEMBED_MODIFIER_SHIFT       (1 << 0)
#define XEMBED_MODIFIER_CONTROL     (1 << 1)
#define XEMBED_MODIFIER_ALT         (1 << 2)
#define XEMBED_MODIFIER_SUPER       (1 << 3)
#define XEMBED_MODIFIER_HYPER       (1 << 4)

/* Flags for XEMBED_ACTIVATE_ACCELERATOR */
#define XEMBED_ACCELERATOR_OVERLOADED   (1 << 0)

/* Directions for focusing */
#define XEMBED_DIRECTION_DEFAULT    0
#define XEMBED_DIRECTION_UP_DOWN    1
#define XEMBED_DIRECTION_LEFT_RIGHT 2

#endif // FPP_XEMBED_H
