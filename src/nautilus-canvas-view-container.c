/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-icon-container.h - the container widget for file manager icons

   Copyright (C) 2002 Sun Microsystems, Inc.

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

   Author: Michael Meeks <michael@ximian.com>
*/
#include <config.h>

#include "nautilus-canvas-view-container.h"

#include <string.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <eel/eel-glib-extensions.h>
#include <libnautilus-private/nautilus-global-preferences.h>
#include <libnautilus-private/nautilus-file-attributes.h>
#include <libnautilus-private/nautilus-thumbnails.h>
#include <libnautilus-private/nautilus-desktop-icon-file.h>

#define ICON_TEXT_ATTRIBUTES_NUM_ITEMS		3
#define ICON_TEXT_ATTRIBUTES_DEFAULT_TOKENS	"size,date_modified,type"

G_DEFINE_TYPE (NautilusCanvasViewContainer, nautilus_canvas_view_container, NAUTILUS_TYPE_CANVAS_CONTAINER);

static GQuark attribute_none_q;

static NautilusCanvasView *
get_canvas_view (NautilusCanvasContainer *container)
{
	/* Type unsafe comparison for performance */
	return ((NautilusCanvasViewContainer *)container)->view;
}

static NautilusIconInfo *
nautilus_canvas_view_container_get_icon_images (NautilusCanvasContainer *container,
					      NautilusCanvasIconData      *data,
					      int                    size,
					      char                 **embedded_text,
					      gboolean               for_drag_accept,
					      gboolean               need_large_embeddded_text,
					      gboolean              *embedded_text_needs_loading,
					      gboolean              *has_window_open)
{
	NautilusCanvasView *canvas_view;
	NautilusFile *file;
	gboolean use_embedding;
	NautilusFileIconFlags flags;
	NautilusIconInfo *icon_info;
	GdkPixbuf *pixbuf;
	GIcon *emblemed_icon;
	GEmblem *emblem;
	GList *emblem_icons, *l;

	file = (NautilusFile *) data;

	g_assert (NAUTILUS_IS_FILE (file));
	canvas_view = get_canvas_view (container);
	g_return_val_if_fail (canvas_view != NULL, NULL);

	use_embedding = FALSE;
	if (embedded_text) {
		*embedded_text = nautilus_file_peek_top_left_text (file, need_large_embeddded_text, embedded_text_needs_loading);
		use_embedding = *embedded_text != NULL;
	}
	
	*has_window_open = nautilus_file_has_open_window (file);

	flags = NAUTILUS_FILE_ICON_FLAGS_USE_MOUNT_ICON_AS_EMBLEM |
		NAUTILUS_FILE_ICON_FLAGS_USE_THUMBNAILS;

	if (use_embedding) {
		flags |= NAUTILUS_FILE_ICON_FLAGS_EMBEDDING_TEXT;
	}
	if (for_drag_accept) {
		flags |= NAUTILUS_FILE_ICON_FLAGS_FOR_DRAG_ACCEPT;
	}

	icon_info = nautilus_file_get_icon (file, size, flags);
	emblem_icons = nautilus_file_get_emblem_icons (file);

	/* apply emblems */
	if (emblem_icons != NULL) {
		l = emblem_icons;

		emblem = g_emblem_new (l->data);
		pixbuf = nautilus_icon_info_get_pixbuf (icon_info);
		emblemed_icon = g_emblemed_icon_new (G_ICON (pixbuf), emblem);
		g_object_unref (emblem);

		for (l = l->next; l != NULL; l = l->next) {
			emblem = g_emblem_new (l->data);
			g_emblemed_icon_add_emblem (G_EMBLEMED_ICON (emblemed_icon),
						    emblem);
			g_object_unref (emblem);
		}

		g_clear_object (&icon_info);
		icon_info = nautilus_icon_info_lookup (emblemed_icon, size);

		g_object_unref (pixbuf);
		g_object_unref (emblemed_icon);
	}

	if (emblem_icons != NULL) {
		g_list_free_full (emblem_icons, g_object_unref);
	}

	return icon_info;
}

