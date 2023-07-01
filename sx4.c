/*
 * ----------------------------------------------------------------------------
 *
 * Copyright (C) 2023 NRK.
 *
 * This file is part of sx4.
 *
 * sx4 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * sx4 is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sx4. If not, see <https://www.gnu.org/licenses/>.
 *
 * ----------------------------------------------------------------------------
 *
 * frog is mands best fren.
 * frogs are good frens, keep the bugs away.
 *
 * ⠀⠀⠀⠀⠀⠀⠀⠀⣠⣔⡲⠂⠈⠉⠉⠉⠉⠁⠖⣒⣤⡀⠀⠀⠀⠀⠀⠀⠀⠀
 * ⠀⠀⠀⠀⠀⠀⠀⣸⣁⣻⣷⠀⠀⠀⠀⠀⠀⠀⢰⣇⣹⣿⡆⠀⠀⠀⠀⠀⠀⠀
 * ⠀⠀⠀⠀⠀⠀⠀⡟⠿⠿⠏⠀⠀⠀⡀⠀⠀⠀⠈⠻⠿⠟⡅⠀⠀⠀⠀⠀⠀⠀
 * ⠀⠀⠀⠀⠀⠀⢨⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⡄⠀⠀⠀⠀⠀⠀
 * ⠀⠀⠀⠀⠀⢀⠂⡼⠉⠛⠡⠶⠶⠶⠶⠶⠚⠙⡲⠖⠛⠻⠃⠈⢂⠀⠀⠀⠀⠀
 * ⠀⠀⢀⡀⢰⣁⡘⠀⠇⠀⠀⠀⠀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠈⢄⠀⡆⠀⠀⠀⠀
 * ⢠⠊⠁⢀⠔⠉⠀⡌⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⠀⠀⠑⢏⠁⠐⠄⠀
 * ⢃⠀⢰⠁⠀⠀⠢⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠄⠊⠀⠆⠀⠀⠀⢱
 * ⠈⠢⡀⢂⠀⠀⠀⢑⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠃⠀⠀⡜⠀⠀⠀⠀⣸
 * ⠀⠀⡀⠭⢧⠀⠀⠸⡈⠐⠤⠀⣀⣀⠀⣀⡀⠠⠐⡘⠁⠀⠀⡔⣈⠡⠒⣖⠈⠀
 * ⠀⠀⠻⠔⣇⣆⠀⠀⠙⠒⠢⠤⢀⣀⡠⡀⢤⣤⡚⠂⠀⠀⡜⠋⢍⣀⠗⠒⠁⠀
 * ⠀⠀⠀⠀⠀⠈⢑⠠⣆⡀⢹⠃⠂⠈⠢⢻⣑⡢⠀⠠⠤⠔⠃⠀⠀⠀⠀⠀⠀⠀
 */

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>

// macros

#define NOP()           ((void)0)
#define DIFF(A, B)      ((A) > (B) ? (A) - (B) : (B) - (A))
#define SIZEOF(...)     ((Size)sizeof(__VA_ARGS__))
#define ARRLEN(X)       (SIZEOF(X) / SIZEOF(0[X]))
#define S(X)            ((Str){ .s = (u8 *)(X), .len = SIZEOF(X) - 1})
#define fatal(...)      fatal_core( \
	(Str []){ __VA_ARGS__ }, ARRLEN(((Str []){ __VA_ARGS__ })) \
)
#ifdef __GNUC__
	/* when debugging, use gcc/clang and compile with
	 * `-fsanitize=undefined -fsanitize-undefined-trap-on-error`
	 * it'll trap if an unreachable code-path is ever reached.
	 */
	#define ASSERT(X)  ((X) ? (void)0 : __builtin_unreachable())
#else
	#define ASSERT(X)  ((void)0)
#endif
#ifdef DEBUG
	#define LOG(...) (fprintf(stderr, __VA_ARGS__), fputc('\n', stderr))
