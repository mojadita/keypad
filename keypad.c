/* main.c -- main program to create a keypad with an XmForm
             widget.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Tue Apr 21 16:15:28 EEST 2020
 * Copyright: (C) 2020 Luis Colorado.  All rights reserved.
 * License: BSD.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushBG.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#define F(_fmt) __FILE__":%d:%s: " _fmt, __LINE__, __func__

/* XmForm has a coordinate set based on a common denominator
 * that it applies to x and y coordinates, in order to subdivide
 * the grid (that has the same subdivisions along the x axis, as
 * it has along the y axis, so in order to make a 4x3 grid we
 * calculate the MCM(3, 4) = 12 to use as the denominator to
 * divide the x and y sides of it.  This makes the coordinates
 * to go as the drawing marks.
 *      0   4   8  12
 *      +---+---+---+  0
 *      | 1 | 2 | 3 |
 *      +---+---+---+  3
 *      | 4 | 5 | 6 |
 *      *---+---+---+  6
 *      | 7 | 8 | 9 |
 *      +---+---+---+  9
 *      | * | 0 | # |
 *      +---+---+---+ 12
 */

struct but_desc {
    int left_x, top_y;
    int right_x, bot_y;
    char *name;
    char *out;
    void (*cb)(Widget, XtPointer, XtPointer);
};

void callback(Widget w, XtPointer data, XtPointer cd);

static struct but_desc
pos_tab[] = {
	/* function keys */
	{ 0, 0, 4, 15, "esc", "<esc>", callback, },
	{ 8, 0, 12, 15, "f1", "<f1>", callback, },
	{ 12, 0, 16, 15, "f2", "<f2>", callback, },
	{ 16, 0, 20, 15, "f3", "<f3>", callback, },
	{ 20, 0, 24, 15, "f4", "<f4>", callback, },
	{ 26, 0, 30, 15, "f5", "<f5>", callback, },
	{ 30, 0, 34, 15, "f6", "<f6>", callback, },
	{ 34, 0, 38, 15, "f7", "<f7>", callback, },
	{ 38, 0, 42, 15, "f8", "<f8>", callback, },
	{ 44, 0, 48, 15, "f9", "<f9>", callback, },
	{ 48, 0, 52, 15, "f10", "<f10>", callback, },
	{ 52, 0, 56, 15, "f11", "<f11>", callback, },
	{ 56, 0, 60, 15, "f12", "<f12>", callback, },
	{ 61, 0, 65, 15, "printscr", "<prt-scr>", callback, },
	{ 65, 0, 69, 15, "scrolllock", "<scrl-lck>", callback, },
	{ 69, 0, 73, 15, "pause", "<pse>", callback, },
 
	/* digits main keyboard */
	{ 0, 15, 4, 30, "lquote", "`", callback, },
	{ 4, 15, 8, 30, "1", "1", callback, },
	{ 8, 15, 12, 30, "2", "2", callback, },
	{ 12, 15, 16, 30, "3", "3", callback, },
	{ 16, 15, 20, 30, "4", "4", callback, },
	{ 20, 15, 24, 30, "5", "5", callback, },
	{ 24, 15, 28, 30, "6", "6", callback, },
	{ 28, 15, 32, 30, "7", "7", callback, },
	{ 32, 15, 36, 30, "8", "8", callback, },
	{ 36, 15, 40, 30, "9", "9", callback, },
	{ 40, 15, 44, 30, "0", "0", callback, },
	{ 44, 15, 48, 30, "dash", "-", callback, },
	{ 48, 15, 52, 30, "equals", "=", callback, },
	{ 52, 15, 60, 30, "backspace", "\b", callback, },

	{ 61, 15, 65, 30, "ins", "<ins>", callback, },
	{ 65, 15, 69, 30, "home", "<home>", callback, },
	{ 69, 15, 73, 30, "pgup", "<pgup>", callback, },

	{ 74, 15, 78, 30, "numlock", "<nlck>", callback, },
	{ 78, 15, 82, 30, "kpdiv", "/", callback, },
	{ 82, 15, 86, 30, "kpmult", "*", callback, },
	{ 86, 15, 90, 30, "kpsub", "-", callback, },
 
	/* second row */
	{ 0, 30, 6, 45, "tab", "\t", callback, },
	{ 6, 30, 10, 45, "q", "Q", callback, },
	{ 10, 30, 14, 45, "w", "W", callback, },
	{ 14, 30, 18, 45, "e", "E", callback, },
	{ 18, 30, 22, 45, "r", "R", callback, },
	{ 22, 30, 26, 45, "t", "T", callback, },
	{ 26, 30, 30, 45, "y", "Y", callback, },
	{ 30, 30, 34, 45, "u", "U", callback, },
	{ 34, 30, 38, 45, "i", "I", callback, },
	{ 38, 30, 42, 45, "o", "O", callback, },
	{ 42, 30, 46, 45, "p", "P", callback, },
	{ 46, 30, 50, 45, "lbr", "[", callback, },
	{ 50, 30, 54, 45, "rbr", "]", callback, },
	{ 54, 30, 60, 45, "backslash", "\\", callback, },

	{ 61, 30, 65, 45, "del", "<del>", callback, },
	{ 65, 30, 69, 45, "end", "<end>", callback, },
	{ 69, 30, 73, 45, "pgdn", "<pgdn>", callback, },

	{ 74, 30, 78, 45, "kp7", "7", callback, },
	{ 78, 30, 82, 45, "kp8", "8", callback, },
	{ 82, 30, 86, 45, "kp9", "9", callback, },
	{ 86, 30, 90, 60, "kpplus", "+", callback, },
 
	{ 0, 45, 7, 60, "caps", "<clck>", callback, },
	{ 7, 45, 11, 60, "a", "A", callback, },
	{ 11, 45, 15, 60, "s", "S", callback, },
	{ 15, 45, 19, 60, "d", "D", callback, },
	{ 19, 45, 23, 60, "f", "F", callback, },
	{ 23, 45, 27, 60, "g", "G", callback, },
	{ 27, 45, 31, 60, "h", "H", callback, },
	{ 31, 45, 35, 60, "j", "J", callback, },
	{ 35, 45, 39, 60, "k", "K", callback, },
	{ 39, 45, 43, 60, "l", "L", callback, },
	{ 43, 45, 47, 60, "semicolon", ";", callback, },
	{ 47, 45, 51, 60, "quote", "'", callback, },
	{ 51, 45, 60, 60, "enter", "\n", callback, },
 
	{ 74, 45, 78, 60, "kp4", "4", callback, },
	{ 78, 45, 82, 60, "kp5", "5", callback, },
	{ 82, 45, 86, 60, "kp6", "6", callback, },
 
	{ 0,  60, 9, 75, "lshift", "<lshft>", callback, },
	/* { 5,  60, 9, 75, "gt_lt", "<", callback, }, */
	{ 9,  60, 13, 75, "z", "Z", callback, },
	{ 13, 60, 17, 75, "x", "X", callback, },
	{ 17, 60, 21, 75, "c", "C", callback, },
	{ 21, 60, 25, 75, "v", "V", callback, },
	{ 25, 60, 29, 75, "b", "B", callback, },
	{ 29, 60, 33, 75, "n", "N", callback, },
	{ 33, 60, 37, 75, "m", "M", callback, },
	{ 37, 60, 41, 75, "comma", ",", callback, },
	{ 41, 60, 45, 75, "dot", ".", callback, },
	{ 45, 60, 49, 75, "slash", "/", callback, },
	{ 49, 60, 60, 75, "rshift", "<rshft>", callback, },
 
	{ 65, 60, 69, 75, "up", "\033[A", callback, },
 
	{ 74, 60, 78, 75, "kp1", "1", callback, },
	{ 78, 60, 82, 75, "kp2", "2", callback, },
	{ 82, 60, 86, 75, "kp3", "3", callback, },
	{ 86, 60, 90, 90, "kpenter", "\n", callback, },
 
	{ 0,  75, 5, 90, "lctrl", "<lctrl>", callback, },
	{ 5,  75, 10, 90, "win", "<win>", callback, },
	{ 10,  75, 15, 90, "alt", "<lalt>", callback, },
	{ 15, 75, 40, 90, "space", " ", callback, },
	{ 40, 75, 45, 90, "ralt", "<ralt>", callback, },
	{ 45, 75, 50, 90, "f_n", "<fn>", callback, },
	{ 50, 75, 55, 90, "menu", "<menu>", callback, },
	{ 55, 75, 60, 90, "rctrl", "<rctrl>", callback, },
 
	{ 61, 75, 65, 90, "left", "\033[D", callback, },
	{ 65, 75, 69, 90, "down", "\033[B", callback, },
	{ 69, 75, 73, 90, "right", "\033[C", callback, },
 
	{ 74, 75, 82, 90, "kp0", "0", callback, },
	{ 82, 75, 86, 90, "kpdot", ".", callback, },

    { 0, 0,  0,  0, NULL, NULL, }, /* sentinel */
};