static char *
nautilus_canvas_view_container_get_icon_description (NautilusCanvasContainer *container,
						   NautilusCanvasIconData      *data)
{
	NautilusFile *file;
	char *mime_type;
	const char *description;

	file = NAUTILUS_FILE (data);
	g_assert (NAUTILUS_IS_FILE (file));

	if (NAUTILUS_IS_DESKTOP_ICON_FILE (file)) {
		return NULL;
	}

	mime_type = nautilus_file_get_mime_type (file);
	description = g_content_type_get_description (mime_type);
	g_free (mime_type);
	return g_strdup (description);
}

static void
nautilus_canvas_view_container_start_monitor_top_left (NautilusCanvasContainer *container,
						     NautilusCanvasIconData      *data,
						     gconstpointer          client,
						     gboolean               large_text)
{
	NautilusFile *file;
	NautilusFileAttributes attributes;
		
	file = (NautilusFile *) data;

	g_assert (NAUTILUS_IS_FILE (file));

	attributes = NAUTILUS_FILE_ATTRIBUTE_TOP_LEFT_TEXT;
	if (large_text) {
		attributes |= NAUTILUS_FILE_ATTRIBUTE_LARGE_TOP_LEFT_TEXT;
	}
	nautilus_file_monitor_add (file, client, attributes);
}

static void
nautilus_canvas_view_container_stop_monitor_top_left (NautilusCanvasContainer *container,
						    NautilusCanvasIconData      *data,
						    gconstpointer          client)
{
	NautilusFile *file;

	file = (NautilusFile *) data;

	g_assert (NAUTILUS_IS_FILE (file));

	nautilus_file_monitor_remove (file, client);
}

static void
nautilus_canvas_view_container_prioritize_thumbnailing (NautilusCanvasContainer *container,
						      NautilusCanvasIconData      *data)
{
	NautilusFile *file;
	char *uri;

	file = (NautilusFile *) data;

	g_assert (NAUTILUS_IS_FILE (file));

	if (nautilus_file_is_thumbnailing (file)) {
		uri = nautilus_file_get_uri (file);
		nautilus_thumbnail_prioritize (uri);
		g_free (uri);
	}
}

static void
update_auto_strv_as_quarks (GSettings   *settings,
			    const gchar *key,
			    gpointer     user_data)
{
	GQuark **storage = user_data;
	int i = 0;
	char **value;

	value = g_settings_get_strv (settings, key);

	g_free (*storage);
	*storage = g_new (GQuark, g_strv_length (value) + 1);

	for (i = 0; value[i] != NULL; ++i) {
		(*storage)[i] = g_quark_from_string (value[i]);
	}
	(*storage)[i] = 0;

	g_strfreev (value);
}

/*
 * Get the preference for which caption text should appear
 * beneath icons.
 */
static GQuark *
nautilus_canvas_view_container_get_icon_text_attributes_from_preferences (void)
{
	static GQuark *attributes = NULL;

	if (attributes == NULL) {
		update_auto_strv_as_quarks (nautilus_icon_view_preferences, 
					    NAUTILUS_PREFERENCES_ICON_VIEW_CAPTIONS,
					    &attributes);
		g_signal_connect (nautilus_icon_view_preferences, 
				  "changed::" NAUTILUS_PREFERENCES_ICON_VIEW_CAPTIONS,
				  G_CALLBACK (update_auto_strv_as_quarks),
				  &attributes);
	}

	/* We don't need to sanity check the attributes list even though it came
	 * from preferences.
	 *
	 * There are 2 ways that the values in the list could be bad.
	 *
	 * 1) The user picks "bad" values.  "bad" values are those that result in
	 *    there being duplicate attributes in the list.
	 *
	 * 2) Value stored in GConf are tampered with.  Its possible physically do
	 *    this by pulling the rug underneath GConf and manually editing its
	 *    config files.  Its also possible to use a third party GConf key
	 *    editor and store garbage for the keys in question.
	 *
	 * Thankfully, the Nautilus preferences machinery deals with both of
	 * these cases.
	 *
	 * In the first case, the preferences dialog widgetry prevents
	 * duplicate attributes by making "bad" choices insensitive.
	 *
	 * In the second case, the preferences getter (and also the auto storage) for
	 * string_array values are always valid members of the enumeration associated
	 * with the preference.
	 *
	 * So, no more error checking on attributes is needed here and we can return
	 * a the auto stored value.
	 */
	return attributes;
}