#else
	#define LOG(...) ((void)0)
#endif
#ifndef VERSION
	#define VERSION "BETA"
#endif

#define CUR_GRAB_MASK  (ButtonPressMask | ButtonReleaseMask | PointerMotionMask)

// types

typedef uint8_t    u8;
typedef uint32_t   u32;
typedef uint64_t   u64;
typedef int64_t    i64;
typedef ptrdiff_t  Size;
typedef struct { u8 *s; Size len; } Str;

typedef struct { int x, y; } Point;
typedef struct { int x, y, w, h; } Rect;

typedef struct {
	Display *dpy;
	struct {
		Window win;
		int x, y, w, h;
	} root;

	Cursor cross;
	Cursor dirs[4];

	// shape
	Window win;
} X11;

enum { EV_CLICK, EV_MOTION, EV_SWAP, EV_ABORT };
typedef struct {
	Point start, last;
	Rect final;
	enum {
		STATE_WAIT,
		STATE_INIT,
		STATE_DRAW,
		// terminating states. don't mess with the order of this
		STATE_EXIT_LOOP,
		STATE_RECT,
		STATE_WINDOW,
		STATE_ABORT,
	} state;
	Window target;

	const char *color_name;
	int border_width;
} DrawCtx;

// functions

#pragma GCC diagnostic ignored "-Wdeprecated-declarations" /* stfu, thanks */

static void
fatal_core(Str *sv, Size n)
{
	for (Size i = 0; i < n; ++i) {
		ASSERT(sv[i].len > 0);
		fwrite(sv[i].s, 1, sv[i].len, stderr);
	}
	exit(1);
}

static Str
str_from_cstr(char *s)
{
	Str ret = { .s = (u8 *)s };
	for (NOP(); ret.s != NULL && ret.s[ret.len] != '\0'; ++ret.len) {}
	return ret;
}

static bool
str_eq(Str a, Str b)
{
	Size n = -1;
	ASSERT(a.len >= 0 && b.len >= 0);
	if (a.len == b.len) {
		for (n = 0; n < a.len && a.s[n] == b.s[n]; ++n) {}
	}
	return n == a.len;
}

static bool
str_to_i64(Str s, i64 *out)
{
	Size i;
	i64 c, n = 0;
	for (i = 0; i < s.len && (c = s.s[i] - '0') >= 0 && c <= 9; ++i) {
		if (((INT64_MAX - c) / 10) < n) {
			return false;
		}
		n = (n * 10) + c;
	}
	*out = n;
	return i == s.len;
}

static bool
str_to_hex64(Str s, i64 *out)
{
	i64 c, n = 0;
	if (s.len <= 2 || !str_eq(S("0x"), (Str){ .s = s.s, .len = 2 })) {
		return false;
	}
	u8 v[] = { 0xF0, '0', 0x00, 'A' - 10, 'a' - 10 };
	for (Size i = 2; i < s.len; ++i) {
		c = s.s[i];
		int d = (c >= '0') + (c > '9') + (c >= 'A') + (c >= 'a');
		ASSERT(d < ARRLEN(v));
		c -= v[d];
		if ((c & 0xF0) || ((INT64_MAX - c) / 16) < n) {
			return false;
		}
		n = (n * 16) + c;
	}
	*out = n;
	return true;
}

static u8 *
int_to_str(u8 *p, int n)
{
	ASSERT(n >= 0);
	u8 *q = p + 32, *end = q;
	do {
		*--q = (n % 10) + '0';
	} while (n /= 10);
	while (q < end) {
		*p++ = *q++;
	}
	return p;
}