int
main(int argc, char **argv)
{
    XtAppContext app_ctx;
    char *class_name = "XKeypad";

    Widget topLevel = XtVaAppInitialize(
        &app_ctx,
        class_name,
        NULL, 0, /* XrmOptionDescRec array */
        &argc, argv, /* args to main */
        NULL, /* default resources */
        NULL); /* empty list of arguments */

    if (!topLevel) {
        fprintf(stderr,
        F("Couldnt create main widget"));
        exit(EXIT_FAILURE);
    }

    char *form_name = "form";
    Widget form = XtVaCreateManagedWidget(
        form_name,
        xmFormWidgetClass,
        topLevel,
        XmNfractionBase, 90,
        NULL);

    if (!form) {
        fprintf(stderr,
            F("Couldn't create form '%s'\n"),
            form_name);
        exit(EXIT_FAILURE);
    }

    struct but_desc *p;
    for (p = pos_tab; p->name; ++p) {
#if DEBUG
        printf(F("Creating button '%s'@{(%d,%d),(%d,%d)}\n"),
            p->name, p->left_x, p->top_y, p->right_x, p->bot_y);
#endif
        Widget button =
            XtVaCreateManagedWidget(
                    p->name,
                    xmPushButtonGadgetClass,
                    form,
                    XmNleftAttachment, XmATTACH_POSITION,
                    XmNrightAttachment, XmATTACH_POSITION,
                    XmNtopAttachment, XmATTACH_POSITION,
                    XmNbottomAttachment, XmATTACH_POSITION,
                    XmNleftPosition, p->left_x,
                    XmNrightPosition, p->right_x,
                    XmNtopPosition, p->top_y,
                    XmNbottomPosition, p->bot_y,
                    NULL, NULL);
        if (!button) {
            fprintf(stderr,
                F("Couldnt create button '%s'\n"),
                p->name);
            exit(EXIT_FAILURE);
        }
        XtAddCallback(button, XmNactivateCallback, p->cb, p);
    }

    XtRealizeWidget(topLevel);
    XtAppMainLoop(app_ctx);

} /* main */

void callback(Widget w, XtPointer p, XtPointer cd)
{
    struct but_desc *data = p;
    write(1, data->out, strlen(data->out));
}
