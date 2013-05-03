/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* nautilus-global-preferences.h - Nautilus specific preference keys and
                                   functions.

   Copyright (C) 1999, 2000, 2001 Eazel, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Authors: Ramiro Estrugo <ramiro@eazel.com>
*/

#ifndef NAUTILUS_GLOBAL_PREFERENCES_H
#define NAUTILUS_GLOBAL_PREFERENCES_H

#include <libnautilus-private/nautilus-global-preferences.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/* Trash options */
#define NAUTILUS_PREFERENCES_CONFIRM_TRASH			"confirm-trash"
#define NAUTILUS_PREFERENCES_ENABLE_DELETE			"enable-delete"

/* Display  */
#define NAUTILUS_PREFERENCES_SHOW_HIDDEN_FILES			"show-hidden"

/* Mouse */
#define NAUTILUS_PREFERENCES_MOUSE_USE_EXTRA_BUTTONS		"mouse-use-extra-buttons"
#define NAUTILUS_PREFERENCES_MOUSE_FORWARD_BUTTON		"mouse-forward-button"
#define NAUTILUS_PREFERENCES_MOUSE_BACK_BUTTON			"mouse-back-button"

typedef enum
{
	NAUTILUS_NEW_TAB_POSITION_AFTER_CURRENT_TAB,
	NAUTILUS_NEW_TAB_POSITION_END,
} NautilusNewTabPosition;

/* Single/Double click preference  */
#define NAUTILUS_PREFERENCES_CLICK_POLICY			"click-policy"

/* Activating executable text files */
#define NAUTILUS_PREFERENCES_EXECUTABLE_TEXT_ACTIVATION		"executable-text-activation"

/* Installing new packages when unknown mime type activated */
#define NAUTILUS_PREFERENCES_INSTALL_MIME_ACTIVATION		"install-mime-activation"

/* Spatial or browser mode */
#define NAUTILUS_PREFERENCES_NEW_TAB_POSITION			"tabs-open-position"

#define NAUTILUS_PREFERENCES_ALWAYS_USE_LOCATION_ENTRY		"always-use-location-entry"

/* Which views should be displayed for new windows */
#define NAUTILUS_WINDOW_STATE_START_WITH_SIDEBAR               "start-with-sidebar"
#define NAUTILUS_WINDOW_STATE_GEOMETRY				"geometry"
#define NAUTILUS_WINDOW_STATE_MAXIMIZED				"maximized"
#define NAUTILUS_WINDOW_STATE_SIDEBAR_WIDTH			"sidebar-width"

/* Sorting order */
#define NAUTILUS_PREFERENCES_SORT_DIRECTORIES_FIRST		"sort-directories-first"
#define NAUTILUS_PREFERENCES_DEFAULT_SORT_ORDER			"default-sort-order"
#define NAUTILUS_PREFERENCES_DEFAULT_SORT_IN_REVERSE_ORDER	"default-sort-in-reverse-order"

/* The default folder viewer - one of the two enums below */
#define NAUTILUS_PREFERENCES_DEFAULT_FOLDER_VIEWER		"default-folder-viewer"

enum
{
	NAUTILUS_DEFAULT_FOLDER_VIEWER_ICON_VIEW,
	NAUTILUS_DEFAULT_FOLDER_VIEWER_LIST_VIEW,
	NAUTILUS_DEFAULT_FOLDER_VIEWER_OTHER
};

/* These IIDs are used by the preferences code and in nautilus-application.c */
#define NAUTILUS_CANVAS_VIEW_IID	"OAFIID:Nautilus_File_Manager_Canvas_View"
#define NAUTILUS_LIST_VIEW_IID		"OAFIID:Nautilus_File_Manager_List_View"


/* Icon View */
#define NAUTILUS_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL		"default-zoom-level"

/* Which text attributes appear beneath icon names */
#define NAUTILUS_PREFERENCES_ICON_VIEW_CAPTIONS				"captions"

/* The default size for thumbnail icons */
#define NAUTILUS_PREFERENCES_ICON_VIEW_THUMBNAIL_SIZE			"thumbnail-size"