static void
draw(DrawCtx *ctx, X11 *x11, Point cur, int event)
{
	enum {
		F_INIT = 0x1, F_DRAW = 0x2, F_CLEAN = 0x4, F_FINISH = 0x8,
		F_ABORT = 0x10, F_SWAP = 0x20
	};
	int features = 0;

	ASSERT(event >= EV_CLICK && event <= EV_ABORT);
	switch (ctx->state) {
	case STATE_WAIT:
		switch (event) {
		case EV_SWAP: NOP(); break; /* NOLINT */
		case EV_MOTION: NOP(); break;
		case EV_CLICK: features = F_INIT; break;
		case EV_ABORT: features = F_ABORT; break;
		}
		break;
	case STATE_INIT:
		switch (event) {
		case EV_SWAP: NOP(); break;
		case EV_MOTION: features = F_DRAW; break;
		case EV_CLICK: // fall through
		case EV_ABORT: features = F_CLEAN | F_ABORT; break;
		}
		break;
	case STATE_DRAW:
		switch (event) {
		case EV_SWAP: features = F_SWAP | F_DRAW; break;
		case EV_MOTION: features = F_DRAW; break;
		case EV_CLICK: features = F_DRAW | F_CLEAN | F_FINISH; break;
		case EV_ABORT: features = F_CLEAN | F_ABORT; break;
		}
		break;
	default: ASSERT(!"unreachable"); break;
	}

	if (features & F_INIT) {
		unsigned long mask = 0;
		XSetWindowAttributes wa;

		LOG("[sx4]: F_INIT");
		if (XShapeQueryExtension(x11->dpy, (int []){0}, (int []){0}) != True) {
			fatal(S("XShapeQueryExtension failed\n"));
		}

		XColor clr;
		int res = XAllocNamedColor(
			x11->dpy,
			DefaultColormap(x11->dpy, DefaultScreen(x11->dpy)),
			ctx->color_name, (XColor []){0}, &clr
		);
		if (!res) {
			fatal(S("failed to allocate color\n"));
		}

		wa.override_redirect = True;
		mask |= CWOverrideRedirect;
		wa.background_pixel = clr.pixel;
		mask |= CWBackPixel;

		wa.event_mask = StructureNotifyMask;
		/* wa.event_mask |= ExposureMask; // NOTE: causes way too many expose events... */
		mask |= CWEventMask;

		x11->win = XCreateWindow(
			x11->dpy, x11->root.win,
			x11->root.x, x11->root.y, x11->root.w, x11->root.h, 0,
			CopyFromParent, InputOutput, CopyFromParent,
			mask, &wa
		);
		XChangeActivePointerGrab(
			x11->dpy, CUR_GRAB_MASK, x11->cross, CurrentTime
		);

		ctx->last  = cur;
		ctx->start = cur;
		ctx->state = STATE_INIT;
	}

	if (features & F_SWAP) {
		Point tmp = cur;
		cur = ctx->start;
		ctx->start = tmp;
		XWarpPointer(
			x11->dpy, None, x11->root.win, 0, 0, 0, 0,
			cur.x, cur.y
		);
	}

	if (features & F_DRAW) {
		int *px = ((int [3]){ cur.x, ctx->start.x, cur.x }) + (ctx->start.x < cur.x);
		int *py = ((int [3]){ cur.y, ctx->start.y, cur.y }) + (ctx->start.y < cur.y);

		// HACK: -/+1 to fix up the selection around the edges
		int x = px[0] - (px[0]-1 == x11->root.x);
		int y = py[0] - (py[0]-1 == x11->root.y);
		px[1] = px[1] + (px[1]+1 == x11->root.w);
		py[1] = py[1] + (py[1]+1 == x11->root.h);
		int w = DIFF(x, px[1]);
		int h = DIFF(y, py[1]);

		ctx->last  = cur;
		ctx->final = (Rect){ x, y, w, h };

		int b = ctx->border_width;
		XRectangle r[] = {
			{ x-b,   y-b,   w+b,   b   }, /* top */
			{ x-b,   y,     b,     h+b }, /* left */
			{ x,     y+h,   w+b,   b   }, /* bottom */
			{ x+w,   y-b,   b,     h+b }, /* right */
		};
		XShapeCombineRectangles(
			x11->dpy, x11->win, ShapeBounding, 0, 0,
			r, ARRLEN(r), ShapeSet, 0
		);

		if (ctx->state == STATE_INIT) {
			LOG("[sx4]: mapping");
			XMapWindow(x11->dpy, x11->win);
			ctx->state = STATE_DRAW;
		}

		Cursor c = x11->dirs[((cur.x > ctx->start.x) << 1) | (cur.y > ctx->start.y)];
		XChangeActivePointerGrab(x11->dpy, CUR_GRAB_MASK, c, CurrentTime);

		XFlush(x11->dpy);
	}

	if (features & F_CLEAN) {
		LOG("[sx4]: F_CLEAN");
		ASSERT(ctx->state >= STATE_INIT && ctx->state < STATE_EXIT_LOOP);
		XDestroyWindow(x11->dpy, x11->win);
		bool destroyed = false, unmapped = (ctx->state == STATE_INIT);
		for (XEvent ev; !(destroyed && unmapped); NOP()) {
			switch (XNextEvent(x11->dpy, &ev), ev.type) {
			case DestroyNotify:
				destroyed = (ev.xdestroywindow.window == x11->win);
				break;
			case UnmapNotify:
				unmapped = (ev.xunmap.window == x11->win);
				break;
			}
		}
	}

	if (features & F_FINISH) {
		ASSERT(ctx->state == STATE_DRAW);
		ASSERT(!(features & F_ABORT));
		ctx->state = STATE_RECT;
	} else if (features & F_ABORT) {
		ctx->state = STATE_ABORT;
	}
}

