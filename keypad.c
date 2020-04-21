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
    { 4, 9,  8, 12, "b0",        "0", callback, },
    { 0, 0,  4,  3, "b1",        "1", callback, },
    { 4, 0,  8,  3, "b2",        "2", callback, },
    { 8, 0, 12,  3, "b3",        "3", callback, },
    { 0, 3,  4,  6, "b4",        "4", callback, },
    { 4, 3,  8,  6, "b5",        "5", callback, },
    { 8, 3, 12,  6, "b6",        "6", callback, },
    { 0, 6,  4,  9, "b7",        "7", callback, },
    { 4, 6,  8,  9, "b8",        "8", callback, },
    { 8, 6, 12,  9, "b9",        "9", callback, },
    { 8, 9, 12, 12, "bhash",     "#", callback, },
    { 0, 9,  4, 12, "basterisk", "*", callback, },
    { 0, 0,  0,  0, NULL, NULL, },
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
        XmNfractionBase, 12,
        NULL);

    if (!form) {
        fprintf(stderr,
            F("Couldn't create form '%s'\n"),
            form_name);
        exit(EXIT_FAILURE);
    }

    struct but_desc *p;
    for (p = pos_tab; p->name; ++p) {
        printf(F("Creating button '%s'@{(%d,%d),(%d,%d)}\n"),
            p->name, p->left_x, p->top_y, p->right_x, p->bot_y);
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
