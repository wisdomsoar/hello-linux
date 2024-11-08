/*
 * https://www.pacificsimplicity.ca/blog/epoll-devinput-keyboard-press-example/
 * Submitted by admin on Wed, 01/11/2017 - 08:41
 * Epoll /dev/input keyboard press example
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <linux/input.h>

#define MAX_LINE_LENGTH 512
#define MAX_TRIGGER_EVENT_FIELD_LENGTH 256
#define MAX_ENTRIES 100
#define MAXEVENTS 10

#define NAME_ELEMENT(element) [element] = #element

struct TriggerEntry {
    char keycode[MAX_TRIGGER_EVENT_FIELD_LENGTH];
    int pressed;
    char command[MAX_TRIGGER_EVENT_FIELD_LENGTH];
};

struct TriggerEntry entries[MAX_ENTRIES];

static const char * const keys[KEY_MAX + 1] = {
	[0 ... KEY_MAX] = NULL,
	NAME_ELEMENT(KEY_RESERVED),		NAME_ELEMENT(KEY_ESC),
	NAME_ELEMENT(KEY_1),			NAME_ELEMENT(KEY_2),
	NAME_ELEMENT(KEY_3),			NAME_ELEMENT(KEY_4),
	NAME_ELEMENT(KEY_5),			NAME_ELEMENT(KEY_6),
	NAME_ELEMENT(KEY_7),			NAME_ELEMENT(KEY_8),
	NAME_ELEMENT(KEY_9),			NAME_ELEMENT(KEY_0),
	NAME_ELEMENT(KEY_MINUS),		NAME_ELEMENT(KEY_EQUAL),
	NAME_ELEMENT(KEY_BACKSPACE),		NAME_ELEMENT(KEY_TAB),
	NAME_ELEMENT(KEY_Q),			NAME_ELEMENT(KEY_W),
	NAME_ELEMENT(KEY_E),			NAME_ELEMENT(KEY_R),
	NAME_ELEMENT(KEY_T),			NAME_ELEMENT(KEY_Y),
	NAME_ELEMENT(KEY_U),			NAME_ELEMENT(KEY_I),
	NAME_ELEMENT(KEY_O),			NAME_ELEMENT(KEY_P),
	NAME_ELEMENT(KEY_LEFTBRACE),		NAME_ELEMENT(KEY_RIGHTBRACE),
	NAME_ELEMENT(KEY_ENTER),		NAME_ELEMENT(KEY_LEFTCTRL),
	NAME_ELEMENT(KEY_A),			NAME_ELEMENT(KEY_S),
	NAME_ELEMENT(KEY_D),			NAME_ELEMENT(KEY_F),
	NAME_ELEMENT(KEY_G),			NAME_ELEMENT(KEY_H),
	NAME_ELEMENT(KEY_J),			NAME_ELEMENT(KEY_K),
	NAME_ELEMENT(KEY_L),			NAME_ELEMENT(KEY_SEMICOLON),
	NAME_ELEMENT(KEY_APOSTROPHE),		NAME_ELEMENT(KEY_GRAVE),
	NAME_ELEMENT(KEY_LEFTSHIFT),		NAME_ELEMENT(KEY_BACKSLASH),
	NAME_ELEMENT(KEY_Z),			NAME_ELEMENT(KEY_X),
	NAME_ELEMENT(KEY_C),			NAME_ELEMENT(KEY_V),
	NAME_ELEMENT(KEY_B),			NAME_ELEMENT(KEY_N),
	NAME_ELEMENT(KEY_M),			NAME_ELEMENT(KEY_COMMA),
	NAME_ELEMENT(KEY_DOT),			NAME_ELEMENT(KEY_SLASH),
	NAME_ELEMENT(KEY_RIGHTSHIFT),		NAME_ELEMENT(KEY_KPASTERISK),
	NAME_ELEMENT(KEY_LEFTALT),		NAME_ELEMENT(KEY_SPACE),
	NAME_ELEMENT(KEY_CAPSLOCK),		NAME_ELEMENT(KEY_F1),
	NAME_ELEMENT(KEY_F2),			NAME_ELEMENT(KEY_F3),
	NAME_ELEMENT(KEY_F4),			NAME_ELEMENT(KEY_F5),
	NAME_ELEMENT(KEY_F6),			NAME_ELEMENT(KEY_F7),
	NAME_ELEMENT(KEY_F8),			NAME_ELEMENT(KEY_F9),
	NAME_ELEMENT(KEY_F10),			NAME_ELEMENT(KEY_NUMLOCK),
	NAME_ELEMENT(KEY_SCROLLLOCK),		NAME_ELEMENT(KEY_KP7),
	NAME_ELEMENT(KEY_KP8),			NAME_ELEMENT(KEY_KP9),
	NAME_ELEMENT(KEY_KPMINUS),		NAME_ELEMENT(KEY_KP4),
	NAME_ELEMENT(KEY_KP5),			NAME_ELEMENT(KEY_KP6),
	NAME_ELEMENT(KEY_KPPLUS),		NAME_ELEMENT(KEY_KP1),
	NAME_ELEMENT(KEY_KP2),			NAME_ELEMENT(KEY_KP3),
	NAME_ELEMENT(KEY_KP0),			NAME_ELEMENT(KEY_KPDOT),
	NAME_ELEMENT(KEY_ZENKAKUHANKAKU), 	NAME_ELEMENT(KEY_102ND),
	NAME_ELEMENT(KEY_F11),			NAME_ELEMENT(KEY_F12),
	NAME_ELEMENT(KEY_RO),			NAME_ELEMENT(KEY_KATAKANA),
	NAME_ELEMENT(KEY_HIRAGANA),		NAME_ELEMENT(KEY_HENKAN),
	NAME_ELEMENT(KEY_KATAKANAHIRAGANA),	NAME_ELEMENT(KEY_MUHENKAN),
	NAME_ELEMENT(KEY_KPJPCOMMA),		NAME_ELEMENT(KEY_KPENTER),
	NAME_ELEMENT(KEY_RIGHTCTRL),		NAME_ELEMENT(KEY_KPSLASH),
	NAME_ELEMENT(KEY_SYSRQ),		NAME_ELEMENT(KEY_RIGHTALT),
	NAME_ELEMENT(KEY_LINEFEED),		NAME_ELEMENT(KEY_HOME),
	NAME_ELEMENT(KEY_UP),			NAME_ELEMENT(KEY_PAGEUP),
	NAME_ELEMENT(KEY_LEFT),			NAME_ELEMENT(KEY_RIGHT),
	NAME_ELEMENT(KEY_END),			NAME_ELEMENT(KEY_DOWN),
	NAME_ELEMENT(KEY_PAGEDOWN),		NAME_ELEMENT(KEY_INSERT),
	NAME_ELEMENT(KEY_DELETE),		NAME_ELEMENT(KEY_MACRO),
	NAME_ELEMENT(KEY_MUTE),			NAME_ELEMENT(KEY_VOLUMEDOWN),
	NAME_ELEMENT(KEY_VOLUMEUP),		NAME_ELEMENT(KEY_POWER),
	NAME_ELEMENT(KEY_KPEQUAL),		NAME_ELEMENT(KEY_KPPLUSMINUS),
	NAME_ELEMENT(KEY_PAUSE),		NAME_ELEMENT(KEY_KPCOMMA),
	NAME_ELEMENT(KEY_HANGUEL),		NAME_ELEMENT(KEY_HANJA),
	NAME_ELEMENT(KEY_YEN),			NAME_ELEMENT(KEY_LEFTMETA),
	NAME_ELEMENT(KEY_RIGHTMETA),		NAME_ELEMENT(KEY_COMPOSE),
	NAME_ELEMENT(KEY_STOP),			NAME_ELEMENT(KEY_AGAIN),
	NAME_ELEMENT(KEY_PROPS),		NAME_ELEMENT(KEY_UNDO),
	NAME_ELEMENT(KEY_FRONT),		NAME_ELEMENT(KEY_COPY),
	NAME_ELEMENT(KEY_OPEN),			NAME_ELEMENT(KEY_PASTE),
	NAME_ELEMENT(KEY_FIND),			NAME_ELEMENT(KEY_CUT),
	NAME_ELEMENT(KEY_HELP),			NAME_ELEMENT(KEY_MENU),
	NAME_ELEMENT(KEY_CALC),			NAME_ELEMENT(KEY_SETUP),
	NAME_ELEMENT(KEY_SLEEP),		NAME_ELEMENT(KEY_WAKEUP),
	NAME_ELEMENT(KEY_FILE),			NAME_ELEMENT(KEY_SENDFILE),
	NAME_ELEMENT(KEY_DELETEFILE),		NAME_ELEMENT(KEY_XFER),
	NAME_ELEMENT(KEY_PROG1),		NAME_ELEMENT(KEY_PROG2),
	NAME_ELEMENT(KEY_WWW),			NAME_ELEMENT(KEY_MSDOS),
	NAME_ELEMENT(KEY_COFFEE),		NAME_ELEMENT(KEY_DIRECTION),
	NAME_ELEMENT(KEY_CYCLEWINDOWS),		NAME_ELEMENT(KEY_MAIL),
	NAME_ELEMENT(KEY_BOOKMARKS),		NAME_ELEMENT(KEY_COMPUTER),
	NAME_ELEMENT(KEY_BACK),			NAME_ELEMENT(KEY_FORWARD),
	NAME_ELEMENT(KEY_CLOSECD),		NAME_ELEMENT(KEY_EJECTCD),
	NAME_ELEMENT(KEY_EJECTCLOSECD),		NAME_ELEMENT(KEY_NEXTSONG),
	NAME_ELEMENT(KEY_PLAYPAUSE),		NAME_ELEMENT(KEY_PREVIOUSSONG),
	NAME_ELEMENT(KEY_STOPCD),		NAME_ELEMENT(KEY_RECORD),
	NAME_ELEMENT(KEY_REWIND),		NAME_ELEMENT(KEY_PHONE),
	NAME_ELEMENT(KEY_ISO),			NAME_ELEMENT(KEY_CONFIG),
	NAME_ELEMENT(KEY_HOMEPAGE),		NAME_ELEMENT(KEY_REFRESH),
	NAME_ELEMENT(KEY_EXIT),			NAME_ELEMENT(KEY_MOVE),
	NAME_ELEMENT(KEY_EDIT),			NAME_ELEMENT(KEY_SCROLLUP),
	NAME_ELEMENT(KEY_SCROLLDOWN),		NAME_ELEMENT(KEY_KPLEFTPAREN),
	NAME_ELEMENT(KEY_KPRIGHTPAREN), 	NAME_ELEMENT(KEY_F13),
	NAME_ELEMENT(KEY_F14),			NAME_ELEMENT(KEY_F15),
	NAME_ELEMENT(KEY_F16),			NAME_ELEMENT(KEY_F17),
	NAME_ELEMENT(KEY_F18),			NAME_ELEMENT(KEY_F19),
	NAME_ELEMENT(KEY_F20),			NAME_ELEMENT(KEY_F21),
	NAME_ELEMENT(KEY_F22),			NAME_ELEMENT(KEY_F23),
	NAME_ELEMENT(KEY_F24),			NAME_ELEMENT(KEY_PLAYCD),
	NAME_ELEMENT(KEY_PAUSECD),		NAME_ELEMENT(KEY_PROG3),
	NAME_ELEMENT(KEY_PROG4),		NAME_ELEMENT(KEY_SUSPEND),
	NAME_ELEMENT(KEY_CLOSE),		NAME_ELEMENT(KEY_PLAY),
	NAME_ELEMENT(KEY_FASTFORWARD),		NAME_ELEMENT(KEY_BASSBOOST),
	NAME_ELEMENT(KEY_PRINT),		NAME_ELEMENT(KEY_HP),
	NAME_ELEMENT(KEY_CAMERA),		NAME_ELEMENT(KEY_SOUND),
	NAME_ELEMENT(KEY_QUESTION),		NAME_ELEMENT(KEY_EMAIL),
	NAME_ELEMENT(KEY_CHAT),			NAME_ELEMENT(KEY_SEARCH),
	NAME_ELEMENT(KEY_CONNECT),		NAME_ELEMENT(KEY_FINANCE),
	NAME_ELEMENT(KEY_SPORT),		NAME_ELEMENT(KEY_SHOP),
	NAME_ELEMENT(KEY_ALTERASE),		NAME_ELEMENT(KEY_CANCEL),
	NAME_ELEMENT(KEY_BRIGHTNESSDOWN),	NAME_ELEMENT(KEY_BRIGHTNESSUP),
	NAME_ELEMENT(KEY_MEDIA),		NAME_ELEMENT(KEY_UNKNOWN),
	NAME_ELEMENT(KEY_OK),
	NAME_ELEMENT(KEY_SELECT),		NAME_ELEMENT(KEY_GOTO),
	NAME_ELEMENT(KEY_CLEAR),		NAME_ELEMENT(KEY_POWER2),
	NAME_ELEMENT(KEY_OPTION),		NAME_ELEMENT(KEY_INFO),
	NAME_ELEMENT(KEY_TIME),			NAME_ELEMENT(KEY_VENDOR),
	NAME_ELEMENT(KEY_ARCHIVE),		NAME_ELEMENT(KEY_PROGRAM),
	NAME_ELEMENT(KEY_CHANNEL),		NAME_ELEMENT(KEY_FAVORITES),
	NAME_ELEMENT(KEY_EPG),			NAME_ELEMENT(KEY_PVR),
	NAME_ELEMENT(KEY_MHP),			NAME_ELEMENT(KEY_LANGUAGE),
	NAME_ELEMENT(KEY_TITLE),		NAME_ELEMENT(KEY_SUBTITLE),
	NAME_ELEMENT(KEY_ANGLE),		NAME_ELEMENT(KEY_ZOOM),
	NAME_ELEMENT(KEY_MODE),			NAME_ELEMENT(KEY_KEYBOARD),
	NAME_ELEMENT(KEY_SCREEN),		NAME_ELEMENT(KEY_PC),
	NAME_ELEMENT(KEY_TV),			NAME_ELEMENT(KEY_TV2),
	NAME_ELEMENT(KEY_VCR),			NAME_ELEMENT(KEY_VCR2),
	NAME_ELEMENT(KEY_SAT),			NAME_ELEMENT(KEY_SAT2),
	NAME_ELEMENT(KEY_CD),			NAME_ELEMENT(KEY_TAPE),
	NAME_ELEMENT(KEY_RADIO),		NAME_ELEMENT(KEY_TUNER),
	NAME_ELEMENT(KEY_PLAYER),		NAME_ELEMENT(KEY_TEXT),
	NAME_ELEMENT(KEY_DVD),			NAME_ELEMENT(KEY_AUX),
	NAME_ELEMENT(KEY_MP3),			NAME_ELEMENT(KEY_AUDIO),
	NAME_ELEMENT(KEY_VIDEO),		NAME_ELEMENT(KEY_DIRECTORY),
	NAME_ELEMENT(KEY_LIST),			NAME_ELEMENT(KEY_MEMO),
	NAME_ELEMENT(KEY_CALENDAR),		NAME_ELEMENT(KEY_RED),
	NAME_ELEMENT(KEY_GREEN),		NAME_ELEMENT(KEY_YELLOW),
	NAME_ELEMENT(KEY_BLUE),			NAME_ELEMENT(KEY_CHANNELUP),
	NAME_ELEMENT(KEY_CHANNELDOWN),		NAME_ELEMENT(KEY_FIRST),
	NAME_ELEMENT(KEY_LAST),			NAME_ELEMENT(KEY_AB),
	NAME_ELEMENT(KEY_NEXT),			NAME_ELEMENT(KEY_RESTART),
	NAME_ELEMENT(KEY_SLOW),			NAME_ELEMENT(KEY_SHUFFLE),
	NAME_ELEMENT(KEY_BREAK),		NAME_ELEMENT(KEY_PREVIOUS),
	NAME_ELEMENT(KEY_DIGITS),		NAME_ELEMENT(KEY_TEEN),
	NAME_ELEMENT(KEY_TWEN),			NAME_ELEMENT(KEY_DEL_EOL),
	NAME_ELEMENT(KEY_DEL_EOS),		NAME_ELEMENT(KEY_INS_LINE),
	NAME_ELEMENT(KEY_DEL_LINE),
	NAME_ELEMENT(KEY_VIDEOPHONE),		NAME_ELEMENT(KEY_GAMES),
	NAME_ELEMENT(KEY_ZOOMIN),		NAME_ELEMENT(KEY_ZOOMOUT),
	NAME_ELEMENT(KEY_ZOOMRESET),		NAME_ELEMENT(KEY_WORDPROCESSOR),
	NAME_ELEMENT(KEY_EDITOR),		NAME_ELEMENT(KEY_SPREADSHEET),
	NAME_ELEMENT(KEY_GRAPHICSEDITOR), 	NAME_ELEMENT(KEY_PRESENTATION),
	NAME_ELEMENT(KEY_DATABASE),		NAME_ELEMENT(KEY_NEWS),
	NAME_ELEMENT(KEY_VOICEMAIL),		NAME_ELEMENT(KEY_ADDRESSBOOK),
	NAME_ELEMENT(KEY_MESSENGER),		NAME_ELEMENT(KEY_DISPLAYTOGGLE),
#ifdef KEY_SPELLCHECK
	NAME_ELEMENT(KEY_SPELLCHECK),
#endif
#ifdef KEY_LOGOFF
	NAME_ELEMENT(KEY_LOGOFF),
#endif
#ifdef KEY_DOLLAR
	NAME_ELEMENT(KEY_DOLLAR),
#endif
#ifdef KEY_EURO
	NAME_ELEMENT(KEY_EURO),
#endif
#ifdef KEY_FRAMEBACK
	NAME_ELEMENT(KEY_FRAMEBACK),
#endif
#ifdef KEY_FRAMEFORWARD
	NAME_ELEMENT(KEY_FRAMEFORWARD),
#endif
#ifdef KEY_CONTEXT_MENU
	NAME_ELEMENT(KEY_CONTEXT_MENU),
#endif
#ifdef KEY_MEDIA_REPEAT
	NAME_ELEMENT(KEY_MEDIA_REPEAT),
#endif
#ifdef KEY_10CHANNELSUP
	NAME_ELEMENT(KEY_10CHANNELSUP),
#endif
#ifdef KEY_10CHANNELSDOWN
	NAME_ELEMENT(KEY_10CHANNELSDOWN),
#endif
#ifdef KEY_IMAGES
	NAME_ELEMENT(KEY_IMAGES),
#endif
	NAME_ELEMENT(KEY_DEL_EOL),		NAME_ELEMENT(KEY_DEL_EOS),
	NAME_ELEMENT(KEY_INS_LINE),	 	NAME_ELEMENT(KEY_DEL_LINE),
	NAME_ELEMENT(KEY_FN),			NAME_ELEMENT(KEY_FN_ESC),
	NAME_ELEMENT(KEY_FN_F1),		NAME_ELEMENT(KEY_FN_F2),
	NAME_ELEMENT(KEY_FN_F3),		NAME_ELEMENT(KEY_FN_F4),
	NAME_ELEMENT(KEY_FN_F5),		NAME_ELEMENT(KEY_FN_F6),
	NAME_ELEMENT(KEY_FN_F7),		NAME_ELEMENT(KEY_FN_F8),
	NAME_ELEMENT(KEY_FN_F9),		NAME_ELEMENT(KEY_FN_F10),
	NAME_ELEMENT(KEY_FN_F11),		NAME_ELEMENT(KEY_FN_F12),
	NAME_ELEMENT(KEY_FN_1),			NAME_ELEMENT(KEY_FN_2),
	NAME_ELEMENT(KEY_FN_D),			NAME_ELEMENT(KEY_FN_E),
	NAME_ELEMENT(KEY_FN_F),			NAME_ELEMENT(KEY_FN_S),
	NAME_ELEMENT(KEY_FN_B),
	NAME_ELEMENT(KEY_BRL_DOT1),		NAME_ELEMENT(KEY_BRL_DOT2),
	NAME_ELEMENT(KEY_BRL_DOT3),		NAME_ELEMENT(KEY_BRL_DOT4),
	NAME_ELEMENT(KEY_BRL_DOT5),		NAME_ELEMENT(KEY_BRL_DOT6),
	NAME_ELEMENT(KEY_BRL_DOT7),		NAME_ELEMENT(KEY_BRL_DOT8),
	NAME_ELEMENT(KEY_BRL_DOT9),		NAME_ELEMENT(KEY_BRL_DOT10),
#ifdef KEY_NUMERIC_0
	NAME_ELEMENT(KEY_NUMERIC_0),		NAME_ELEMENT(KEY_NUMERIC_1),
	NAME_ELEMENT(KEY_NUMERIC_2),		NAME_ELEMENT(KEY_NUMERIC_3),
	NAME_ELEMENT(KEY_NUMERIC_4),		NAME_ELEMENT(KEY_NUMERIC_5),
	NAME_ELEMENT(KEY_NUMERIC_6),		NAME_ELEMENT(KEY_NUMERIC_7),
	NAME_ELEMENT(KEY_NUMERIC_8),		NAME_ELEMENT(KEY_NUMERIC_9),
	NAME_ELEMENT(KEY_NUMERIC_STAR),		NAME_ELEMENT(KEY_NUMERIC_POUND),
#endif
#ifdef KEY_NUMERIC_11
	NAME_ELEMENT(KEY_NUMERIC_11),		NAME_ELEMENT(KEY_NUMERIC_12),
#endif
	NAME_ELEMENT(KEY_BATTERY),
	NAME_ELEMENT(KEY_BLUETOOTH),		NAME_ELEMENT(KEY_BRIGHTNESS_CYCLE),
	NAME_ELEMENT(KEY_BRIGHTNESS_ZERO),
#ifdef KEY_DASHBOARD
	NAME_ELEMENT(KEY_DASHBOARD),
#endif
	NAME_ELEMENT(KEY_DISPLAY_OFF),		NAME_ELEMENT(KEY_DOCUMENTS),
	NAME_ELEMENT(KEY_FORWARDMAIL),		NAME_ELEMENT(KEY_NEW),
	NAME_ELEMENT(KEY_KBDILLUMDOWN),		NAME_ELEMENT(KEY_KBDILLUMUP),
	NAME_ELEMENT(KEY_KBDILLUMTOGGLE), 	NAME_ELEMENT(KEY_REDO),
	NAME_ELEMENT(KEY_REPLY),		NAME_ELEMENT(KEY_SAVE),
#ifdef KEY_SCALE
	NAME_ELEMENT(KEY_SCALE),
#endif
	NAME_ELEMENT(KEY_SEND),
	NAME_ELEMENT(KEY_SCREENLOCK),		NAME_ELEMENT(KEY_SWITCHVIDEOMODE),
#ifdef KEY_UWB
	NAME_ELEMENT(KEY_UWB),
#endif
#ifdef KEY_VIDEO_NEXT
	NAME_ELEMENT(KEY_VIDEO_NEXT),
#endif
#ifdef KEY_VIDEO_PREV
	NAME_ELEMENT(KEY_VIDEO_PREV),
#endif
#ifdef KEY_WIMAX
	NAME_ELEMENT(KEY_WIMAX),
#endif
#ifdef KEY_WLAN
	NAME_ELEMENT(KEY_WLAN),
#endif
#ifdef KEY_RFKILL
	NAME_ELEMENT(KEY_RFKILL),
#endif
#ifdef KEY_MICMUTE
	NAME_ELEMENT(KEY_MICMUTE),
#endif
#ifdef KEY_CAMERA_FOCUS
	NAME_ELEMENT(KEY_CAMERA_FOCUS),
#endif
#ifdef KEY_WPS_BUTTON
	NAME_ELEMENT(KEY_WPS_BUTTON),
#endif
#ifdef KEY_TOUCHPAD_TOGGLE
	NAME_ELEMENT(KEY_TOUCHPAD_TOGGLE),
	NAME_ELEMENT(KEY_TOUCHPAD_ON),
	NAME_ELEMENT(KEY_TOUCHPAD_OFF),
#endif
#ifdef KEY_CAMERA_ZOOMIN
	NAME_ELEMENT(KEY_CAMERA_ZOOMIN),	NAME_ELEMENT(KEY_CAMERA_ZOOMOUT),
	NAME_ELEMENT(KEY_CAMERA_UP),		NAME_ELEMENT(KEY_CAMERA_DOWN),
	NAME_ELEMENT(KEY_CAMERA_LEFT),		NAME_ELEMENT(KEY_CAMERA_RIGHT),
#endif
#ifdef KEY_ATTENDANT_ON
	NAME_ELEMENT(KEY_ATTENDANT_ON),		NAME_ELEMENT(KEY_ATTENDANT_OFF),
	NAME_ELEMENT(KEY_ATTENDANT_TOGGLE),	NAME_ELEMENT(KEY_LIGHTS_TOGGLE),
#endif

	NAME_ELEMENT(BTN_0),			NAME_ELEMENT(BTN_1),
	NAME_ELEMENT(BTN_2),			NAME_ELEMENT(BTN_3),
	NAME_ELEMENT(BTN_4),			NAME_ELEMENT(BTN_5),
	NAME_ELEMENT(BTN_6),			NAME_ELEMENT(BTN_7),
	NAME_ELEMENT(BTN_8),			NAME_ELEMENT(BTN_9),
	NAME_ELEMENT(BTN_LEFT),			NAME_ELEMENT(BTN_RIGHT),
	NAME_ELEMENT(BTN_MIDDLE),		NAME_ELEMENT(BTN_SIDE),
	NAME_ELEMENT(BTN_EXTRA),		NAME_ELEMENT(BTN_FORWARD),
	NAME_ELEMENT(BTN_BACK),			NAME_ELEMENT(BTN_TASK),
	NAME_ELEMENT(BTN_TRIGGER),		NAME_ELEMENT(BTN_THUMB),
	NAME_ELEMENT(BTN_THUMB2),		NAME_ELEMENT(BTN_TOP),
	NAME_ELEMENT(BTN_TOP2),			NAME_ELEMENT(BTN_PINKIE),
	NAME_ELEMENT(BTN_BASE),			NAME_ELEMENT(BTN_BASE2),
	NAME_ELEMENT(BTN_BASE3),		NAME_ELEMENT(BTN_BASE4),
	NAME_ELEMENT(BTN_BASE5),		NAME_ELEMENT(BTN_BASE6),
	NAME_ELEMENT(BTN_DEAD),			NAME_ELEMENT(BTN_C),
#ifdef BTN_SOUTH
	NAME_ELEMENT(BTN_SOUTH),		NAME_ELEMENT(BTN_EAST),
	NAME_ELEMENT(BTN_NORTH),		NAME_ELEMENT(BTN_WEST),
#else
	NAME_ELEMENT(BTN_A),			NAME_ELEMENT(BTN_B),
	NAME_ELEMENT(BTN_X),			NAME_ELEMENT(BTN_Y),
#endif
	NAME_ELEMENT(BTN_Z),			NAME_ELEMENT(BTN_TL),
	NAME_ELEMENT(BTN_TR),			NAME_ELEMENT(BTN_TL2),
	NAME_ELEMENT(BTN_TR2),			NAME_ELEMENT(BTN_SELECT),
	NAME_ELEMENT(BTN_START),		NAME_ELEMENT(BTN_MODE),
	NAME_ELEMENT(BTN_THUMBL),		NAME_ELEMENT(BTN_THUMBR),
	NAME_ELEMENT(BTN_TOOL_PEN),		NAME_ELEMENT(BTN_TOOL_RUBBER),
	NAME_ELEMENT(BTN_TOOL_BRUSH),		NAME_ELEMENT(BTN_TOOL_PENCIL),
	NAME_ELEMENT(BTN_TOOL_AIRBRUSH),	NAME_ELEMENT(BTN_TOOL_FINGER),
	NAME_ELEMENT(BTN_TOOL_MOUSE),		NAME_ELEMENT(BTN_TOOL_LENS),
#ifdef BTN_STYLUS3
	NAME_ELEMENT(BTN_STYLUS3),
#endif
	NAME_ELEMENT(BTN_TOUCH),		NAME_ELEMENT(BTN_STYLUS),
	NAME_ELEMENT(BTN_STYLUS2),		NAME_ELEMENT(BTN_TOOL_DOUBLETAP),
	NAME_ELEMENT(BTN_TOOL_TRIPLETAP),
#ifdef BTN_TOOL_QUADTAP
	NAME_ELEMENT(BTN_TOOL_QUADTAP),
#endif
	NAME_ELEMENT(BTN_GEAR_DOWN),
	NAME_ELEMENT(BTN_GEAR_UP),

#ifdef BTN_DPAD_UP
	NAME_ELEMENT(BTN_DPAD_UP),		NAME_ELEMENT(BTN_DPAD_DOWN),
	NAME_ELEMENT(BTN_DPAD_LEFT),		NAME_ELEMENT(BTN_DPAD_RIGHT),
#endif
#ifdef KEY_ALS_TOGGLE
	NAME_ELEMENT(KEY_ALS_TOGGLE),
#endif
#ifdef KEY_BUTTONCONFIG
	NAME_ELEMENT(KEY_BUTTONCONFIG),
#endif
#ifdef KEY_TASKMANAGER
	NAME_ELEMENT(KEY_TASKMANAGER),
#endif
#ifdef KEY_JOURNAL
	NAME_ELEMENT(KEY_JOURNAL),
#endif
#ifdef KEY_CONTROLPANEL
	NAME_ELEMENT(KEY_CONTROLPANEL),
#endif
#ifdef KEY_APPSELECT
	NAME_ELEMENT(KEY_APPSELECT),
#endif
#ifdef KEY_SCREENSAVER
	NAME_ELEMENT(KEY_SCREENSAVER),
#endif
#ifdef KEY_VOICECOMMAND
	NAME_ELEMENT(KEY_VOICECOMMAND),
#endif
#ifdef KEY_ASSISTANT
	NAME_ELEMENT(KEY_ASSISTANT),
#endif
#ifdef KEY_BRIGHTNESS_MIN
	NAME_ELEMENT(KEY_BRIGHTNESS_MIN),
#endif
#ifdef KEY_BRIGHTNESS_MAX
	NAME_ELEMENT(KEY_BRIGHTNESS_MAX),
#endif
#ifdef KEY_KBDINPUTASSIST_PREV
	NAME_ELEMENT(KEY_KBDINPUTASSIST_PREV),
#endif
#ifdef KEY_KBDINPUTASSIST_NEXT
	NAME_ELEMENT(KEY_KBDINPUTASSIST_NEXT),
#endif
#ifdef KEY_KBDINPUTASSIST_PREVGROUP
	NAME_ELEMENT(KEY_KBDINPUTASSIST_PREVGROUP),
#endif
#ifdef KEY_KBDINPUTASSIST_NEXTGROUP
	NAME_ELEMENT(KEY_KBDINPUTASSIST_NEXTGROUP),
#endif
#ifdef KEY_KBDINPUTASSIST_ACCEPT
	NAME_ELEMENT(KEY_KBDINPUTASSIST_ACCEPT),
#endif
#ifdef KEY_KBDINPUTASSIST_CANCEL
	NAME_ELEMENT(KEY_KBDINPUTASSIST_CANCEL),
#endif

#ifdef KEY_RIGHT_UP
	NAME_ELEMENT(KEY_RIGHT_UP),		NAME_ELEMENT(KEY_RIGHT_DOWN),
	NAME_ELEMENT(KEY_LEFT_UP),		NAME_ELEMENT(KEY_LEFT_DOWN),
#endif

#ifdef KEY_ROOT_MENU
	NAME_ELEMENT(KEY_ROOT_MENU),
	NAME_ELEMENT(KEY_MEDIA_TOP_MENU),
	NAME_ELEMENT(KEY_AUDIO_DESC),
	NAME_ELEMENT(KEY_3D_MODE),
	NAME_ELEMENT(KEY_NEXT_FAVORITE),
	NAME_ELEMENT(KEY_STOP_RECORD),
	NAME_ELEMENT(KEY_PAUSE_RECORD),
	NAME_ELEMENT(KEY_VOD),
	NAME_ELEMENT(KEY_UNMUTE),
	NAME_ELEMENT(KEY_FASTREVERSE),
	NAME_ELEMENT(KEY_SLOWREVERSE),
#endif

#ifdef KEY_DATA
	NAME_ELEMENT(KEY_DATA),
#endif

#ifdef KEY_ONSCREEN_KEYBOARD
	NAME_ELEMENT(KEY_ONSCREEN_KEYBOARD),
#endif

#ifdef BTN_TRIGGER_HAPPY
	NAME_ELEMENT(BTN_TRIGGER_HAPPY1),	NAME_ELEMENT(BTN_TRIGGER_HAPPY11),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY2),	NAME_ELEMENT(BTN_TRIGGER_HAPPY12),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY3),	NAME_ELEMENT(BTN_TRIGGER_HAPPY13),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY4),	NAME_ELEMENT(BTN_TRIGGER_HAPPY14),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY5),	NAME_ELEMENT(BTN_TRIGGER_HAPPY15),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY6),	NAME_ELEMENT(BTN_TRIGGER_HAPPY16),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY7),	NAME_ELEMENT(BTN_TRIGGER_HAPPY17),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY8),	NAME_ELEMENT(BTN_TRIGGER_HAPPY18),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY9),	NAME_ELEMENT(BTN_TRIGGER_HAPPY19),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY10),	NAME_ELEMENT(BTN_TRIGGER_HAPPY20),

	NAME_ELEMENT(BTN_TRIGGER_HAPPY21),	NAME_ELEMENT(BTN_TRIGGER_HAPPY31),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY22),	NAME_ELEMENT(BTN_TRIGGER_HAPPY32),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY23),	NAME_ELEMENT(BTN_TRIGGER_HAPPY33),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY24),	NAME_ELEMENT(BTN_TRIGGER_HAPPY34),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY25),	NAME_ELEMENT(BTN_TRIGGER_HAPPY35),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY26),	NAME_ELEMENT(BTN_TRIGGER_HAPPY36),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY27),	NAME_ELEMENT(BTN_TRIGGER_HAPPY37),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY28),	NAME_ELEMENT(BTN_TRIGGER_HAPPY38),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY29),	NAME_ELEMENT(BTN_TRIGGER_HAPPY39),
	NAME_ELEMENT(BTN_TRIGGER_HAPPY30),	NAME_ELEMENT(BTN_TRIGGER_HAPPY40),
#endif
#ifdef BTN_TOOL_QUINTTAP
	NAME_ELEMENT(BTN_TOOL_QUINTTAP),
#endif
};

int num_usr_def_evt = 0;

static const char *const evval[3] = {
        "RELEASED",
        "PRESSED ",
        "REPEATED"
};

static int make_socket_non_blocking(int sfd)
{
        int flags, s;

        flags = fcntl(sfd, F_GETFL, 0);
        if (flags == -1) {
                perror("fcntl");
                return -1;
        }

        flags |= O_NONBLOCK;
        s = fcntl(sfd, F_SETFL, flags);
        if (s == -1) {
                perror("fcntl");
                return -1;
        }

        return 0;
}

void read_config()
{
    FILE *file;
    char line[MAX_LINE_LENGTH];

    file = fopen("/etc/config/xs_trigger.conf", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        // Skip lines starting with '#'
        if (line[0] == '#')
            continue;

        // Parse the line into keycode, pressed, and command
        struct TriggerEntry entry;
        if (sscanf(line, "%s %d %s", entry.keycode, &entry.pressed, entry.command) != 3) {
            printf("Invalid line: %s\n", line);
            continue;
        }

        // Store the entry in the array
        entries[num_usr_def_evt++] = entry;
        if (num_usr_def_evt >= MAX_ENTRIES) {
            printf("Exceeded maximum number of entries\n");
            break;
        }
    }

    fclose(file);

    // Print the stored entries
    printf("Stored entries:\n");
	int i = 0;
    for (i = 0; i < num_usr_def_evt; i++) 
	{
        printf("Entry %d:\n", i + 1);
        printf("Keycode: %s\n", entries[i].keycode);
        printf("Pressed or released: %d\n", entries[i].pressed);
        printf("Command: %s\n", entries[i].command);
    }
	printf("=======================================================\n");
}


int main(int argc, char *argv[])
{
        int sfd, s;
        int efd;
        struct epoll_event event;
        struct epoll_event *events;
        char *dev_path;
        int done = 0;

		printf("xs_trigger Version: 0.0.1\n");
        if (argc != 2) {
                fprintf(stderr, "Usage: %s [devPath]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
		
		read_config();
		
        dev_path = argv[1];
        sfd = open(dev_path, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
        if (sfd == -1) {
                fprintf(stderr, "open_port: Unable to open %s\n", dev_path);
                return (-1);
        }
        make_socket_non_blocking(sfd);

        char name[256] = "Unknown";
        ioctl(sfd, EVIOCGNAME(sizeof(name)), name);
        printf("Reading from device : %s (%s)\n", dev_path, name);

        efd = epoll_create1(0);
        if (efd == -1) {
                perror("epoll_create");
                abort();
        }

        event.data.fd = sfd;
        event.events = EPOLLIN | EPOLLET;
        s = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
        if (s == -1) {
                perror("epoll_ctl");
                abort();
        }

        /* Buffer where events are returned */
        events = calloc(MAXEVENTS, sizeof event);

        while (1) {
                int n, i;

                n = epoll_wait(efd, events, MAXEVENTS, -1);
                for (i = 0; i < n; i++) 
				{
                        if ((events[i].events & EPOLLERR) ||
                            (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
                                       
                                fprintf(stderr, "epoll error\n");
                                close(events[i].data.fd);
                                continue;
                        }
                        else if (sfd == events[i].data.fd) 
						{
							while (1) 
							{
								int len = 0;
								struct input_event key_press[64];
								//if ((len = read(sfd, &key_press, sizeof key_press)) != -1) {
								if ((len = read(sfd, key_press, sizeof key_press)) != -1) 
								{
									//printf("len: %d\n", len);
									if (key_press[0].type == 1) //EV_KEY
									{
										printf("Code: %d; Value: %d; Val(str): %s; Codename: %s\n", 
												key_press[0].code, 
												key_press[0].value,
												evval[key_press[0].value],
												keys[key_press[0].code]
												);
										int j = 0;
										for (j = 0; j < num_usr_def_evt; j++) 
										{
											if ( strncmp(entries[j].keycode, keys[key_press[0].code], 256) == 0 &&
											     entries[j].pressed == key_press[0].value)
											 {
												printf("executing: %s\n", entries[j].command);
												system(entries[j].command);
											 }
											//printf("Entry %d:\n", j + 1);
											//printf("Keycode: %s\n", entries[j].keycode);
											//printf("Pressed or released: %d\n", entries[j].pressed);
											//printf("Command: %s\n", entries[j].command);
										}
									}
								}
								break;
							}
							continue;
                        } else {
                                int done = 0;

                                if (done) {
                                        printf("Closed on descriptor %d\n",
                                               events[i].data.fd);

                                        close(events[i].data.fd);
                                }
                        }
                }
        }

        free(events);

        close(sfd);
        return EXIT_SUCCESS;
}