static int
quarkv_length (GQuark *attributes)
{
	int i;
	i = 0;
	while (attributes[i] != 0) {
		i++;
	}
	return i;
}

/**
 * nautilus_canvas_view_get_icon_text_attribute_names:
 *
 * Get a list representing which text attributes should be displayed
 * beneath an icon. The result is dependent on zoom level and possibly
 * user configuration. Don't free the result.
 * @view: NautilusCanvasView to query.
 * 
 **/
static GQuark *
nautilus_canvas_view_container_get_icon_text_attribute_names (NautilusCanvasContainer *container,
							    int *len)
{
	GQuark *attributes;
	int piece_count;

	const int pieces_by_level[] = {
		0,	/* NAUTILUS_ZOOM_LEVEL_SMALLEST */
		0,	/* NAUTILUS_ZOOM_LEVEL_SMALLER */
		0,	/* NAUTILUS_ZOOM_LEVEL_SMALL */
		1,	/* NAUTILUS_ZOOM_LEVEL_STANDARD */
		2,	/* NAUTILUS_ZOOM_LEVEL_LARGE */
		2,	/* NAUTILUS_ZOOM_LEVEL_LARGER */
		3	/* NAUTILUS_ZOOM_LEVEL_LARGEST */
	};

	piece_count = pieces_by_level[nautilus_canvas_container_get_zoom_level (container)];

	attributes = nautilus_canvas_view_container_get_icon_text_attributes_from_preferences ();

	*len = MIN (piece_count, quarkv_length (attributes));

	return attributes;
}

/* This callback returns the text, both the editable part, and the
 * part below that is not editable.
 */
static void
nautilus_canvas_view_container_get_icon_text (NautilusCanvasContainer *container,
					    NautilusCanvasIconData      *data,
					    char                 **editable_text,
					    char                 **additional_text,
					    gboolean               include_invisible)
{
	GQuark *attributes;
	char *text_array[4];
	int i, j, num_attributes;
	NautilusCanvasView *canvas_view;
	NautilusFile *file;
	gboolean use_additional;

	file = NAUTILUS_FILE (data);

	g_assert (NAUTILUS_IS_FILE (file));
	g_assert (editable_text != NULL);
	canvas_view = get_canvas_view (container);
	g_return_if_fail (canvas_view != NULL);

	use_additional = (additional_text != NULL);

	/* In the smallest zoom mode, no text is drawn. */
	if (nautilus_canvas_container_get_zoom_level (container) == NAUTILUS_ZOOM_LEVEL_SMALLEST &&
            !include_invisible) {
		*editable_text = NULL;
	} else {
		/* Strip the suffix for nautilus object xml files. */
		*editable_text = nautilus_file_get_display_name (file);
	}

	if (!use_additional) {
		return;
	}

	if (NAUTILUS_IS_DESKTOP_ICON_FILE (file) ||
	    nautilus_file_is_nautilus_link (file)) {
		/* Don't show the normal extra information for desktop icons,
		 * or desktop files, it doesn't make sense. */
 		*additional_text = NULL;
		return;
	}

	/* Find out what attributes go below each icon. */
	attributes = nautilus_canvas_view_container_get_icon_text_attribute_names (container,
									   &num_attributes);

	/* Get the attributes. */
	j = 0;
	for (i = 0; i < num_attributes; ++i) {
		char *text;
		if (attributes[i] == attribute_none_q) {
			continue;
		}
		text = nautilus_file_get_string_attribute_q (file, attributes[i]);
		if (text == NULL) {
			continue;
		}
		text_array[j++] = text;
	}
	text_array[j] = NULL;

	/* Return them. */
	if (j == 0) {
		*additional_text = NULL;
	} else if (j == 1) {
		/* Only one item, avoid the strdup + free */
		*additional_text = text_array[0];
	} else {
		*additional_text = g_strjoinv ("\n", text_array);
		
		for (i = 0; i < j; i++) {
			g_free (text_array[i]);
		}
	}
}

