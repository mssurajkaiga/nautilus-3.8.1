/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* nautilus-view.h
 *
 * Copyright (C) 1999, 2000  Free Software Foundaton
 * Copyright (C) 2000, 2001  Eazel, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Authors: Ettore Perazzoli
 * 	    Darin Adler <darin@bentspoon.com>
 * 	    John Sullivan <sullivan@eazel.com>
 *          Pavel Cisler <pavel@eazel.com>
 */

#ifndef NAUTILUS_VIEW_H
#define NAUTILUS_VIEW_H

#include <gtk/gtk.h>
#include <gio/gio.h>

#include <libnautilus-private/nautilus-directory.h>
#include <libnautilus-private/nautilus-file.h>
#include <libnautilus-private/nautilus-link.h>

typedef struct NautilusView NautilusView;
typedef struct NautilusViewClass NautilusViewClass;

#include "nautilus-window.h"
#include "nautilus-window-slot.h"

#define NAUTILUS_TYPE_VIEW nautilus_view_get_type()
#define NAUTILUS_VIEW(obj)\
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), NAUTILUS_TYPE_VIEW, NautilusView))
#define NAUTILUS_VIEW_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_CAST ((klass), NAUTILUS_TYPE_VIEW, NautilusViewClass))
#define NAUTILUS_IS_VIEW(obj)\
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), NAUTILUS_TYPE_VIEW))
#define NAUTILUS_IS_VIEW_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_TYPE ((klass), NAUTILUS_TYPE_VIEW))
#define NAUTILUS_VIEW_GET_CLASS(obj)\
	(G_TYPE_INSTANCE_GET_CLASS ((obj), NAUTILUS_TYPE_VIEW, NautilusViewClass))

typedef struct NautilusViewDetails NautilusViewDetails;

struct NautilusView {
	GtkScrolledWindow parent;

	NautilusViewDetails *details;
};

struct NautilusViewClass {
	GtkScrolledWindowClass parent_class;

	/* The 'clear' signal is emitted to empty the view of its contents.
	 * It must be replaced by each subclass.
	 */
	void 	(* clear) 		 (NautilusView *view);
	
	/* The 'begin_file_changes' signal is emitted before a set of files
	 * are added to the view. It can be replaced by a subclass to do any 
	 * necessary preparation for a set of new files. The default
	 * implementation does nothing.
	 */
	void 	(* begin_file_changes) (NautilusView *view);
	
	/* The 'add_file' signal is emitted to add one file to the view.
	 * It must be replaced by each subclass.
	 */
	void    (* add_file) 		 (NautilusView *view, 
					  NautilusFile *file,
					  NautilusDirectory *directory);
	void    (* remove_file)		 (NautilusView *view, 
					  NautilusFile *file,
					  NautilusDirectory *directory);

	/* The 'file_changed' signal is emitted to signal a change in a file,
	 * including the file being removed.
	 * It must be replaced by each subclass.
	 */
	void 	(* file_changed)         (NautilusView *view, 
					  NautilusFile *file,
					  NautilusDirectory *directory);

	/* The 'end_file_changes' signal is emitted after a set of files
	 * are added to the view. It can be replaced by a subclass to do any 
	 * necessary cleanup (typically, cleanup for code in begin_file_changes).
	 * The default implementation does nothing.
	 */
	void 	(* end_file_changes)    (NautilusView *view);
	
	/* The 'begin_loading' signal is emitted before any of the contents
	 * of a directory are added to the view. It can be replaced by a 
	 * subclass to do any necessary preparation to start dealing with a
	 * new directory. The default implementation does nothing.
	 */
	void 	(* begin_loading) 	 (NautilusView *view);

	/* The 'end_loading' signal is emitted after all of the contents
	 * of a directory are added to the view. It can be replaced by a 
	 * subclass to do any necessary clean-up. The default implementation 
	 * does nothing.
	 *
	 * If all_files_seen is true, the handler may assume that
	 * no load error ocurred, and all files of the underlying
	 * directory were loaded.
	 *
	 * Otherwise, end_loading was emitted due to cancellation,
	 * which usually means that not all files are available.
	 */
	void 	(* end_loading) 	 (NautilusView *view,
					  gboolean all_files_seen);

