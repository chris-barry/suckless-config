/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const char font[]            = "-*-terminus-medium-r-*-*-16-*-*-*-*-*-*-*";
static const char normbordercolor[] = "#444444";
static const char normbgcolor[]     = "#222222";
static const char normfgcolor[]     = "#bbbbbb";
static const char selbordercolor[]  = "#005577";
static const char selbgcolor[]      = "#005577";
static const char selfgcolor[]      = "#eeeeee";
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const Bool showbar           = True;     /* False means no bar */
static const Bool topbar            = True;     /* False means bottom bar */

/* tagging */
static const char *tags[] = { "shell", "web", "chat", "misc" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            True,        -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       False,       -1 },
};

/* layout(s) */
static const float mfact      = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster      = 1;    /* number of clients in master area */
static const Bool resizehints = False; /* True means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", font, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };

static const char *lowervol[] = { "amixer", "-q", "set", "Master", "5-", "unmute", NULL };
static const char *raisevol[] = { "amixer", "-q", "set", "Master", "5+", "unmute", NULL };
static const char *mute[]     = { "amixer", "-q", "set", "Master", "toggle", NULL };

static const char *bright[]   = { "sudo", "/usr/bin/backlight", "-inc", "1", NULL };
static const char *dim[]      = { "sudo", "/usr/bin/backlight", "-dec", "1", NULL };

static const char *tlock[]    = { "xautolock", "-toggle", NULL };
static const char *lock[]     = { "slock", NULL };

static const char *mpdtoggle[]= { "mpc", "toggle", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,           XK_b,      togglebar,      {0} },
	{ MODKEY,           XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,           XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,           XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,           XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,           XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,           XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,           XK_Return, zoom,           {0} },
	{ MODKEY,           XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask, XK_c,      killclient,     {0} },
	{ MODKEY,           XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,           XK_f,      setlayout,      {.v = &layouts[1]} },
	/*
	{ MODKEY,           XK_m,      setlayout,      {.v = &layouts[2]} },
	*/
	{ MODKEY,           XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask, XK_space,  togglefloating, {0} },
	{ MODKEY,           XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask, XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,           XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,           XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask, XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask, XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(            XK_1,                      0)
	TAGKEYS(            XK_2,                      1)
	TAGKEYS(            XK_3,                      2)
	TAGKEYS(            XK_4,                      3)
	{ MODKEY|ShiftMask, XK_q,      quit,           {0} },

	/* Added by Chris Barry */
	{ MODKEY|ControlMask, XK_Delete,                spawn, {.v = lock } },     /* ctrl-alt-l = lock screen */

	{ 0,                  XF86XK_Suspend,           spawn, {.v = lock } },     /* Testing */
	{ 0,                  XF86XK_Suspend,           spawn, {.v = lock } },     /* Testing */

	{ MODKEY|ControlMask, XK_t,                     spawn, {.v = termcmd} },   /* ctrl-alt-t = start terminal */
	{ ControlMask,        XK_space,                 spawn, {.v = dmenucmd } }, /* ctrl-space = start dmenu */

	{ 0,                  XF86XK_AudioLowerVolume,  spawn, {.v = lowervol} },  /* alt-F6 = lower volume */
	{ 0,                  XF86XK_AudioRaiseVolume,  spawn, {.v = raisevol} },  /* alt-F7 = raise volume */ 
	{ 0,                  XF86XK_AudioMute,         spawn, {.v = mute} },      /* alt-F7 = raise volume */ 

    { 0,                  XF86XK_MonBrightnessUp,   spawn, {.v = bright} },
    { 0,                  XF86XK_MonBrightnessDown, spawn, {.v = dim } },
    { MODKEY,             XK_F4,                    spawn, {.v = bright} },    /* For when not on laptop keyboard. */
    { MODKEY,             XK_F3,                    spawn, {.v = dim } },      /* For when not on laptop keyboard. */

    { MODKEY,             XK_F5,                    spawn, {.v = mpdtoggle } },
    { 0,                  XF86XK_AudioPlay,         spawn, {.v = mpdtoggle } },

};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },

	{ ClkStatusText,        0,              Button2,        spawn,          {.v = tlock } },
};

