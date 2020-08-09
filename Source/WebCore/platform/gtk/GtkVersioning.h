/*
 * Copyright (C) 2020 Igalia S.L.
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#pragma once

#include <gtk/gtk.h>

#if USE(GTK4)

#define GDK_MOD1_MASK GDK_ALT_MASK

typedef GdkKeyEvent GdkEventKey;
typedef GdkFocusEvent GdkEventFocus;

static inline gboolean
gtk_widget_is_toplevel(GtkWidget* widget)
{
    // In theory anything which implements GtkRoot can be a toplevel widget,
    // in practice only the ones which are GtkWindow or derived need to be
    // considered here.
    return GTK_IS_WINDOW(widget);
}

static inline GtkWidget*
gtk_widget_get_toplevel(GtkWidget* widget)
{
    return GTK_WIDGET(gtk_widget_get_root(widget));
}

static inline void
gtk_window_get_position(GtkWindow*, int* x, int* y)
{
    *x = *y = 0;
}

static inline void
gtk_init(int*, char***)
{
    gtk_init();
}

static inline gboolean
gtk_init_check(int*, char***)
{
    return gtk_init_check();
}

static inline GdkEvent*
gdk_event_copy(GdkEvent* event)
{
    return gdk_event_ref(event);
}

static inline void
gtk_widget_size_allocate(GtkWidget* widget, GtkAllocation* allocation)
{
    gtk_widget_size_allocate(widget, allocation, -1);
}

static inline void
gtk_widget_queue_resize_no_redraw(GtkWidget* widget)
{
    gtk_widget_queue_resize(widget);
}

static inline gboolean
gdk_event_get_state(const GdkEvent *event, GdkModifierType *state)
{
    *state = gdk_event_get_modifier_state(const_cast<GdkEvent*>(event));
    // The GTK3 method returns TRUE if there is a state, otherwise
    // FALSE.
    return !!*state;
}

static inline gboolean
gdk_event_get_coords(const GdkEvent *event, double *x, double *y)
{
    return gdk_event_get_position(const_cast<GdkEvent*>(event), x, y);
}

static inline gboolean
gdk_event_get_root_coords(const GdkEvent *event, double *x, double *y)
{
    // GTK4 does not provide a way of obtaining screen-relative event coordinates, and even
    // on Wayland GTK3 cannot know where a surface is and will return the surface-relative
    // coordinates anyway, so do the same here.
    return gdk_event_get_position(const_cast<GdkEvent*>(event), x, y);
}

static inline gboolean
gdk_event_is_scroll_stop_event(const GdkEvent* event)
{
    return gdk_scroll_event_is_stop(const_cast<GdkEvent*>(event));
}

static inline gboolean
gdk_event_get_scroll_direction(const GdkEvent* event, GdkScrollDirection* direction)
{
    *direction = gdk_scroll_event_get_direction(const_cast<GdkEvent*>(event));
    // The GTK3 method returns TRUE if the scroll direction is not
    // GDK_SCROLL_SMOOTH, so do the same here.
    return *direction != GDK_SCROLL_SMOOTH;
}

static inline gboolean
gdk_event_get_scroll_deltas(const GdkEvent* event, gdouble *x, gdouble *y)
{
    gdk_scroll_event_get_deltas(const_cast<GdkEvent*>(event), x, y);
    // The GTK3 method returns TRUE if the event is a smooth scroll
    // event, so do the same here.
    return gdk_scroll_event_get_direction(const_cast<GdkEvent*>(event)) == GDK_SCROLL_SMOOTH;
}

static inline gboolean
gdk_event_get_button(const GdkEvent* event, guint* button)
{
    if (button)
        *button = gdk_button_event_get_button(const_cast<GdkEvent*>(event));
    return true;
}

static inline gboolean
gdk_event_get_keyval(const GdkEvent* event, guint* keyval)
{
    if (keyval)
        *keyval = gdk_key_event_get_keyval(const_cast<GdkEvent*>(event));
    return TRUE;
}

static inline gboolean
gdk_event_get_keycode(const GdkEvent* event, guint16* keycode)
{
    if (keycode)
        *keycode = gdk_key_event_get_keycode(const_cast<GdkEvent*>(event));
    return TRUE;
}

static inline GtkWidget* gtk_popover_menu_new()
{
    return gtk_popover_menu_new_from_model(nullptr);
}

static inline void gtk_popover_bind_model(GtkPopover* popover, GMenuModel* model, const char*)
{
    ASSERT(GTK_IS_POPOVER_MENU(popover));
    gtk_popover_menu_set_menu_model(GTK_POPOVER_MENU(popover), model);
}

static inline void gtk_popover_set_relative_to(GtkPopover* popover, GtkWidget* parent)
{
    gtk_widget_set_parent(GTK_WIDGET(popover), parent);
}

#else

static inline void gtk_popover_set_has_arrow(GtkPopover*, gboolean)
{
}

#endif // USE(GTK4)