	/* Function pointers that don't have corresponding signals */

        /* reset_to_defaults is a function pointer that subclasses must 
         * override to set sort order, zoom level, etc to match default
         * values. 
         */
        void     (* reset_to_defaults)	         (NautilusView *view);

	/* get_backing uri is a function pointer for subclasses to
	 * override. Subclasses may replace it with a function that
	 * returns the URI for the location where to create new folders,
	 * files, links and paste the clipboard to.
	 */

	char *	(* get_backing_uri)		(NautilusView *view);

	/* get_selection is not a signal; it is just a function pointer for
	 * subclasses to replace (override). Subclasses must replace it
	 * with a function that returns a newly-allocated GList of
	 * NautilusFile pointers.
	 */
	GList *	(* get_selection) 	 	(NautilusView *view);
	
	/* get_selection_for_file_transfer  is a function pointer for
	 * subclasses to replace (override). Subclasses must replace it
	 * with a function that returns a newly-allocated GList of
	 * NautilusFile pointers. The difference from get_selection is
	 * that any files in the selection that also has a parent folder
	 * in the selection is not included.
	 */
	GList *	(* get_selection_for_file_transfer)(NautilusView *view);
	
        /* select_all is a function pointer that subclasses must override to
         * select all of the items in the view */
        void     (* select_all)	         	(NautilusView *view);

        /* select_first is a function pointer that subclasses must override to
         * select the first item in the view */
        void     (* select_first)	      	(NautilusView *view);

        /* set_selection is a function pointer that subclasses must
         * override to select the specified items (and unselect all
         * others). The argument is a list of NautilusFiles. */

        void     (* set_selection)	 	(NautilusView *view, 
        					 GList *selection);
        					 
        /* invert_selection is a function pointer that subclasses must
         * override to invert selection. */

        void     (* invert_selection)	 	(NautilusView *view);        					 

	/* Return an array of locations of selected icons in their view. */
	GArray * (* get_selected_icon_locations) (NautilusView *view);

        /* bump_zoom_level is a function pointer that subclasses must override
         * to change the zoom level of an object. */
        void    (* bump_zoom_level)      	(NautilusView *view,
					  	 int zoom_increment);

        /* zoom_to_level is a function pointer that subclasses must override
         * to set the zoom level of an object to the specified level. */
        void    (* zoom_to_level) 		(NautilusView *view, 
        				         NautilusZoomLevel level);

        NautilusZoomLevel (* get_zoom_level)    (NautilusView *view);

	/* restore_default_zoom_level is a function pointer that subclasses must override
         * to restore the zoom level of an object to a default setting. */
        void    (* restore_default_zoom_level) (NautilusView *view);

        /* can_zoom_in is a function pointer that subclasses must override to
         * return whether the view is at maximum size (furthest-in zoom level) */
        gboolean (* can_zoom_in)	 	(NautilusView *view);

        /* can_zoom_out is a function pointer that subclasses must override to
         * return whether the view is at minimum size (furthest-out zoom level) */
        gboolean (* can_zoom_out)	 	(NautilusView *view);
        
        /* reveal_selection is a function pointer that subclasses may
         * override to make sure the selected items are sufficiently
         * apparent to the user (e.g., scrolled into view). By default,
         * this does nothing.
         */
        void     (* reveal_selection)	 	(NautilusView *view);

        /* merge_menus is a function pointer that subclasses can override to
         * add their own menu items to the window's menu bar.
         * If overridden, subclasses must call parent class's function.
         */
        void    (* merge_menus)         	(NautilusView *view);
        void    (* unmerge_menus)         	(NautilusView *view);

        /* update_menus is a function pointer that subclasses can override to
         * update the sensitivity or wording of menu items in the menu bar.
         * It is called (at least) whenever the selection changes. If overridden, 
         * subclasses must call parent class's function.
         */
        void    (* update_menus)         	(NautilusView *view);