/* ellipsization preferences */
#define NAUTILUS_PREFERENCES_ICON_VIEW_TEXT_ELLIPSIS_LIMIT		"text-ellipsis-limit"
#define NAUTILUS_PREFERENCES_DESKTOP_TEXT_ELLIPSIS_LIMIT		"text-ellipsis-limit"

/* List View */
#define NAUTILUS_PREFERENCES_LIST_VIEW_DEFAULT_ZOOM_LEVEL		"default-zoom-level"
#define NAUTILUS_PREFERENCES_LIST_VIEW_DEFAULT_VISIBLE_COLUMNS		"default-visible-columns"
#define NAUTILUS_PREFERENCES_LIST_VIEW_DEFAULT_COLUMN_ORDER		"default-column-order"
#define NAUTILUS_PREFERENCES_LIST_VIEW_USE_TREE                         "use-tree-view"

enum
{
	NAUTILUS_CLICK_POLICY_SINGLE,
	NAUTILUS_CLICK_POLICY_DOUBLE
};

enum
{
	NAUTILUS_EXECUTABLE_TEXT_LAUNCH,
	NAUTILUS_EXECUTABLE_TEXT_DISPLAY,
	NAUTILUS_EXECUTABLE_TEXT_ASK
};

typedef enum
{
	NAUTILUS_SPEED_TRADEOFF_ALWAYS,
	NAUTILUS_SPEED_TRADEOFF_LOCAL_ONLY,
	NAUTILUS_SPEED_TRADEOFF_NEVER
} NautilusSpeedTradeoffValue;

#define NAUTILUS_PREFERENCES_SHOW_DIRECTORY_ITEM_COUNTS "show-directory-item-counts"
#define NAUTILUS_PREFERENCES_SHOW_FILE_THUMBNAILS	"show-image-thumbnails"
#define NAUTILUS_PREFERENCES_FILE_THUMBNAIL_LIMIT	"thumbnail-limit"

typedef enum
{
	NAUTILUS_COMPLEX_SEARCH_BAR,
	NAUTILUS_SIMPLE_SEARCH_BAR
} NautilusSearchBarMode;

#define NAUTILUS_PREFERENCES_DESKTOP_FONT		   "font"
#define NAUTILUS_PREFERENCES_DESKTOP_HOME_VISIBLE          "home-icon-visible"
#define NAUTILUS_PREFERENCES_DESKTOP_HOME_NAME             "home-icon-name"
#define NAUTILUS_PREFERENCES_DESKTOP_TRASH_VISIBLE         "trash-icon-visible"
#define NAUTILUS_PREFERENCES_DESKTOP_TRASH_NAME            "trash-icon-name"
#define NAUTILUS_PREFERENCES_DESKTOP_VOLUMES_VISIBLE	   "volumes-visible"
#define NAUTILUS_PREFERENCES_DESKTOP_NETWORK_VISIBLE       "network-icon-visible"
#define NAUTILUS_PREFERENCES_DESKTOP_NETWORK_NAME          "network-icon-name"
#define NAUTILUS_PREFERENCES_DESKTOP_BACKGROUND_FADE       "background-fade"

/* bulk rename utility */
#define NAUTILUS_PREFERENCES_BULK_RENAME_TOOL              "bulk-rename-tool"

/* Lockdown */
#define NAUTILUS_PREFERENCES_LOCKDOWN_COMMAND_LINE         "disable-command-line"

/* Desktop background */
#define NAUTILUS_PREFERENCES_SHOW_DESKTOP		   "show-desktop-icons"

/* Recent files */
#define NAUTILUS_PREFERENCES_RECENT_FILES_ENABLED          "remember-recent-files"


void nautilus_global_preferences_init                      (void);
char *nautilus_global_preferences_get_default_folder_viewer_preference_as_iid (void);

GSettings *nautilus_preferences;
GSettings *nautilus_icon_view_preferences;
GSettings *nautilus_list_view_preferences;
GSettings *nautilus_desktop_preferences;
GSettings *nautilus_window_state;
GSettings *gtk_filechooser_preferences;
GSettings *gnome_lockdown_preferences;
GSettings *gnome_background_preferences;
GSettings *gnome_interface_preferences;
GSettings *gnome_privacy_preferences;

G_END_DECLS

#endif /* NAUTILUS_GLOBAL_PREFERENCES_H */
