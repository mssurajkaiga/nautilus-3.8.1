/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* nautilus-file-utilities.h - interface for file manipulation routines.

   Copyright (C) 1999, 2000, 2001 Eazel, Inc.

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Authors: John Sullivan <sullivan@eazel.com>
*/

#ifndef NAUTILUS_FILE_UTILITIES_H
#define NAUTILUS_FILE_UTILITIES_H

#include <gio/gio.h>
#include <gtk/gtk.h>

#define NAUTILUS_SAVED_SEARCH_EXTENSION ".savedSearch"
#define NAUTILUS_SAVED_SEARCH_MIMETYPE "application/x-gnome-saved-search"

/* These functions all return something something that needs to be
 * freed with g_free, is not NULL, and is guaranteed to exist.
 */
char *   nautilus_get_xdg_dir                        (const char *type);
char *   nautilus_get_user_directory                 (void);
char *   nautilus_get_desktop_directory              (void);
GFile *  nautilus_get_desktop_location               (void);
char *   nautilus_get_desktop_directory_uri          (void);
char *   nautilus_get_home_directory_uri             (void);
gboolean nautilus_is_desktop_directory_file          (GFile *dir,
						      const char *filename);
gboolean nautilus_is_root_directory                  (GFile *dir);
gboolean nautilus_is_desktop_directory               (GFile *dir);
gboolean nautilus_is_home_directory                  (GFile *dir);
gboolean nautilus_is_home_directory_file             (GFile *dir,
						      const char *filename);
gboolean nautilus_is_in_system_dir                   (GFile *location);
GMount * nautilus_get_mounted_mount_for_root         (GFile *location);

gboolean nautilus_should_use_templates_directory     (void);
char *   nautilus_get_templates_directory            (void);
char *   nautilus_get_templates_directory_uri        (void);
void     nautilus_create_templates_directory         (void);

char *   nautilus_get_searches_directory             (void);

char *	 nautilus_compute_title_for_location	     (GFile *file);

gboolean nautilus_is_file_roller_installed           (void);

GIcon *  nautilus_special_directory_get_icon         (GUserDirectory directory);
GIcon *  nautilus_special_directory_get_symbolic_icon (GUserDirectory directory);

gboolean nautilus_uri_parse                          (const char  *uri,
						      char       **host,
						      guint16     *port,
						      char       **userinfo);

/* Return an allocated file name that is guranteed to be unique, but
 * tries to make the name readable to users.
 * This isn't race-free, so don't use for security-related things
 */
char *   nautilus_ensure_unique_file_name            (const char *directory_uri,
						      const char *base_name,
			                              const char *extension);

GFile *  nautilus_find_existing_uri_in_hierarchy     (GFile *location);

char * nautilus_get_accel_map_file (void);
char * nautilus_get_scripts_directory_path (void);

GHashTable * nautilus_trashed_files_get_original_directories (GList *files,
							      GList **unhandled_files);
void nautilus_restore_files_from_trash (GList *files,
					GtkWindow *parent_window);

typedef void (*NautilusMountGetContent) (const char **content, gpointer user_data);

char ** nautilus_get_cached_x_content_types_for_mount (GMount *mount);
void nautilus_get_x_content_types_for_mount_async (GMount *mount,
						   NautilusMountGetContent callback,
						   GCancellable *cancellable,
						   gpointer user_data);

gboolean nautilus_file_selection_equal (GList *selection_a, GList *selection_b);

#endif /* NAUTILUS_FILE_UTILITIES_H */