	/* sort_files is a function pointer that subclasses can override
	 * to provide a sorting order to determine which files should be
	 * presented when only a partial list is provided.
	 */
	int     (* compare_files)              (NautilusView *view,
						NautilusFile    *a,
						NautilusFile    *b);

	/* using_manual_layout is a function pointer that subclasses may
	 * override to control whether or not items can be freely positioned
	 * on the user-visible area.
	 * Note that this value is not guaranteed to be constant within the
	 * view's lifecycle. */
	gboolean (* using_manual_layout)     (NautilusView *view);

	/* is_read_only is a function pointer that subclasses may
	 * override to control whether or not the user is allowed to
	 * change the contents of the currently viewed directory. The
	 * default implementation checks the permissions of the
	 * directory.
	 */
	gboolean (* is_read_only)	        (NautilusView *view);

	/* is_empty is a function pointer that subclasses must
	 * override to report whether the view contains any items.
	 */
	gboolean (* is_empty)                   (NautilusView *view);

	gboolean (* can_rename_file)            (NautilusView *view,
						 NautilusFile *file);
	/* select_all specifies whether the whole filename should be selected
	 * or only its basename (i.e. everything except the extension)
	 * */
	void	 (* start_renaming_file)        (NautilusView *view,
					  	 NautilusFile *file,
						 gboolean select_all);

	/* convert *point from widget's coordinate system to a coordinate
	 * system used for specifying file operation positions, which is view-specific.
	 *
	 * This is used by the the icon view, which converts the screen position to a zoom
	 * level-independent coordinate system.
	 */
	void (* widget_to_file_operation_position) (NautilusView *view,
						    GdkPoint     *position);

	/* Preference change callbacks, overriden by icon and list views. 
	 * Icon and list views respond by synchronizing to the new preference
	 * values and forcing an update if appropriate.
	 */
	void	(* click_policy_changed)	   (NautilusView *view);
	void	(* sort_directories_first_changed) (NautilusView *view);

	/* Get the id string for this view. Its a constant string, not memory managed */
	const char *   (* get_view_id)            (NautilusView          *view);

	/* Return the uri of the first visible file */	
	char *         (* get_first_visible_file) (NautilusView          *view);
	/* Scroll the view so that the file specified by the uri is at the top
	   of the view */
	void           (* scroll_to_file)	  (NautilusView          *view,
						   const char            *uri);

        /* Signals used only for keybindings */
        gboolean (* trash)                         (NautilusView *view);
        gboolean (* delete)                        (NautilusView *view);
};

/* GObject support */
GType               nautilus_view_get_type                         (void);

/* Functions callable from the user interface and elsewhere. */
NautilusWindowSlot *nautilus_view_get_nautilus_window_slot     (NautilusView  *view);
char *              nautilus_view_get_uri                          (NautilusView  *view);

void                nautilus_view_display_selection_info           (NautilusView  *view);

GdkAtom	            nautilus_view_get_copied_files_atom            (NautilusView  *view);
gboolean            nautilus_view_get_active                       (NautilusView  *view);

/* Wrappers for signal emitters. These are normally called 
 * only by NautilusView itself. They have corresponding signals
 * that observers might want to connect with.
 */
gboolean            nautilus_view_get_loading                      (NautilusView  *view);

/* Hooks for subclasses to call. These are normally called only by 
 * NautilusView and its subclasses 
 */
void                nautilus_view_activate_files                   (NautilusView        *view,
								    GList                  *files,
								    NautilusWindowOpenFlags flags,
								    gboolean                confirm_multiple);
void                nautilus_view_preview_files                    (NautilusView        *view,
								    GList               *files,
								    GArray              *locations);
