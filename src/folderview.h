/*
 * Claws Mail -- a GTK based, lightweight, and fast e-mail client
 * Copyright (C) 1999-2012 Hiroyuki Yamamoto and the Claws Mail team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef __FOLDERVIEW_H__
#define __FOLDERVIEW_H__

#include <glib.h>
#include <gtk/gtk.h>

#include "mainwindow.h"
#include "viewtypes.h"
#include "folder.h"

typedef enum
{
	F_COL_FOLDERITEM,
	F_COL_FONT,
	F_COL_COLOR,
	F_COL_ICON,
	F_COL_FOLDER,
	F_COL_NEW,
	F_COL_UNREAD,
	F_COL_TOTAL,
	F_COL_ALIGN,
	N_FOLDER_COLS
} FolderColumnType;

struct _FolderView
{
	GtkWidget *scrolledwin;
	GtkWidget *treeview;
	GtkWidget *headerpopupmenu;

	MainWindow *mainwin;
	SummaryView *summaryview;

	GdkRGBA color_new;
	GdkRGBA color_op;

	PangoFontDescription *normal_font;
	PangoFontDescription *bold_font;

	FolderItem *open_item; /* Pointer to currently open item */
	FolderItem *menu_item; /* Pointer to item the popup menu is open for */
	GtkTreePath *open_path; /* Pointer to currently open item */

	gboolean data_row_clicked; /* helper for saving column-widths */

	GHashTable *popups;

	/* DND states */
	GSList *paths_to_recollapse;
	guint   drag_timer_id;		/* timer id */
	FolderItem *drag_dst_item;	/* dragged item (destination) */
	GtkTreePath *drag_dst_path;	/* drag path (destination) */

	gint scroll_value;
	guint scroll_timeout_id;
};

typedef struct _FolderViewPopup FolderViewPopup;
typedef struct _FolderColumnState FolderColumnState;

struct _FolderViewPopup
{
	gchar			 *klass;
	gchar			 *path;
	GtkActionEntry		 *entries;
	gint		 	  n_entries;
	GtkToggleActionEntry	 *toggle_entries;
	gint		 	  n_toggle_entries;
	GtkRadioActionEntry	 *radio_entries;
	gint		  	  n_radio_entries;
	gint			  radio_default;
	void		  	(*radio_callback)	(GtkAction *action, GtkRadioAction *current, gpointer data);
	void			(*add_menuitems)	(GtkUIManager *ui_manager, FolderItem *item);
	void			(*set_sensitivity)	(GtkUIManager *ui_manager, FolderItem *item);
};

struct _FolderColumnState
{
	FolderColumnType type;
	gboolean visible;
};

void folderview_initialize		(void);
FolderView *folderview_create		(MainWindow *mainwin);
void folderview_init			(FolderView	*folderview);

void folderview_set			(FolderView	*folderview);
void folderview_set_all			(void);

void folderview_select			(FolderView	*folderview,
					 FolderItem	*item);
void folderview_unselect		(FolderView	*folderview);
void folderview_select_next_with_flag	(FolderView	*folderview,
					 MsgPermFlags    flag);

FolderItem *folderview_get_selected_item	(FolderView	*folderview);
FolderItem *folderview_get_opened_item		(FolderView	*folderview);

void folderview_rescan_tree		(Folder		*folder,
					 gboolean	 rebuild);
gint folderview_check_new		(Folder		*folder);
void folderview_check_new_all		(void);

void folderview_run_processing(FolderItem *item);

void folderview_move_folder		(FolderView 	*folderview,
					 FolderItem 	*from_folder,
					 FolderItem 	*to_folder,
					 gboolean	 copy);

void folderview_set_target_folder_color (GdkRGBA	color_op);

void folderview_reinit_fonts		(FolderView *folderview);

void folderview_reflect_prefs		(FolderView 		*folderview);
void folderview_register_popup		(FolderViewPopup	*fpopup);
void folderview_unregister_popup	(FolderViewPopup	*fpopup);
void folderview_update_search_icon	(FolderItem 		*item, 	
					 gboolean 		 matches);
void folderview_set_column_order	(FolderView		*folderview);
void folderview_finish_dnd		(const gchar 		*data, 
					 GdkDragContext 	*drag_context,
			   		 guint 			 time, 
					 FolderItem 		*item);
void folderview_close_opened		(FolderView 		*folderview,
					 gboolean		 dirty);
void folderview_remove_item		(FolderView *folderview,
					 FolderItem *item);

void folderview_freeze			(FolderView *folderview);
void folderview_thaw			(FolderView *folderview);
void folderview_grab_focus		(FolderView *folderview);

gboolean folderview_is_drag_source	(FolderView *folderview,
					 GdkDragContext *context);

void folderview_update_row		(FolderView *folderview,
					 FolderItem *item);
#endif /* __FOLDERVIEW_H__ */