/* Sort as follows:
 *   0) home link
 *   1) network link
 *   2) mount links
 *   3) other
 *   4) trash link
 */
typedef enum {
	SORT_HOME_LINK,
	SORT_NETWORK_LINK,
	SORT_MOUNT_LINK,
	SORT_OTHER,
	SORT_TRASH_LINK
} SortCategory;

static SortCategory
get_sort_category (NautilusFile *file)
{
	NautilusDesktopLink *link;
	SortCategory category;

	category = SORT_OTHER;
	
	if (NAUTILUS_IS_DESKTOP_ICON_FILE (file)) {
		link = nautilus_desktop_icon_file_get_link (NAUTILUS_DESKTOP_ICON_FILE (file));
		if (link != NULL) {
			switch (nautilus_desktop_link_get_link_type (link)) {
			case NAUTILUS_DESKTOP_LINK_HOME:
				category = SORT_HOME_LINK;
				break;
			case NAUTILUS_DESKTOP_LINK_MOUNT:
				category = SORT_MOUNT_LINK;
				break;
			case NAUTILUS_DESKTOP_LINK_TRASH:
				category = SORT_TRASH_LINK;
				break;
			case NAUTILUS_DESKTOP_LINK_NETWORK:
				category = SORT_NETWORK_LINK;
				break;
			default:
				category = SORT_OTHER;
				break;
			}
			g_object_unref (link);
		}
	} 
	
	return category;
}

static int
fm_desktop_canvas_container_icons_compare (NautilusCanvasContainer *container,
					 NautilusCanvasIconData      *data_a,
					 NautilusCanvasIconData      *data_b)
{
	NautilusFile *file_a;
	NautilusFile *file_b;
	NautilusView *directory_view;
	SortCategory category_a, category_b;

	file_a = (NautilusFile *) data_a;
	file_b = (NautilusFile *) data_b;

	directory_view = NAUTILUS_VIEW (NAUTILUS_CANVAS_VIEW_CONTAINER (container)->view);
	g_return_val_if_fail (directory_view != NULL, 0);
	
	category_a = get_sort_category (file_a);
	category_b = get_sort_category (file_b);

	if (category_a == category_b) {
		return nautilus_file_compare_for_sort 
			(file_a, file_b, NAUTILUS_FILE_SORT_BY_DISPLAY_NAME, 
			 nautilus_view_should_sort_directories_first (directory_view),
			 FALSE);
	}

	if (category_a < category_b) {
		return -1;
	} else {
		return +1;
	}
}

static int
nautilus_canvas_view_container_compare_icons (NautilusCanvasContainer *container,
					    NautilusCanvasIconData      *icon_a,
					    NautilusCanvasIconData      *icon_b)
{
	NautilusCanvasView *canvas_view;

	canvas_view = get_canvas_view (container);
	g_return_val_if_fail (canvas_view != NULL, 0);

	if (NAUTILUS_CANVAS_VIEW_CONTAINER (container)->sort_for_desktop) {
		return fm_desktop_canvas_container_icons_compare
			(container, icon_a, icon_b);
	}

	/* Type unsafe comparisons for performance */
	return nautilus_canvas_view_compare_files (canvas_view,
					   (NautilusFile *)icon_a,
					   (NautilusFile *)icon_b);
}