void                nautilus_view_start_batching_selection_changes (NautilusView  *view);
void                nautilus_view_stop_batching_selection_changes  (NautilusView  *view);
void                nautilus_view_notify_selection_changed         (NautilusView  *view);
GtkUIManager *      nautilus_view_get_ui_manager                   (NautilusView  *view);
NautilusDirectory  *nautilus_view_get_model                        (NautilusView  *view);
NautilusFile       *nautilus_view_get_directory_as_file            (NautilusView  *view);
void                nautilus_view_pop_up_background_context_menu   (NautilusView  *view,
								    GdkEventButton   *event);
void                nautilus_view_pop_up_selection_context_menu    (NautilusView  *view,
								    GdkEventButton   *event); 
gboolean            nautilus_view_should_show_file                 (NautilusView  *view,
								    NautilusFile     *file);
gboolean	    nautilus_view_should_sort_directories_first    (NautilusView  *view);
void                nautilus_view_ignore_hidden_file_preferences   (NautilusView  *view);
void                nautilus_view_set_show_foreign                 (NautilusView  *view,
								    gboolean          show_foreign);
gboolean            nautilus_view_handle_scroll_event              (NautilusView  *view,
								    GdkEventScroll   *event);

void                nautilus_view_freeze_updates                   (NautilusView  *view);
void                nautilus_view_unfreeze_updates                 (NautilusView  *view);
gboolean            nautilus_view_get_is_renaming                  (NautilusView  *view);
void                nautilus_view_set_is_renaming                  (NautilusView  *view,
								    gboolean       renaming);
void                nautilus_view_add_subdirectory                (NautilusView  *view,
								   NautilusDirectory*directory);
void                nautilus_view_remove_subdirectory             (NautilusView  *view,
								   NautilusDirectory*directory);

gboolean            nautilus_view_is_editable                     (NautilusView *view);

/* NautilusView methods */
const char *      nautilus_view_get_view_id                (NautilusView      *view);

/* file operations */
char *            nautilus_view_get_backing_uri            (NautilusView      *view);
void              nautilus_view_move_copy_items            (NautilusView      *view,
							    const GList       *item_uris,
							    GArray            *relative_item_points,
							    const char        *target_uri,
							    int                copy_action,
							    int                x,
							    int                y);
void              nautilus_view_new_file_with_initial_contents (NautilusView *view,
								const char *parent_uri,
								const char *filename,
								const char *initial_contents,
								int length,
								GdkPoint *pos);

/* selection handling */
void              nautilus_view_activate_selection         (NautilusView      *view);
int               nautilus_view_get_selection_count        (NautilusView      *view);
GList *           nautilus_view_get_selection              (NautilusView      *view);
void              nautilus_view_set_selection              (NautilusView      *view,
							    GList             *selection);


void              nautilus_view_load_location              (NautilusView      *view,
							    GFile             *location);
void              nautilus_view_stop_loading               (NautilusView      *view);

char *            nautilus_view_get_first_visible_file     (NautilusView      *view);
void              nautilus_view_scroll_to_file             (NautilusView      *view,
							    const char        *uri);
char *            nautilus_view_get_title                  (NautilusView      *view);
gboolean          nautilus_view_supports_zooming           (NautilusView      *view);
void              nautilus_view_bump_zoom_level            (NautilusView      *view,
							    int                zoom_increment);
void              nautilus_view_zoom_to_level              (NautilusView      *view,
							    NautilusZoomLevel  level);
void              nautilus_view_restore_default_zoom_level (NautilusView      *view);
gboolean          nautilus_view_can_zoom_in                (NautilusView      *view);
gboolean          nautilus_view_can_zoom_out               (NautilusView      *view);
NautilusZoomLevel nautilus_view_get_zoom_level             (NautilusView      *view);
void              nautilus_view_pop_up_location_context_menu (NautilusView    *view,
							      GdkEventButton  *event,
							      const char      *location);
void              nautilus_view_grab_focus                 (NautilusView      *view);
void              nautilus_view_update_menus               (NautilusView      *view);

gboolean          nautilus_view_get_show_hidden_files      (NautilusView      *view);

#endif /* NAUTILUS_VIEW_H */