extern int
main(int argc, char *argv[])
{
	Str usage = S("Usage: sx4: [-Bkhv] [-b width] [-c color]\n");
	Str version = S(
		"sx4 " VERSION "\n"
		"Copyright (C) 2023 NRK.\n"
		"License: GPLv3+ <https://gnu.org/licenses/gpl.html>\n"
		"Upstream: <https://codeberg.org/NRK/sx4>\n"
	);

	X11 x11;
	DrawCtx ctx = {
		.color_name = "#ff3838",
		.border_width = 3,
	};
	enum {
		F_WM_BORDER  =  1 <<  0,
		F_KEYBOARD   =  1 <<  1,
		F_MAUS       =  1 <<  2,
		F_ROOT       =  1 <<  3,
		F_CURSOR     =  1 <<  4,
		F_COMP_WARN  =  1 <<  5,
	};
	u32 features = F_KEYBOARD | F_MAUS | F_CURSOR | F_ROOT | F_COMP_WARN;

	for (int i = 1; i < argc; ++i) {
		i64 tmpi;
		Str a = str_from_cstr(argv[i]);
		if (str_eq(a, S("--keep-border")) || str_eq(a, S("-B"))) {
			features |= F_WM_BORDER;
		} else if (str_eq(a, S("--border-width")) || str_eq(a, S("-b"))) {
			Str tmp = str_from_cstr(argv[++i]);
			// 512 pixels ought to be enough for everyone
			if (!str_to_i64(tmp, &tmpi) || tmpi > 512 || tmpi < 0) {
				fatal(S("invalid border width\n"));
			}
			ctx.border_width = tmpi;
		} else if (str_eq(a, S("--color")) || str_eq(a, S("-c"))) {
			ctx.color_name = argv[++i];
			if (ctx.color_name == NULL) {
				fatal(S("invalid color name\n"));
			}
		} else if (str_eq(a, S("--no-keyboard")) || str_eq(a, S("-k"))) {
			features &= ~F_KEYBOARD;
		} else if (str_eq(a, S("--window")) || str_eq(a, S("-w"))) {
			Str tmp = str_from_cstr(argv[++i]);
			if (!(str_to_i64(tmp, &tmpi) || str_to_hex64(tmp, &tmpi)) ||
			    tmpi < 0 || (u64)tmpi > (Window)-1)
			{
				fatal(S("invalid window ID\n"));
			}
			ctx.state = STATE_WINDOW;
			ctx.target = tmpi;
			features = 0x0;
		} else if (str_eq(a, S("--help")) || str_eq(a, S("-h"))) {
			fatal(usage);
		} else if (str_eq(a, S("--version")) || str_eq(a, S("-v"))) {
			fatal(version);
		} else {
			fatal(S("unknown argument: `"), a, S("`\n"));
		}
	}

	if ((x11.dpy = XOpenDisplay(NULL)) == NULL) {
		fatal(S("failed to open X display\n"));
	}

	if (features & F_COMP_WARN) {
		u8 buf[64] = "_NET_WM_CM_S";
		int_to_str(buf + S("_NET_WM_CM_S").len, DefaultScreen(x11.dpy));
		Atom cm = XInternAtom(x11.dpy, (char *)buf, False);
		if (XGetSelectionOwner(x11.dpy, cm) != None) {
			Str msg = S("compositor detected, things may be broken!!\n");
			fwrite(msg.s, 1, msg.len, stderr);
		}
	}

	if (features & F_ROOT) {
		XWindowAttributes tmp;
		x11.root.win = DefaultRootWindow(x11.dpy);
		if (XGetWindowAttributes(x11.dpy, x11.root.win, &tmp) == 0) {
			fatal(S("XGetWindowAttributes() failed\n"));
		}
		x11.root.x = tmp.x;
		x11.root.y = tmp.y;
		x11.root.w = tmp.width;
		x11.root.h = tmp.height;
	}

	if (features & F_CURSOR) {
		x11.dirs[0x0] = XCreateFontCursor(x11.dpy, XC_ul_angle);
		x11.dirs[0x1] = XCreateFontCursor(x11.dpy, XC_ll_angle);
		x11.dirs[0x2] = XCreateFontCursor(x11.dpy, XC_ur_angle);
		x11.dirs[0x3] = XCreateFontCursor(x11.dpy, XC_lr_angle);
	}

	if (features & F_KEYBOARD) {
		/* when launched via dwm keybinding, it fails the grab since
		 * dwm has it grabbed already. listen for FocusChangeMask and
		 * keep retrying. */
		int res;
		XSelectInput(x11.dpy, x11.root.win, FocusChangeMask);
		do {
			res = XGrabKeyboard(
				x11.dpy, x11.root.win, 0,
				GrabModeAsync, GrabModeAsync, CurrentTime
			);
			XNextEvent(x11.dpy, (XEvent []){0});
		} while (res == AlreadyGrabbed);
		XSelectInput(x11.dpy, x11.root.win, 0x0);
		if (res != GrabSuccess) {
			fatal(S("failed to grab keyboard\n"));
		}
	}

	if (features & F_MAUS) {
		x11.cross = XCreateFontCursor(x11.dpy, XC_cross);
		int res = XGrabPointer(
			x11.dpy, x11.root.win, 0,
			ButtonPressMask, GrabModeAsync,
			GrabModeAsync, x11.root.win, x11.cross, CurrentTime
		);
		if (res != GrabSuccess) {
			fatal(S("failed to grab cursor\n"));
		}
	}

	bool queued = false;
	for (XEvent ev; ctx.state < STATE_EXIT_LOOP; NOP()) {
		if (!queued) {
			XNextEvent(x11.dpy, &ev);
		}
		queued = false;

		switch (ev.type) {
		case KeyPress: // TODO?: more rich keyboard controls
			switch (XKeycodeToKeysym(x11.dpy, ev.xkey.keycode, 0)) {
			case XK_space:
				draw(&ctx, &x11, (Point){ ev.xkey.x, ev.xkey.y }, EV_CLICK);
				break;
			case XK_f:
				draw(&ctx, &x11, (Point){ ev.xkey.x, ev.xkey.y }, EV_SWAP);
				break;
			case XK_q:
			case XK_Escape:
				draw(&ctx, &x11, (Point){ ev.xkey.x, ev.xkey.y }, EV_ABORT);
				break;
			}
			break;
		case ButtonPress: {
			int btn = ev.xbutton.button;
			Point p = { ev.xbutton.x, ev.xbutton.y };
			if (btn == Button1) {
				draw(&ctx, &x11, p, EV_CLICK);
			} else if (btn == Button3 && ctx.state == STATE_WAIT) {
				ctx.state = STATE_WINDOW;
				ctx.target = ev.xbutton.subwindow;
			} else if ((btn == Button4 || btn == Button5) && ctx.state == STATE_DRAW) {
				draw(&ctx, &x11, p, EV_SWAP);
			} else {
				draw(&ctx, &x11, p, EV_ABORT);
			}
		} break;
		case MotionNotify: {
			Point p = (Point){ ev.xmotion.x, ev.xmotion.y };
			int ndiscarded = 0;
			for (int n = 0; n > 0 || (n = XPending(x11.dpy)) > 0; --n) {
				XNextEvent(x11.dpy, &ev);
				if (ev.type == MotionNotify) {
					p = (Point){ ev.xmotion.x, ev.xmotion.y };
					++ndiscarded;
				} else {
					queued = true;
					LOG("[sx4]: queuing event");
					break;
				}
			}
			if (ndiscarded) LOG("[sx4]: discarded %d stale MotionNotify event", ndiscarded);
			draw(&ctx, &x11, p, EV_MOTION);
		} break;
		case Expose:
			LOG("[sx4]: Expose...");
			draw(&ctx, &x11, ctx.last, EV_MOTION);
			break;
		case DestroyNotify:
			fatal(S("recieved DestroyNotify\n"));
			break;
		case KeyRelease: case ButtonRelease: case MapNotify:
			NOP(); /* ignored */
			break;
		default:
			LOG("[sx4]: Event(%d)...", ev.type);
			break;
		}
	}

	if (ctx.state == STATE_WINDOW) {
		XWindowAttributes tmp;
		if (XGetWindowAttributes(x11.dpy, ctx.target, &tmp) == 0) {
			fatal(S("XGetWindowAttributes() failed\n"));
		}
		int b = tmp.border_width;
		ctx.final = (Rect){ tmp.x, tmp.y, tmp.width, tmp.height };
		if (features & F_WM_BORDER) {
			ctx.final.w += b * 2;
			ctx.final.h += b * 2;
		} else {
			ctx.final.x += b;
			ctx.final.y += b;
		}
	}

	if (ctx.state != STATE_ABORT && ctx.final.w > 0 && ctx.final.h > 0) {
		ASSERT(ctx.state == STATE_WINDOW || ctx.state == STATE_RECT);
		u8 buf[32 * 4], *p = buf;
		p = int_to_str(p, ctx.final.x); *p++ = ',';
		p = int_to_str(p, ctx.final.y); *p++ = ',';
		p = int_to_str(p, ctx.final.w); *p++ = ',';
		p = int_to_str(p, ctx.final.h); *p++ = '\n';
		ASSERT(p < (buf + SIZEOF(buf)));
		fwrite(buf, 1, p - buf, stdout);
		fflush(stdout);
		if (ferror(stdout)) {
			fatal(S("failed to write to stdout\n"));
		}
	}

#ifdef DEBUG
	if (features & F_MAUS) {
		XUngrabPointer(x11.dpy, CurrentTime);
	}
	if (features & F_KEYBOARD) {
		XUngrabKeyboard(x11.dpy, CurrentTime);
	}
#endif
	XCloseDisplay(x11.dpy);

	return 0;
}

// TODO: fix glitches under certain compositors/WMs
// TODO?: --highlight
// TODO?: --no-maus
// TODO?: --format
