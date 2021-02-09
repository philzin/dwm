/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx        = 1;        /* border pixel of windows */
static unsigned int snap            = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static unsigned int gappih    = 0;       /* horiz inner gap between windows */
static unsigned int gappiv    = 0;       /* vert inner gap between windows */
static unsigned int gappoh    = 0;       /* horiz outer gap between windows and screen edge */
static unsigned int gappov    = 0;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static int showbar                  = 1;        /* 0 means no bar */
static int topbar                   = 1;        /* 0 means bottom bar */
static const int user_bh            = 14;        /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const char *fonts[]          = { "Siji", "monospace:pixelsize=12", "Noto Color Emoji:pixelsize=10" };
static const char dmenufont[]       = "monospace:pixelsize=12";
static char normbgcolor[]           = "#1d1f21";
static char normbordercolor[]       = "#666666";
static char normfgcolor[]           = "#c5c8c6";
static char selfgcolor[]            = "#eaeaea";
static char selbgcolor[]	    = "#198844";
static char selbordercolor[]	    = "#198844";
static char *colors[][3]            = {
	/*               fg         bg         border   */
	[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
	[SchemeSel]  = { selfgcolor, selbgcolor,  selbordercolor  },
	[SchemeStatus]  = { normfgcolor, normbgcolor,  "#000000"  }, // Statusbar right {text,background,not used but cannot be empty}
	[SchemeTagsSel]  = { selfgcolor, selbgcolor,  "#000000"  }, // Tagbar left selected {text,background,not used but cannot be empty}
	[SchemeTagsNorm]  = { normfgcolor, normbgcolor,  "#000000"  }, // Tagbar left unselected {text,background,not used but cannot be empty}
	[SchemeInfoSel]  = { selbgcolor, normbgcolor,  "#000000"  }, // infobar middle  selected {text,background,not used but cannot be empty}
	[SchemeInfoNorm]  = { normfgcolor, normbgcolor,  "#000000"  }, // infobar middle  unselected {text,background,not used but cannot be empty}
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "St",      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ "URxvt",   NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
	{ NULL,      NULL,     "Discord",      1<<7,      0,          0,           0,        -1 },
	{ NULL,      NULL,     "雀魂Plus",     1<<6,      0,          0,           0,        -1 },
	{ NULL,      NULL,     "Majsoul Plus", 1<<6,      0,          0,           0,        -1 },
	{ NULL,      NULL,     "Transmission Remote", 1<<2, 0,        0,           0,        -1 },
	{ "mpv",     NULL,     NULL,           1<<1,      0,          0,           0,        -1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "",      tile },    /* first entry is default */
	{ "",      NULL },    /* no layout function means floating behavior */
	{ "",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \
/*	{ MOD, XK_q,     ACTION##stack, {.i = 0 } }, \
*	{ MOD, XK_a,     ACTION##stack, {.i = 1 } }, \
*	{ MOD, XK_z,     ACTION##stack, {.i = 2 } }, \
*	{ MOD, XK_x,     ACTION##stack, {.i = -1 } }, */

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, "-i", NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *urxvtcmd[] = { "urxvt", NULL };
static const char *browsercmd[]  = { "chromium", NULL };
static const char *disccmd[]  = { "discord", NULL };
static const char *upvol[]   = { "pamixer", "-i", "5", NULL };
static const char *downvol[]   = { "pamixer", "-d", "5", NULL };
static const char *mutevol[]   = { "pamixer", "-t", NULL };
static const char *brightup[] = { "brightnessctl", "s", "10%+", NULL };
static const char *brightdown[] = { "brightnessctl", "s", "10%-", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "color0",             STRING,  &normbgcolor },
		{ "color8",             STRING,  &normbordercolor },
		{ "color15",            STRING,  &normfgcolor },
		{ "color2",             STRING,  &selbgcolor },
		{ "color15",             STRING,  &selbordercolor },
		{ "color15",            STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          	INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",       	FLOAT,   &mfact },
		{ "gappih",          	INTEGER, &gappih },
		{ "gappoh",          	INTEGER, &gappoh },
		{ "gappiv",          	INTEGER, &gappiv },
		{ "gappov",       	INTEGER, &gappov },
		{ "systrayspacing",	INTEGER, &systrayspacing },
};


#include "shiftview.c"
#include <X11/XF86keysym.h>
static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_r,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = urxvtcmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_c,	   spawn,      	   {.v = browsercmd } },
	{ MODKEY,                       XK_d,	   spawn,      	   {.v = disccmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY,                       XK_F5,     xrdb,           {.v = NULL } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_u,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ Mod1Mask,                     XK_Tab,    view,           {0} },
	{ Mod1Mask,                     XK_F4,     killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_q,      setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_g,      togglegaps,     {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,              	        XK_Right,  shiftview,  	   { .i = +1 } },
	{ MODKEY,              	        XK_Left,   shiftview,      { .i = -1 } },
	{ 0,           XF86XK_AudioRaiseVolume,    spawn,          {.v = upvol   } },
	{ 0,           XF86XK_AudioLowerVolume,    spawn,          {.v = downvol } },
	{ 0,                  XF86XK_AudioMute,    spawn,          {.v = mutevol } },
	{ 0,		XF86XK_MonBrightnessUp,	   spawn,	   {.v = brightup } },
	{ 0,		XF86XK_MonBrightnessDown,  spawn,	   {.v = brightdown } },
	{ 0,				XK_Print,  spawn,	   SHCMD("maim -u | tee ~/Pictures/screenshots/$(date +%s).png ~/.cache/lastscr.png | xclip -selection clipboard -target image/png") },
	{ ControlMask,			XK_Print,  spawn,	   SHCMD("maim -su | tee ~/Pictures/screenshots/$(date +%s).png ~/.cache/lastscr.png | xclip -selection clipboard -target image/png") },
	{ Mod1Mask,			XK_Print,  spawn,	   SHCMD("maim -ui $(xdotool getactivewindow) | tee ~/Pictures/screenshots/$(date +%s).png ~/.cache/lastscr.png | xclip -selection clipboard -target image/png") },
	{ ControlMask|MODKEY|Mod1Mask,  XK_u,      spawn,	   SHCMD("curl -fsL -F \"files[]=@$(realpath ~/.cache/lastscr.png)\" https://uguu.se/upload.php | jq -r '.files[].url' | xclip -selection clipboard && dunstify -I ~/.cache/lastscr.png 'Screenshot uploaded' $(xclip -o -selection clipboard)") },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,	   quit,           {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