static int
nautilus_canvas_view_container_compare_icons_by_name (NautilusCanvasContainer *container,
						    NautilusCanvasIconData      *icon_a,
						    NautilusCanvasIconData      *icon_b)
{
	return nautilus_file_compare_for_sort
		(NAUTILUS_FILE (icon_a),
		 NAUTILUS_FILE (icon_b),
		 NAUTILUS_FILE_SORT_BY_DISPLAY_NAME,
		 FALSE, FALSE);
}

static void
nautilus_canvas_view_container_freeze_updates (NautilusCanvasContainer *container)
{
	NautilusCanvasView *canvas_view;
	canvas_view = get_canvas_view (container);
	g_return_if_fail (canvas_view != NULL);
	nautilus_view_freeze_updates (NAUTILUS_VIEW (canvas_view));
}

static void
nautilus_canvas_view_container_unfreeze_updates (NautilusCanvasContainer *container)
{
	NautilusCanvasView *canvas_view;
	canvas_view = get_canvas_view (container);
	g_return_if_fail (canvas_view != NULL);
	nautilus_view_unfreeze_updates (NAUTILUS_VIEW (canvas_view));
}

static void
nautilus_canvas_view_container_class_init (NautilusCanvasViewContainerClass *klass)
{
	NautilusCanvasContainerClass *ic_class;

	ic_class = &klass->parent_class;

	attribute_none_q = g_quark_from_static_string ("none");
	
	ic_class->get_icon_text = nautilus_canvas_view_container_get_icon_text;
	ic_class->get_icon_images = nautilus_canvas_view_container_get_icon_images;
	ic_class->get_icon_description = nautilus_canvas_view_container_get_icon_description;
	ic_class->start_monitor_top_left = nautilus_canvas_view_container_start_monitor_top_left;
	ic_class->stop_monitor_top_left = nautilus_canvas_view_container_stop_monitor_top_left;
	ic_class->prioritize_thumbnailing = nautilus_canvas_view_container_prioritize_thumbnailing;

	ic_class->compare_icons = nautilus_canvas_view_container_compare_icons;
	ic_class->compare_icons_by_name = nautilus_canvas_view_container_compare_icons_by_name;
	ic_class->freeze_updates = nautilus_canvas_view_container_freeze_updates;
	ic_class->unfreeze_updates = nautilus_canvas_view_container_unfreeze_updates;
}

static void
nautilus_canvas_view_container_init (NautilusCanvasViewContainer *canvas_container)
{
	gtk_style_context_add_class (gtk_widget_get_style_context (GTK_WIDGET (canvas_container)),
				     GTK_STYLE_CLASS_VIEW);

}

NautilusCanvasContainer *
nautilus_canvas_view_container_construct (NautilusCanvasViewContainer *canvas_container, NautilusCanvasView *view)
{
	AtkObject *atk_obj;

	g_return_val_if_fail (NAUTILUS_IS_CANVAS_VIEW (view), NULL);

	canvas_container->view = view;
	atk_obj = gtk_widget_get_accessible (GTK_WIDGET (canvas_container));
	atk_object_set_name (atk_obj, _("Icon View"));

	return NAUTILUS_CANVAS_CONTAINER (canvas_container);
}

NautilusCanvasContainer *
nautilus_canvas_view_container_new (NautilusCanvasView *view)
{
	return nautilus_canvas_view_container_construct
		(g_object_new (NAUTILUS_TYPE_CANVAS_VIEW_CONTAINER, NULL),
		 view);
}

void
nautilus_canvas_view_container_set_sort_desktop (NautilusCanvasViewContainer *container,
					       gboolean         desktop)
{
	container->sort_for_desktop = desktop;
}
