/*
 * Claws Mail -- a GTK+ based, lightweight, and fast e-mail client
 * Copyright (C) 1999-2022 the Claws Mail team and Hiroyuki Yamamoto
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
 */

#include "config.h"
#include "defs.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "account.h"
#include "alertpanel.h"
#include "folder_item_prefs.h"
#include "folderutils.h"
#include "folderview.h"
#include "inc.h"
#include "manual.h"
#include "partial_download.h"
#include "prefs_common.h"
#include "prefs_folder_item.h"
#include "prefs_folder_column.h"
#include "statusbar.h"
#include "stock_pixmap.h"
#include "summary_search.h"

#include "common/defs.h"
#include "common/hooks.h"
#include "common/timing.h"
#include "common/utils.h"

#include "gtk/menu.h"

static GList *folderview_list = NULL;

static GdkPixbuf *inboxxpm;
static GdkPixbuf *inboxhrmxpm;
static GdkPixbuf *inboxopenxpm;
static GdkPixbuf *inboxopenhrmxpm;
static GdkPixbuf *outboxxpm;
static GdkPixbuf *outboxhrmxpm;
static GdkPixbuf *outboxopenxpm;
static GdkPixbuf *outboxopenhrmxpm;
static GdkPixbuf *folderxpm;
static GdkPixbuf *folderhrmxpm;
static GdkPixbuf *folderopenxpm;
static GdkPixbuf *folderopenhrmxpm;
static GdkPixbuf *trashopenxpm;
static GdkPixbuf *trashopenhrmxpm;
static GdkPixbuf *trashxpm;
static GdkPixbuf *trashhrmxpm;
static GdkPixbuf *queuexpm;
static GdkPixbuf *queuehrmxpm;
static GdkPixbuf *queueopenxpm;
static GdkPixbuf *queueopenhrmxpm;
static GdkPixbuf *draftsxpm;
static GdkPixbuf *draftsopenxpm;
static GdkPixbuf *foldersubsxpm;
static GdkPixbuf *foldersubsopenxpm;
static GdkPixbuf *foldernoselectxpm;
static GdkPixbuf *foldernoselectopenxpm;

static GdkPixbuf *m_inboxxpm;
static GdkPixbuf *m_inboxhrmxpm;
static GdkPixbuf *m_inboxopenxpm;
static GdkPixbuf *m_inboxopenhrmxpm;
static GdkPixbuf *m_outboxxpm;
static GdkPixbuf *m_outboxhrmxpm;
static GdkPixbuf *m_outboxopenxpm;
static GdkPixbuf *m_outboxopenhrmxpm;
static GdkPixbuf *m_folderxpm;
static GdkPixbuf *m_folderhrmxpm;
static GdkPixbuf *m_folderopenxpm;
static GdkPixbuf *m_folderopenhrmxpm;
static GdkPixbuf *m_trashopenxpm;
static GdkPixbuf *m_trashopenhrmxpm;
static GdkPixbuf *m_trashxpm;
static GdkPixbuf *m_trashhrmxpm;
static GdkPixbuf *m_queuexpm;
static GdkPixbuf *m_queuehrmxpm;
static GdkPixbuf *m_queueopenxpm;
static GdkPixbuf *m_queueopenhrmxpm;
static GdkPixbuf *m_draftsxpm;
static GdkPixbuf *m_draftsopenxpm;
static GdkPixbuf *m_foldersubsxpm;
static GdkPixbuf *m_foldernoselectxpm;

static GdkPixbuf *searchicon;

static gboolean folder_has_children		(FolderItem *item);
static gboolean folder_has_new_children		(FolderItem *item);
static gboolean folder_has_unread_children	(FolderItem *item);
static gboolean folder_has_read_children	(FolderItem *item);
static gboolean folder_has_matching_children	(FolderItem *item);
static gboolean folder_has_marked_children	(FolderItem *item);

static gboolean folderview_open			(FolderView *folderview,
						 FolderItem *item);

static void folderview_add_folderitem		(FolderView *folderview,
						 FolderItem *item);
static void folderview_remove_folderitem	(FolderView *folderview,
						 FolderItem *item);

static void set_sens_and_popup_menu		(FolderView *folderview,
						 FolderItem *item,
						 GdkEventButton *event);
static void mark_all_read_cb			(GtkAction *action,
						 gpointer data);
static void mark_all_unread_cb			(GtkAction *action,
						 gpointer data);
static void mark_all_read_recursive_cb		(GtkAction *action,
						 gpointer data);
static void mark_all_unread_recursive_cb	(GtkAction *action,
						 gpointer data);
static void folderview_run_processing_cb	(GtkAction *action,
						 gpointer data);
static void folderview_search_cb		(GtkAction *action,
						 gpointer data);
static void folderview_property_cb		(GtkAction *action,
						 gpointer data);
static void folderview_processing_cb		(GtkAction *action,
						 gpointer data);
static void folderview_empty_trash_cb		(GtkAction *action,
						 gpointer data);
static void folderview_send_queue_cb		(GtkAction *action,
						 gpointer data);
static void folderview_header_set_displayed_columns_cb	(GtkAction *gaction,
							 gpointer data);
static gboolean folderview_header_button_pressed	(GtkWidget *widget,
							 GdkEvent *orgevent,
							 gpointer user_data);
static void row_activated_cb			(GtkTreeView *treeview,
						 GtkTreePath *path,
						 GtkTreeViewColumn *column,
						 gpointer user_data);


#ifndef GENERIC_UMPC
static void folderview_drag_begin	 (GtkWidget *widget,
					  GdkDragContext *context,
					  FolderView *folderview);
#endif
static void folderview_drag_data_get     (GtkWidget        *widget,
					  GdkDragContext   *drag_context,
					  GtkSelectionData *selection_data,
					  guint             info,
					  guint             time,
					  FolderView       *folderview);
static gboolean folderview_drag_motion_cb(GtkWidget      *widget,
					  GdkDragContext *context,
					  gint            x,
					  gint            y,
					  guint           time,
					  FolderView     *folderview);
static void folderview_drag_leave_cb     (GtkWidget        *widget,
					  GdkDragContext   *context,
					  guint             time,
					  FolderView       *folderview);
static void folderview_drag_received_cb  (GtkWidget        *widget,
					  GdkDragContext   *drag_context,
					  gint              x,
					  gint              y,
					  GtkSelectionData *data,
					  guint             info,
					  guint             time,
					  FolderView       *folderview);
static void folderview_drag_end_cb	 (GtkWidget	   *widget,
					  GdkDragContext   *drag_context,
					  FolderView	   *folderview);

static void folderview_toggle_expansion	(GtkTreeView *treeview);

GHashTable *folderview_popups;

GList *treeview_default_columns_ordered;

static void create_action_groups		(gpointer key,
						 gpointer value,
						 gpointer data);

static GtkActionEntry folderview_common_popup_entries[] = 
{
	{"FolderViewPopup",                  NULL, "FolderViewPopup", NULL, NULL , NULL},
	{"FolderViewPopup/MarkAllRead",      NULL, N_("Mark all re_ad"), NULL, NULL, G_CALLBACK(mark_all_read_cb) },
	{"FolderViewPopup/MarkAllUnread",    NULL, N_("Mark all u_nread"), NULL, NULL, G_CALLBACK(mark_all_unread_cb) },
	{"FolderViewPopup/MarkAllReadRec",   NULL, N_("Mark all read recursi_vely"), NULL, NULL, G_CALLBACK(mark_all_read_recursive_cb) },
	{"FolderViewPopup/MarkAllUnreadRec", NULL, N_("Mark all unread recursi_vely"), NULL, NULL, G_CALLBACK(mark_all_unread_recursive_cb) },
	{"FolderViewPopup/---",              NULL, "---", NULL, NULL , NULL},
	{"FolderViewPopup/RunProcessing",    NULL, N_("R_un processing rules"), NULL, NULL, G_CALLBACK(folderview_run_processing_cb) },
	{"FolderViewPopup/SearchFolder",     NULL, N_("_Search folder..."), NULL, NULL, G_CALLBACK(folderview_search_cb) },
	{"FolderViewPopup/Properties",       NULL, N_("_Properties..."), NULL, NULL, G_CALLBACK(folderview_property_cb) },
	{"FolderViewPopup/Processing",       NULL, N_("Process_ing..."), NULL, NULL, G_CALLBACK(folderview_processing_cb) },
	{"FolderViewPopup/EmptyTrash",       NULL, N_("Empty _trash..."), NULL, NULL, G_CALLBACK(folderview_empty_trash_cb) },
	{"FolderViewPopup/SendQueue",        NULL, N_("Send _queue..."), NULL, NULL, G_CALLBACK(folderview_send_queue_cb) },
};

static GtkActionEntry folderview_header_popup_entries[] =
{
	{"FolderViewHeaderPopup",                     NULL, "FolderViewHeaderPopup", NULL, NULL, NULL },
	{"FolderViewHeaderPopup/SetDisplayedColumns", NULL, N_("Set Displayed columns"), NULL, NULL, G_CALLBACK(folderview_header_set_displayed_columns_cb) }
};

GtkTargetEntry folderview_drag_types[] =
{
	{"claws-mail/internal", GTK_TARGET_SAME_APP, TARGET_DUMMY},
	{"text/uri-list", 0, TARGET_MAIL_URI_LIST}
};

static void folderview_update_row_iter(FolderView *folderview,
				       GtkTreeIter *iter, FolderItem *item);

/* Sort function for the treeview, making sure that all special
 * folders are on top. */
static gint folderview_compare_func(GtkTreeModel *model, GtkTreeIter *a,
				    GtkTreeIter *b, gpointer context)
{
	gchar *str_a = NULL, *str_b = NULL;
#ifdef G_OS_WIN32
	gchar *str_a_dn = NULL, *str_b_dn = NULL;
#endif /* G_OS_WIN32 */
	gint val = 0;
	FolderItem *item_a = NULL, *item_b = NULL;
	GtkTreeIter parent;

	gtk_tree_model_get(model, a, F_COL_FOLDERITEM, &item_a, -1);
	gtk_tree_model_get(model, b, F_COL_FOLDERITEM, &item_b, -1);

	/* no sort for root folder */
	if (!gtk_tree_model_iter_parent(GTK_TREE_MODEL(model), &parent, a))
		return 0;

	/* if both a and b are special folders, sort them according to
	 * their types (which is in-order). Note that this assumes that
	 * there are no multiple folders of a special type. */
	if (item_a->stype != F_NORMAL && item_b->stype != F_NORMAL)
		return item_a->stype - item_b->stype;

	/* if b is normal folder, and a is not, b is smaller (ends up
	 * lower in the list) */
	if (item_a->stype != F_NORMAL && item_b->stype == F_NORMAL)
		return item_b->stype - item_a->stype;

	/* if b is special folder, and a is not, b is larger (ends up
	 * higher in the list) */
	if (item_a->stype == F_NORMAL && item_b->stype != F_NORMAL)
		return item_b->stype - item_a->stype;

	/* XXX g_utf8_collate_key() comparisons may speed things
	 * up when having large lists of folders */
	gtk_tree_model_get(model, a, F_COL_FOLDER, &str_a, -1);
	gtk_tree_model_get(model, b, F_COL_FOLDER, &str_b, -1);

	/* otherwise just compare the folder names */
#ifdef G_OS_WIN32
	str_a_dn = g_utf8_casefold(str_a, -1);
	str_b_dn = g_utf8_casefold(str_b, -1);
	g_free(str_a);
	g_free(str_b);
	str_a = str_a_dn;
	str_b = str_b_dn;
#endif /* G_OS_WIN32 */
	val = g_utf8_collate(str_a, str_b);

	g_free(str_a);
	g_free(str_b);

	return val;
}

/****************************
 * Treeview signal handlers *
 ****************************/
static gboolean button_press_cb(GtkWidget *widget, GdkEventButton *event,
				gpointer user_data)
{
	FolderView *folderview = (FolderView *)user_data;
	GtkTreePath *path;
	GtkTreeViewColumn *column;
	GtkTreeIter iter;
	gboolean ret;
	FolderItem *item;
	gboolean is_expander;

	debug_print("button-press: button %d\n", event->button);

	if (event->button == 2) {
		/* Middle-click does nothing */
		return TRUE;
	}

	ret = gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget),
			event->x, event->y,
			&path, &column, NULL, NULL);
	if (!ret) {
		debug_print("clicked empty space, ignoring\n");
		gtk_tree_path_free(path);
		return FALSE;
	}

	folderview->data_row_clicked = TRUE; /* don't save column-widths on button-release */
	is_expander = gtkut_tree_is_expander_clicked(GTK_TREE_VIEW(widget),
			path, column, event->x, event->y);

	if (event->button == 1) { /* Left click */
		gtk_tree_path_free(path);
		if (is_expander)
			return FALSE;

		if (event->type == GDK_2BUTTON_PRESS) {
			folderview_toggle_expansion(GTK_TREE_VIEW(widget));
			return FALSE;
		}
	} else if (event->button == 3) { /* Right click */
		debug_print("will show popup menu\n");

		gtk_tree_view_set_cursor(GTK_TREE_VIEW(widget), path, NULL, FALSE);
		/* Find out which item the click was on */
		GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
		gtk_tree_model_get_iter(model, &iter, path);
		gtk_tree_path_free(path);
		gtk_tree_model_get(model, &iter,
				F_COL_FOLDERITEM, &item, -1);

		cm_return_val_if_fail(item != NULL, FALSE);

		set_sens_and_popup_menu(folderview, item, event);

		/* Do not even allow the cursor to move */
		return TRUE;
	}

	return FALSE;
}

static gboolean button_release_cb(GtkWidget *widget, GdkEventButton *event,
				  gpointer user_data)
{
	FolderView *folderview = (FolderView *)user_data;
	gboolean data_row_clicked = folderview->data_row_clicked;
	GtkTreePath *path;
	GtkTreeViewColumn *column;
	gint i;

	/* TM: Workaround for missing "resize_column" signal
	 * Opposed to button-press, the button-release signal is fired for
	 * headers as well, but event->y gets the SAME values, when clicked on
	 * the headers, as if clicked on the first data row. To distiguish
	 * them, data_row_clicked is set in the button-press handler.
	 */
	folderview->data_row_clicked = FALSE;
	/* ignore clicks on and below data rows */
	if (data_row_clicked || !gtk_tree_view_get_path_at_pos
			(GTK_TREE_VIEW(widget),
			 event->x, event->y, &path, NULL, NULL, NULL))
		return FALSE;

	/* button released on header row: save column widths */
	for (i = F_COL_FOLDER; i <= F_COL_TOTAL; i++) {
		column = (GtkTreeViewColumn*)g_list_nth_data
				(treeview_default_columns_ordered,
				 i - F_COL_FOLDER);
		prefs_common.folder_col_size[i] =
			 gtk_tree_view_column_get_width(column);
	}
	return FALSE;
}


static void folderview_toggle_expansion(GtkTreeView *treeview)
{
	GtkTreePath *path;

	gtk_tree_view_get_cursor(treeview, &path, NULL);
	if (path) {
		if (gtk_tree_view_row_expanded(treeview, path))
			gtk_tree_view_collapse_row(treeview, path);
		else
			gtk_tree_view_expand_row(treeview,path, FALSE);
		gtk_tree_path_free(path);
	}
}

void folderview_open_or_expand_selected(FolderView *folderview)
{
	GtkTreeView *treeview = GTK_TREE_VIEW(folderview->treeview);
	GtkTreePath *path;
	GtkTreeViewColumn *column;
	GtkTreeModel *model;
	GtkTreeIter iter;

	gtk_tree_view_get_cursor(treeview, &path, &column);

	model = gtk_tree_view_get_model(treeview);
	if (!gtk_tree_model_get_iter(model, &iter, path)) {
		gtk_tree_path_free(path);
		return;
	}

	if (gtk_tree_model_iter_has_child(model, &iter)
		&& !gtk_tree_view_row_expanded(treeview, path))
			gtk_tree_view_expand_row(treeview, path, FALSE);
	else
		row_activated_cb(treeview, path, column, (gpointer)folderview);

	gtk_tree_path_free(path);
}

void folderview_collape_or_open_parent(FolderView *folderview)
{
	GtkTreeView *treeview = GTK_TREE_VIEW(folderview->treeview);
	GtkTreePath *path, *parent_path;

	gtk_tree_view_get_cursor(treeview, &path, NULL);

	/* if expanded: collapse current row */
	if (gtk_tree_view_row_expanded(treeview, path)) {
		gtk_tree_view_collapse_row(treeview, path);
		gtk_tree_path_free(path);
		return;
	}

	/* otherwise: open parent */
	parent_path = gtkut_tree_path_get_parent(treeview, path);
	if (parent_path) {
		row_activated_cb(treeview, parent_path, NULL, (gpointer)folderview);
		gtk_tree_path_free(parent_path);
		gtk_tree_path_free(path);
		return;
	}

}

static gboolean folderview_key_pressed(GtkWidget *widget, GdkEventKey *event,
				       FolderView *folderview)
{
	if (!event) return FALSE;

	if (quicksearch_has_focus(folderview->summaryview->quicksearch))
		return FALSE;

	switch (event->keyval) {
	case GDK_KEY_Right:
		/* expand, if collapsed. otherwise open selected folder */
		folderview_open_or_expand_selected(folderview);
		break;
	case GDK_KEY_Left:
		/* collapse, if expanded. otherwise open parent folder */
		folderview_collape_or_open_parent(folderview);
		break;
	default:
		break;
	}

	return FALSE;
}

static gboolean cursor_changed_cb(GtkTreeView *treeview, gpointer user_data)
{
	FolderView *folderview = (FolderView *)user_data;

	debug_print("cursor-changed\n");

	/* Only do something if some other signal handler has indicated that
	 * a selected folder should be opened. */

	if (folderview->open_item != NULL)
		folderview_unselect(folderview);

	return FALSE;
}

static void row_expanded_collapsed_cb(GtkTreeView *treeview, GtkTreeIter *iter,
				      GtkTreePath *path, gpointer user_data)
{
	FolderView *folderview = (FolderView *)user_data;
	gboolean expanded = gtk_tree_view_row_expanded(treeview, path);
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);
	FolderItem *item = NULL;
	GtkTreeIter child;
	GtkTreePath *cpath;

	debug_print("row-expanded/collapsed\n");

	gtk_tree_model_get(model, iter,
			F_COL_FOLDERITEM, &item, -1);

	if (item == NULL)
		return;

	/* Remember if the folderitem is expanded or not, as it gets saved
	 * into folderlist.xml. */
	item->collapsed = !expanded;
	debug_print("%s folder '%s'\n",
			(expanded ? ">>> expanding" : "<<< collapsing"), item->path);

	folderview_update_row_iter(folderview, iter, item);

	/* GtkTreeView does not remember expanded state of child rows
	 * after their parent row gets collapsed, so we need to reexpand
	 * all children as needed. */
	if (expanded && gtk_tree_model_iter_children(model, &child, iter)) {
		do {
			gtk_tree_model_get(model, &child,
					F_COL_FOLDERITEM, &item, -1);
			if (gtk_tree_model_iter_has_child(model, &child) &&
					item != NULL && !item->collapsed) {
				cpath = gtk_tree_model_get_path(model, &child);
				gtk_tree_view_expand_row(treeview, cpath, FALSE);
				gtk_tree_path_free(cpath);
			}
		} while (gtk_tree_model_iter_next(model, &child));
	}
}

static void row_activated_cb(GtkTreeView *treeview, GtkTreePath *path,
			     GtkTreeViewColumn *column, gpointer user_data)
{
	FolderView *folderview = (FolderView *)user_data;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);
	FolderItem *item;

	debug_print("row-activated\n");

	if (folderview->open_path)
		gtk_tree_path_free(folderview->open_path);
	folderview->open_path = gtk_tree_path_copy(path);
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get(model, &iter,
			F_COL_FOLDERITEM, &item, -1);

	folderview_open(folderview, item);
	folderview_update_row(folderview, item);

	/* CLAWS: set compose button type: news folder items
	 * always have a news folder as parent */
	if (item->folder)
		toolbar_set_compose_button
			(folderview->mainwin->toolbar,
			 FOLDER_TYPE(item->folder) == F_NEWS ?
			 COMPOSEBUTTON_NEWS : COMPOSEBUTTON_MAIL);

	if (item->path)
		debug_print("Folder %s is selected\n", item->path);

	/* TM: Why expand, if there are no children?
	if (!GTK_CMCTREE_ROW(row)->children)
		gtk_cmctree_expand(ctree, row);
	*/

	/* ungrab the mouse event */
	/* TM: disabled - grab_add was done by ctree
	if (gtk_widget_has_grab(GTK_WIDGET(treeview))) {
		gtk_grab_remove(GTK_WIDGET(treeview));
		if (gdk_pointer_is_grabbed())
			gdk_pointer_ungrab(GDK_CURRENT_TIME);
	} */
}

/***********************************
 * End of treeview signal handlers *
 ***********************************/

static gboolean hook_folder_item_update(gpointer source,
					gpointer data)
{
	FolderItemUpdateData *update_info = (FolderItemUpdateData *)source;
	FolderView *folderview = (FolderView *)data;

	debug_print("hook_folder_item_update\n");

	cm_return_val_if_fail(update_info != NULL, TRUE);
	cm_return_val_if_fail(update_info->item != NULL, TRUE);
	cm_return_val_if_fail(folderview != NULL, FALSE);

	if (update_info->update_flags & (F_ITEM_UPDATE_MSGCNT | F_ITEM_UPDATE_NAME)) {
		folderview_update_row(folderview, update_info->item);
	}

	if ((update_info->update_flags & F_ITEM_UPDATE_CONTENT) &&
			update_info->item == folderview->summaryview->folder_item) {
		if (!quicksearch_has_sat_predicate(folderview->summaryview->quicksearch))
			summary_show(folderview->summaryview, update_info->item, FALSE);
	}

	return FALSE;
}

static gboolean hook_folder_update(gpointer source,
				   gpointer data)
{
	FolderUpdateData *hookdata = (FolderUpdateData *)source;
	FolderView *folderview = (FolderView *)data;

	debug_print("hook_folder_update\n");

	cm_return_val_if_fail(hookdata != NULL, FALSE);
	cm_return_val_if_fail(folderview != NULL, FALSE);

	if (hookdata->update_flags & FOLDER_ADD_FOLDERITEM) {
		debug_print("hook_folder_update ADD FOLDERITEM '%s'\n", hookdata->item->name);
		folderview_add_folderitem(folderview, hookdata->item);
	} else if (hookdata->update_flags & FOLDER_RENAME_FOLDERITEM) {
		debug_print("hook_folder_update RENAME FOLDERITEM '%s'\n", hookdata->item->name);
		folderview_update_row(folderview, hookdata->item);
	} else if (hookdata->update_flags & FOLDER_REMOVE_FOLDERITEM) {
		debug_print("hook_folder_update REMOVE FOLDERITEM '%s'\n", hookdata->item->name);
		if (hookdata->item == folderview->open_item) {
			debug_print("closing open item before removing it\n");
			folderview_close_opened(folderview, FALSE);
		}
		folderview_remove_folderitem(folderview, hookdata->item);
	} else if (hookdata->update_flags & FOLDER_MOVE_FOLDERITEM) {
		debug_print("hook_folder_update MOVE FOLDERITEM '%s'\n", hookdata->item->name);
		/* do nothing, it's done by the ADD and REMOVE) */
	} else if (hookdata->update_flags &
			(FOLDER_TREE_CHANGED | FOLDER_ADD_FOLDER | FOLDER_REMOVE_FOLDER)) {
		debug_print("hook_folder_update OTHER\n");
		folderview_set(folderview);
	}

	return FALSE;
}

/* Helper function for folderview_create()
 * Sets up the treeview widget, minus connecting the signal handlers. */
static GtkWidget *treeview_create_unconnected()
{
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer, *text_renderer;
	GtkWidget *image;
	GdkPixbuf *new_pixbuf;
	GdkPixbuf *unread_pixbuf = NULL;
	GdkPixbuf *total_pixbuf = NULL;

	debug_print("treeview_create_unconnected\n");

	stock_pixbuf_gdk(STOCK_PIXMAP_NEW, &new_pixbuf);
	stock_pixbuf_gdk(STOCK_PIXMAP_UNREAD, &unread_pixbuf);
	stock_pixbuf_gdk(STOCK_PIXMAP_READ, &total_pixbuf);

	GtkTreeStore *store = gtk_tree_store_new(N_FOLDER_COLS,
			G_TYPE_POINTER, /* pointer to FolderItem */
			PANGO_TYPE_FONT_DESCRIPTION, /* font */
			GDK_TYPE_RGBA, /* text color */
			GDK_TYPE_PIXBUF, /* icon */
			G_TYPE_STRING, /* folder name */
			G_TYPE_STRING, /* new */
			G_TYPE_STRING, /* unread */
			G_TYPE_STRING, /* total */
			PANGO_TYPE_ALIGNMENT); /* alignment of the column text */
	gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(store),
			F_COL_FOLDER, folderview_compare_func, NULL, NULL);

	GtkWidget *treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);

	gtk_widget_set_name(GTK_WIDGET(treeview), "folderview_treeview");

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), TRUE);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(treeview),
			prefs_common.use_stripes_in_summaries);
	gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(treeview), TRUE);
	gtk_tree_view_set_search_column(GTK_TREE_VIEW(treeview), F_COL_FOLDER);

	/* Icon */
	column = gtk_tree_view_column_new();
	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_cell_renderer_set_padding(renderer, 0, 0);
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_attributes(column, renderer,
			"pixbuf", F_COL_ICON,
			NULL);

	/* Folder name */
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_renderer_set_padding(renderer, 4, 0);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
			"text", F_COL_FOLDER,
			"font-desc", F_COL_FONT,
			"foreground-rgba", F_COL_COLOR,
			NULL);
	gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_column_set_fixed_width(column, prefs_common.folder_col_size[F_COL_FOLDER]);
	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_column_set_expand(column, TRUE);
	gtk_tree_view_column_set_title(column, _("Folder"));

	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	treeview_default_columns_ordered = g_list_append
			(treeview_default_columns_ordered, column);

	/* New */
	column = gtk_tree_view_column_new();
	renderer = text_renderer = gtk_cell_renderer_text_new();
	gtk_cell_renderer_set_padding(renderer, 0, 0);
	gtk_cell_renderer_set_alignment(renderer, 1, 0);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
			"text", F_COL_NEW,
			"font-desc", F_COL_FONT,
			"foreground-rgba", F_COL_COLOR,
			NULL);
	gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_column_set_fixed_width(column, prefs_common.folder_col_size[F_COL_NEW]);
	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_column_set_expand(column, FALSE);
	image = gtk_image_new_from_pixbuf(new_pixbuf);
	gtk_widget_show(image);
	gtk_tree_view_column_set_widget(column, image);

	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	treeview_default_columns_ordered = g_list_append
			(treeview_default_columns_ordered, column);

	/* Unread */
	column = gtk_tree_view_column_new();
	renderer = text_renderer;
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
			"text", F_COL_UNREAD,
			"font-desc", F_COL_FONT,
			"foreground-rgba", F_COL_COLOR,
			NULL);
	gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_column_set_fixed_width(column, prefs_common.folder_col_size[F_COL_UNREAD]);
	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_column_set_expand(column, FALSE);
	image = gtk_image_new_from_pixbuf(unread_pixbuf);
	gtk_widget_show(image);
	gtk_tree_view_column_set_widget(column, image);

	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	treeview_default_columns_ordered = g_list_append
			(treeview_default_columns_ordered, column);

	/* Total */
	column = gtk_tree_view_column_new();
	renderer = text_renderer;
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
			"text", F_COL_TOTAL,
			"font-desc", F_COL_FONT,
			"foreground-rgba", F_COL_COLOR,
			NULL);
	gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	gtk_tree_view_column_set_fixed_width(column, prefs_common.folder_col_size[F_COL_TOTAL]);
	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_column_set_expand(column, FALSE);
	image = gtk_image_new_from_pixbuf(total_pixbuf);
	gtk_widget_show(image);
	gtk_tree_view_column_set_widget(column, image);

	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	treeview_default_columns_ordered = g_list_append
			(treeview_default_columns_ordered, column);

	return treeview;
}

FolderView *folderview_create(MainWindow *mainwin)
{
	FolderView *folderview;
	GtkWidget *scrolledwin;
	
	cm_return_val_if_fail(mainwin != NULL, NULL);

	debug_print("Creating folder view...\n");
	folderview = g_new0(FolderView, 1);
	
	folderview->mainwin = mainwin;
	folderview->summaryview = mainwin->summaryview;

	folderview->normal_font = NULL;
	folderview->bold_font = NULL;

	folderview->open_item = NULL;
	folderview->menu_item = NULL;
	folderview->open_path = gtk_tree_path_new_from_string("0");

	folderview->data_row_clicked = FALSE;

	folderview->popups = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_foreach(folderview_popups, create_action_groups, folderview);

	scrolledwin = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_name(GTK_WIDGET(scrolledwin), "folderview");
	gtk_scrolled_window_set_policy
		(GTK_SCROLLED_WINDOW(scrolledwin),
		 GTK_POLICY_AUTOMATIC,
		 prefs_common.folderview_vscrollbar_policy);
	
	folderview->scrolledwin = scrolledwin;
	folderview->treeview = treeview_create_unconnected();

	/* Set up signal callbacks for the treeview */
	int i;
	for (i = 0; i < N_FOLDER_COLS; i++) {
		GtkTreeViewColumn *column = gtk_tree_view_get_column
		    (GTK_TREE_VIEW(folderview->treeview), i);
		if (column) {
			GtkWidget *button = gtk_tree_view_column_get_button(column);
			if (button) {
				//TM:no effect(with column as well)//gtk_widget_set_can_focus(button, FALSE);
				gtk_tree_view_column_set_clickable(column, TRUE);
				g_signal_connect(G_OBJECT(button),
							"button-press-event",
							G_CALLBACK(folderview_header_button_pressed),
							folderview);
			}
		}
	}
	g_signal_connect(G_OBJECT(folderview->treeview), "key_press_event",
			 G_CALLBACK(folderview_key_pressed), folderview);
	g_signal_connect(G_OBJECT(folderview->treeview), "button-press-event",
			G_CALLBACK(button_press_cb), folderview);
	g_signal_connect(G_OBJECT(folderview->treeview), "button-release-event",
			G_CALLBACK(button_release_cb), folderview);
	g_signal_connect(G_OBJECT(folderview->treeview), "cursor-changed",
			G_CALLBACK(cursor_changed_cb), folderview);
	g_signal_connect(G_OBJECT(folderview->treeview), "row-activated",
			G_CALLBACK(row_activated_cb), folderview);

	g_signal_connect(G_OBJECT(folderview->treeview), "row-expanded",
			G_CALLBACK(row_expanded_collapsed_cb), folderview);
	g_signal_connect(G_OBJECT(folderview->treeview), "row-collapsed",
			G_CALLBACK(row_expanded_collapsed_cb), folderview);

        /* drag and drop callbacks */
#ifndef GENERIC_UMPC
	/* drag-n-dropping folders on maemo is impractical as this
	 * opens the folder almost everytime */
	g_signal_connect(G_OBJECT(folderview->treeview), "drag_begin",
			 G_CALLBACK(folderview_drag_begin), folderview);
#endif
	g_signal_connect(G_OBJECT(folderview->treeview), "drag_data_get",
			 G_CALLBACK(folderview_drag_data_get),
			 folderview);
	g_signal_connect(G_OBJECT(folderview->treeview), "drag_motion",
			 G_CALLBACK(folderview_drag_motion_cb),
			 folderview);
	g_signal_connect(G_OBJECT(folderview->treeview), "drag_leave",
			 G_CALLBACK(folderview_drag_leave_cb),
			 folderview);
	g_signal_connect(G_OBJECT(folderview->treeview), "drag_data_received",
			 G_CALLBACK(folderview_drag_received_cb),
			 folderview);
	g_signal_connect(G_OBJECT(folderview->treeview), "drag_end",
			 G_CALLBACK(folderview_drag_end_cb),
			 folderview);
	gtk_drag_source_set(folderview->treeview,
			    GDK_BUTTON1_MASK,
			    folderview_drag_types, 2,
			    GDK_ACTION_MOVE | GDK_ACTION_COPY | GDK_ACTION_DEFAULT);
	gtk_drag_dest_set(folderview->treeview,
			  GTK_DEST_DEFAULT_ALL & ~GTK_DEST_DEFAULT_HIGHLIGHT,
			  folderview_drag_types, 2,
			  GDK_ACTION_MOVE | GDK_ACTION_COPY | GDK_ACTION_DEFAULT);
	gtk_tree_view_set_activate_on_single_click
			(GTK_TREE_VIEW(folderview->treeview), TRUE);
	//TM:TODO: Is there a "resize_column"-like signal?

	gtk_container_add(GTK_CONTAINER(scrolledwin), folderview->treeview);
	gtk_widget_show_all(scrolledwin);

	gtk_action_group_add_actions(mainwin->action_group,
			folderview_header_popup_entries,
			G_N_ELEMENTS(folderview_header_popup_entries),
			(gpointer)folderview);

	MENUITEM_ADDUI_MANAGER(mainwin->ui_manager, "/Menus", "FolderViewHeaderPopup", "FolderViewHeaderPopup", GTK_UI_MANAGER_MENU)
	MENUITEM_ADDUI_MANAGER(mainwin->ui_manager, "/Menus/FolderViewHeaderPopup", "SetDisplayedColumns", "FolderViewHeaderPopup/SetDisplayedColumns", GTK_UI_MANAGER_MENUITEM)

	folderview->headerpopupmenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(
				gtk_ui_manager_get_widget(mainwin->ui_manager,
					"/Menus/FolderViewHeaderPopup") ));

	/* Register our hooks */
	hooks_register_hook(FOLDER_ITEM_UPDATE_HOOKLIST,
			hook_folder_item_update, (gpointer)folderview);
	hooks_register_hook(FOLDER_UPDATE_HOOKLIST,
			hook_folder_update, (gpointer)folderview);

	folderview_list = g_list_append(folderview_list, folderview);

	return folderview;
}
/*
static void folderview_set_fonts(FolderView *folderview)
{
	PangoFontDescription *font_desc;
	GtkWidget *ctree = folderview->ctree;

	font_desc = pango_font_description_from_string(NORMAL_FONT);
	if (font_desc) {
		gtk_widget_override_font(ctree, font_desc);
		pango_font_description_free(font_desc);
	}

	if (!bold_style) {
		bold_style = gtk_style_copy(gtk_widget_get_style(ctree));

		if (prefs_common.derive_from_normal_font || !BOLD_FONT) {
			font_desc = pango_font_description_from_string(NORMAL_FONT);
			if (font_desc) {
				pango_font_description_free(bold_style->font_desc);
				bold_style->font_desc = font_desc;
			}
			pango_font_description_set_weight
				(bold_style->font_desc, PANGO_WEIGHT_BOLD);
		} else {
			font_desc = pango_font_description_from_string(BOLD_FONT);
			if (font_desc) {
				pango_font_description_free(bold_style->font_desc);
				bold_style->font_desc = font_desc;
			}
		}
	}
}
*/
void folderview_initialize(void)
{
	FolderViewPopup *fpopup;

	debug_print("folderview_initialize\n");

	fpopup = g_new0(FolderViewPopup, 1);
	fpopup->klass = "common";
	fpopup->path = "<CommonFolder>";
	fpopup->entries = folderview_common_popup_entries;
	fpopup->n_entries = G_N_ELEMENTS(folderview_common_popup_entries);
	fpopup->set_sensitivity = NULL;

	folderview_popups = g_hash_table_new(g_str_hash, g_str_equal);
	g_hash_table_insert(folderview_popups, "common", fpopup);
}

void folderview_init(FolderView *folderview)
{
	debug_print("folderview_init\n");

	stock_pixbuf_gdk(STOCK_PIXMAP_INBOX_CLOSE, &inboxxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_INBOX_CLOSE_HRM, &inboxhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_INBOX_OPEN, &inboxopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_INBOX_OPEN_HRM, &inboxopenhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_OUTBOX_CLOSE, &outboxxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_OUTBOX_CLOSE_HRM, &outboxhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_OUTBOX_OPEN, &outboxopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_OUTBOX_OPEN_HRM, &outboxopenhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_CLOSE, &folderxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_CLOSE_HRM, &folderhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_OPEN, &folderopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_OPEN_HRM, &folderopenhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_TRASH_OPEN, &trashopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_TRASH_OPEN_HRM, &trashopenhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_TRASH_CLOSE, &trashxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_TRASH_CLOSE_HRM, &trashhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_QUEUE_CLOSE, &queuexpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_QUEUE_CLOSE_HRM, &queuehrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_QUEUE_OPEN, &queueopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_QUEUE_OPEN_HRM, &queueopenhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DRAFTS_CLOSE, &draftsxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DRAFTS_OPEN, &draftsopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_SUBS_OPEN, &foldersubsopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_SUBS_CLOSE, &foldersubsxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_NOSELECT_OPEN, &foldernoselectopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_NOSELECT_CLOSE, &foldernoselectxpm);

	stock_pixbuf_gdk(STOCK_PIXMAP_INBOX_CLOSE_MARK, &m_inboxxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_INBOX_CLOSE_HRM_MARK, &m_inboxhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_INBOX_OPEN_MARK, &m_inboxopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_INBOX_OPEN_HRM_MARK, &m_inboxopenhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_OUTBOX_CLOSE_MARK, &m_outboxxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_OUTBOX_CLOSE_HRM_MARK, &m_outboxhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_OUTBOX_OPEN_MARK, &m_outboxopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_OUTBOX_OPEN_HRM_MARK, &m_outboxopenhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_CLOSE_MARK, &m_folderxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_CLOSE_HRM_MARK, &m_folderhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_OPEN_MARK, &m_folderopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_OPEN_HRM_MARK, &m_folderopenhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_TRASH_OPEN_MARK, &m_trashopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_TRASH_OPEN_HRM_MARK, &m_trashopenhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_TRASH_CLOSE_MARK, &m_trashxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_TRASH_CLOSE_HRM_MARK, &m_trashhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_QUEUE_CLOSE_MARK, &m_queuexpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_QUEUE_CLOSE_HRM_MARK, &m_queuehrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_QUEUE_OPEN_MARK, &m_queueopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_QUEUE_OPEN_HRM_MARK, &m_queueopenhrmxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DRAFTS_CLOSE_MARK, &m_draftsxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DRAFTS_OPEN_MARK, &m_draftsopenxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_SUBS_CLOSE_MARK, &m_foldersubsxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_DIR_NOSELECT_CLOSE_MARK, &m_foldernoselectxpm);
	stock_pixbuf_gdk(STOCK_PIXMAP_QUICKSEARCH, &searchicon);
	
	folderview->normal_font = pango_font_description_from_string(NORMAL_FONT);
	folderview->bold_font = pango_font_description_from_string(BOLD_FONT);
}

/* Recursive helper function for folderview_set()
 * Adds folder at 'node' to the tree, then calls itself for
 * all children of that folder. If 'expand_parent' is TRUE,
 * expand the parent row after adding ourself. */
void folderview_add_folder(FolderView *folderview, GtkTreeStore *store,
			   GNode *node, GtkTreeIter *parent,
			   gboolean expand_parent)
{
	GtkTreeIter iter;
	GtkTreePath *path = NULL;
	FolderItem *item;
	GNode *n;
	gboolean expand_me = FALSE;

	cm_return_if_fail(node != NULL);
	cm_return_if_fail(node->data != NULL);
	cm_return_if_fail(store != NULL);

	item = FOLDER_ITEM(node->data);

	/* Add the folder item itself */
	debug_print("+++ appending '%s'\n", item->path);
	gtk_tree_store_append(store, &iter, parent);
	gtk_tree_store_set(store, &iter,
			F_COL_FOLDERITEM, item,
			F_COL_FOLDER, item->name,
			F_COL_ALIGN, PANGO_ALIGN_RIGHT,
			-1);

	folderview_update_row_iter(folderview, &iter, item);

	/* Expand the parent if requested. */
	if (parent != NULL && expand_parent) {
		path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), parent);
		if (!gtk_tree_view_row_expanded(
					GTK_TREE_VIEW(folderview->treeview), path)) {
			debug_print(">>> expanding parent '%s'\n", item->path);
			gtk_tree_view_expand_row(GTK_TREE_VIEW(folderview->treeview),
					path, FALSE);
		}
		gtk_tree_path_free(path);
	}

	/* Since a GtkTreeView row can only be expanded if it has at
	 * least one child, we need to do it after the first child
	 * has been added. */
	if (!item->collapsed)
		expand_me = TRUE;

	/* Add all children recursively */
	for (n = node->children; n != NULL; n = n->next)
		folderview_add_folder(folderview, store, n, &iter, expand_me);
}

/* This will effectively reset contents of the folderview, repopulating
 * it from folder list. */
void folderview_set(FolderView *folderview)
{
	debug_print("folderview_set\n");
	GtkTreeModel *store =
		gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));

	gtk_tree_store_clear(GTK_TREE_STORE(store));

	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(store),
			F_COL_FOLDER, GTK_SORT_ASCENDING);

	GList *list = folder_get_list();
	for (; list != NULL; list = list->next) {
		Folder *folder = FOLDER(list->data);
		folderview_add_folder(folderview, GTK_TREE_STORE(store),
				      folder->node, NULL, FALSE);
	}
}

void folderview_set_all(void)
{
	GList *list;
	
	debug_print("folderview_set_all\n");

	for (list = folderview_list; list != NULL; list = list->next)
		folderview_set((FolderView *)list->data);
}

typedef struct _PostponedSelectData
{
	FolderView *folderview;
	FolderItem *folderitem;
} PostponedSelectData;

static gboolean postpone_select(void *data)
{
	PostponedSelectData *psdata = (PostponedSelectData *)data;
	debug_print("trying again\n");
	//psdata->folderview->open_folder = TRUE;
	main_window_cursor_normal(psdata->folderview->mainwin);
	STATUSBAR_POP(psdata->folderview->mainwin);
	folderview_open(psdata->folderview, psdata->folderitem);
	g_free(psdata);
	return FALSE;
}

/* This will just open specified folderitem, without doing anything to
 * the folderview widget. Any UI changes like updating treeview cursor
 * or adjusting folder icons need to be done by the caller. */
static gboolean folderview_open(FolderView *folderview, FolderItem *item)
{
	gchar *buf;
	gboolean summary_opened = FALSE;

	cm_return_val_if_fail(folderview != NULL, FALSE);
	cm_return_val_if_fail(item != NULL, FALSE);

	debug_print("folderview_open '%s'\n", item->name);

	if (item == folderview->open_item) {
		debug_print("folder already opened\n");
		return FALSE;
	}

	START_TIMING("");

	buf = g_strdup_printf(_("Opening folder %s..."), item->path ?
			item->path : "(null)");
	STATUSBAR_PUSH(folderview->mainwin, buf);
	g_free(buf);

	main_window_cursor_wait(folderview->mainwin);

	gint res = folder_item_open(item);

	if (res == -1 && item->no_select == FALSE) {
		main_window_cursor_normal(folderview->mainwin);
		STATUSBAR_POP(folderview->mainwin);
		END_TIMING();
		alertpanel_error(_("Folder could not be opened."));
		return FALSE;
	} else if (res == -2 && item->no_select == FALSE) {
		PostponedSelectData *data = g_new0(PostponedSelectData, 1);
		data->folderitem = item;
		data->folderview = folderview;
		debug_print("postponing open of %s till end of scan\n",
			item->path ? item->path:item->name);
		g_timeout_add(500, postpone_select, data);
		END_TIMING();
		return FALSE;
	}

	main_window_cursor_normal(folderview->mainwin);

	summary_set_prefs_from_folderitem(folderview->summaryview, item);
	summary_opened = summary_show(folderview->summaryview, item, FALSE);

	folder_clean_cache_memory(item);

	STATUSBAR_POP(folderview->mainwin);
	END_TIMING();

	if (summary_opened)
		folderview->open_item = item;

	return summary_opened;
}

struct finditer_ctx {
	FolderItem *item;
	GtkTreeIter *iter;
};

/* This function is intended to be used with
 * gtk_tree_model_foreach(), and user_data has to point
 * to a struct finditer_ctx with a valid item member.
 * The iter member has to be freed by the caller afterwards. */
static gboolean folderview_find_iter_foreach_func(GtkTreeModel *model,
						  GtkTreePath *path,
						  GtkTreeIter *iter,
						  gpointer user_data)
{
	struct finditer_ctx *ctx = (struct finditer_ctx *)user_data;
	FolderItem *item;
	
	cm_return_val_if_fail(ctx->item != NULL, FALSE);

	gtk_tree_model_get(model, iter,
			F_COL_FOLDERITEM, &item, -1);

	if (ctx->item == item) {
		debug_print("folderview_find_iter_foreach_func: found the row for '%s'\n",
				ctx->item->name);
		/* We need to make our own copy of the iterator, since the
		 * original is no longer valid after this function returns. */
		ctx->iter = gtk_tree_iter_copy(iter);
		return TRUE;
	}

	return FALSE;
}

static void folderview_add_folderitem(FolderView *folderview, FolderItem *item)
{
	/* Find the row with parent item. */
	struct finditer_ctx *ctx = g_new0(struct finditer_ctx, 1);
	ctx->item = folder_item_parent(item);
	ctx->iter = NULL;
	GtkTreeModel *model =
		gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));

	gtk_tree_model_foreach(model, folderview_find_iter_foreach_func, ctx);

	if (ctx->iter == NULL) {
		g_free(ctx);
		return;
	}

	GtkTreeIter *parent_iter = ctx->iter;

	/* Add the folder */
	folderview_add_folder(folderview, GTK_TREE_STORE(model), item->node,
			      parent_iter, TRUE);

	gtk_tree_iter_free(ctx->iter);
	g_free(ctx);

	/* Fire "folder item updated" hooks. */
	FolderItemUpdateData hookdata;
	hookdata.item = item;
	hookdata.update_flags = F_ITEM_UPDATE_NAME;
	hookdata.msg = NULL;
	hooks_invoke(FOLDER_ITEM_UPDATE_HOOKLIST, &hookdata);
}

static void folderview_remove_folderitem(FolderView *folderview,
					 FolderItem *item)
{
	/* Find the row with our item. */
	struct finditer_ctx *ctx = g_new0(struct finditer_ctx, 1);
	ctx->item = item;
	ctx->iter = NULL;
	GtkTreeModel *model =
		gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));

	gtk_tree_model_foreach(model, folderview_find_iter_foreach_func, ctx);

	if (ctx->iter == NULL) {
		g_free(ctx);
		return;
	}

	GtkTreeIter *iter = ctx->iter;

	gtk_tree_store_remove(GTK_TREE_STORE(model), iter);

	gtk_tree_iter_free(ctx->iter);
	g_free(ctx);
}

/* This will place cursor on row belonging to the specified folderitem,
 * and optionally open the folder itself. */
void folderview_select(FolderView *folderview, FolderItem *item)
{
	debug_print("folderview_select\n");

	/* No need to reopen, that would just disturb the user */
	if (item == folderview->open_item) {
		debug_print("selected folder already opened\n");
		return;
	} else if (folderview->open_item != NULL) {
		debug_print("closing previously opened folder\n");
		folderview_unselect(folderview);
	}

	/* Find the row with our item. */
	struct finditer_ctx *ctx = g_new0(struct finditer_ctx, 1);
	ctx->item = item;
	ctx->iter = NULL;
	GtkTreeModel *model =
		gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));

	gtk_tree_model_foreach(model, folderview_find_iter_foreach_func, ctx);

	/* Place cursor on our row */
	if (ctx->iter != NULL) {
		/* Expand row first (can't be selected otherwise) */
		GtkTreePath *path = gtk_tree_model_get_path(model, ctx->iter);
		gtk_tree_view_expand_to_path(GTK_TREE_VIEW(folderview->treeview),path);
		gtk_tree_path_free(path);

		GtkTreeSelection *selection =
			gtk_tree_view_get_selection(GTK_TREE_VIEW(folderview->treeview));
		gtk_tree_selection_select_iter(selection, ctx->iter);
	}
	gtk_tree_iter_free(ctx->iter);
	g_free(ctx);
	debug_print("selected folder '%s'\n", item->name);

	/* Folderview cursor placed, open the item */
	folderview_open(folderview, item);
}

void folderview_unselect(FolderView *folderview)
{
	debug_print("folderview_unselect\n");
	FolderItem *olditem = folderview->open_item;

	/* Save cache for old folder */
	/* We don't want to lose all caches if sylpheed crashed */
	if (olditem) {
		gchar *buf;
		buf = g_strdup_printf(_("Closing Folder %s..."),
			olditem->path ? olditem->path : olditem->name);
		/* will be null if we just moved the previously opened folder */
		STATUSBAR_PUSH(folderview->mainwin, buf);
		main_window_cursor_wait(folderview->mainwin);
		g_free(buf);
		summary_save_prefs_to_folderitem(folderview->summaryview,
						 olditem);
		folder_item_close(olditem);
		main_window_cursor_normal(folderview->mainwin);
		STATUSBAR_POP(folderview->mainwin);
	}

	folderview->open_item = NULL;

	/* show closed folder icon */
	folderview_update_row(folderview, olditem);
}

static GtkTreePath *folderview_find_next_with_flag(GtkTreeView *treeview,
						   GtkTreePath *path,
						   MsgPermFlags flag)
{
	FolderItem *item;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path_old = NULL;

	model = gtk_tree_view_get_model(treeview);
	if (!model)
		return NULL;

	if (path)
		path = gtkut_tree_path_next(treeview, path);
	else
		path = gtk_tree_path_new_from_string("0"); /* top node */

	for (; path && gtk_tree_model_get_iter(model, &iter, path);
	       path = gtkut_tree_path_next(treeview, path)) {
		if (path_old) {
			gtk_tree_path_free(path_old);
			path_old = NULL;
		}
		gtk_tree_model_get(model, &iter, F_COL_FOLDERITEM, &item, -1);
		if (!item)
			continue;
		if (item->stype == F_TRASH || item->stype == F_DRAFT)
			continue;
		switch (flag) {
		case MSG_UNREAD:
			if((item->unread_msgs > 0) && (!item->prefs || !item->prefs->skip_on_goto_unread_or_new))
				goto out;
			break;
		case MSG_NEW:
			if((item->new_msgs > 0) && (!item->prefs || !item->prefs->skip_on_goto_unread_or_new))
				goto out;
			break;
		case MSG_MARKED:
			if(item->marked_msgs > 0)
				goto out;
			break;
		default:
			if(item->total_msgs > 0)
				goto out;
			break;
		}
		path_old = path;
	}

	if (path_old)
		gtk_tree_path_free(path_old);
	return NULL;

out:
	if (path_old)
		gtk_tree_path_free(path_old);
	return path;
}

void folderview_select_next_with_flag(FolderView *folderview,
				      MsgPermFlags flag)
{
	GtkTreeView *treeview = GTK_TREE_VIEW(folderview->treeview);
	GtkTreePath *path = NULL, *startpath = NULL;
	EntryAction last_summary_select_prio = prefs_common.summary_select_prio[0];
	gboolean update_cursor = FALSE;

	switch (flag) {
	case MSG_UNREAD:
		prefs_common.summary_select_prio[0] = ACTION_OLDEST_UNREAD;
		break;
	case MSG_NEW:
		prefs_common.summary_select_prio[0] = ACTION_OLDEST_NEW;
		break;
	case MSG_MARKED:
		prefs_common.summary_select_prio[0] = ACTION_OLDEST_MARKED;
		break;
	default:
		prefs_common.summary_select_prio[0] = ACTION_OLDEST_LIST;
		break;
	}

	gtk_tree_view_get_cursor(treeview, &startpath, NULL);
	if (!startpath)
		return;

	path = folderview_find_next_with_flag(treeview, startpath, flag);
	if (path != NULL) {
		update_cursor = TRUE;
		goto out;
	}

/* TM: - this inhibits search from top - what was this used for?
	if (!folderview->open_item ||
	    folderview->open_item == folderview_get_selected_item(folderview)) {
		goto out;
	}
*/

	/* search again from the first node */
	path = folderview_find_next_with_flag(treeview, NULL, flag);
	update_cursor = (path != NULL);

out:
	if (update_cursor) {
		gtk_tree_view_set_cursor(treeview, path, NULL, FALSE);
		row_activated_cb(treeview, path,
				 gtk_tree_view_get_column(treeview,0),
				 folderview);
	}
	if (startpath)
		gtk_tree_path_free(startpath);
	if (path)
		gtk_tree_path_free(path);
	prefs_common.summary_select_prio[0] = last_summary_select_prio;
}

FolderItem *folderview_get_selected_item(FolderView *folderview)
{
	debug_print("folderview_get_selected_item\n");

	/* If this function is called by a popup menu callback,
	 * they want the FolderItem for which the menu is open. */
	if (folderview->menu_item != NULL)
		return folderview->menu_item;

	GtkTreeIter iter;
	FolderItem *item;

	GtkTreeSelection *selection =
		gtk_tree_view_get_selection(GTK_TREE_VIEW(folderview->treeview));

	cm_return_val_if_fail(folderview != NULL, NULL);

	if (gtk_tree_selection_count_selected_rows(selection) == 0)
		return NULL;

	gtk_tree_selection_get_selected(selection, NULL, &iter);
	GtkTreeModel *model =
		gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));

	cm_return_val_if_fail(model != NULL, NULL);

	/* Get the FolderItem belonging to that row */
	gtk_tree_model_get(model, &iter,
			F_COL_FOLDERITEM, &item, -1);

	return item;
}

FolderItem *folderview_get_opened_item(FolderView *folderview)
{
	debug_print("folderview_get_opened_item\n");
	return folderview->open_item;
}

static void get_row_pixbufs(FolderView *folderview, GtkTreeIter *iter,
			    GdkPixbuf **pixbuf_closed, GdkPixbuf **pixbuf_open)
{
	FolderItem *item;
	GdkPixbuf *xpm, *openxpm;

	cm_return_if_fail(iter != NULL);

	debug_print("set_row_pixbufs\n");

	GtkTreeModel *model =
		gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));

	gtk_tree_model_get(model, iter,
			F_COL_FOLDERITEM, &item, -1);

	cm_return_if_fail(item != NULL);

	SpecialFolderItemType stype = item->stype;
	gboolean mark = FALSE;

	GtkTreePath *path = gtk_tree_model_get_path(model, iter);
	gboolean expanded =
		gtk_tree_view_row_expanded(GTK_TREE_VIEW(folderview->treeview), path);
	gtk_tree_path_free(path);

	if (expanded)
		mark = (item->marked_msgs != 0);
	else
		mark = folder_has_marked_children(item);

	/* Any folder that has a parent of special type will inherit that
	 * type's icon. */
	if (stype == F_NORMAL) {
		if (folder_has_parent_of_type(item, F_TRASH))
			stype = F_TRASH;
		else if (folder_has_parent_of_type(item, F_DRAFT))
			stype = F_DRAFT;
		else if (folder_has_parent_of_type(item, F_OUTBOX))
			stype = F_OUTBOX;
		else if (folder_has_parent_of_type(item, F_QUEUE))
			stype = F_QUEUE;
	}

	switch (stype) {
		case F_INBOX:
			if (item->hide_read_msgs || item->hide_read_threads) {
				xpm = mark?m_inboxhrmxpm:inboxhrmxpm;
				openxpm = mark?m_inboxopenhrmxpm:inboxopenhrmxpm;
			} else {
				xpm = mark?m_inboxxpm:inboxxpm;
				openxpm = mark?m_inboxopenxpm:inboxopenxpm;
			}
			break;

		case F_OUTBOX:
			if (item->hide_read_msgs || item->hide_read_threads) {
				xpm = mark?m_outboxhrmxpm:outboxhrmxpm;
				openxpm = mark?m_outboxopenhrmxpm:outboxopenhrmxpm;
			} else {
				xpm = mark?m_outboxxpm:outboxxpm;
				openxpm = mark?m_outboxopenxpm:outboxopenxpm;
			}
			break;

		case F_QUEUE:
			if (item->hide_read_msgs || item->hide_read_threads) {
				xpm = mark?m_queuehrmxpm:queuehrmxpm;
				openxpm = mark?m_queueopenhrmxpm:queueopenhrmxpm;
			} else {
				xpm = mark?m_queuexpm:queuexpm;
				openxpm = mark?m_queueopenxpm:queueopenxpm;
			}
			break;
		case F_TRASH:
			if (item->hide_read_msgs || item->hide_read_threads) {
				xpm = mark?m_trashhrmxpm:trashhrmxpm;
				openxpm = mark?m_trashopenhrmxpm:trashopenhrmxpm;
			} else {
				xpm = mark?m_trashxpm:trashxpm;
				openxpm = mark?m_trashopenxpm:trashopenxpm;
			}
			break;

		case F_DRAFT:
			xpm = mark?m_draftsxpm:draftsxpm;
			openxpm = mark?m_draftsopenxpm:draftsopenxpm;
			break;
		default:
			if (!item->path &&
					FOLDER_TYPE(item->folder) == F_IMAP &&
					item->folder->account->imap_subsonly) {
				xpm = mark?m_foldersubsxpm:foldersubsxpm;
				openxpm = foldersubsopenxpm;
			} else if (item->no_select) {
				xpm = mark?m_foldernoselectxpm:foldernoselectxpm;
				openxpm = foldernoselectopenxpm;
			} else if (item->hide_read_msgs || item->hide_read_threads) {
				xpm = mark?m_folderhrmxpm:folderhrmxpm;
				openxpm = mark?m_folderopenhrmxpm:folderopenhrmxpm;
			} else {
				xpm = mark?m_folderxpm:folderxpm;
				openxpm = mark?m_folderopenxpm:folderopenxpm;
			}
	}

	if (item->search_match) {
		xpm = openxpm = searchicon;
	}

	*pixbuf_closed = xpm;
	*pixbuf_open = openxpm;
}

static void folderview_update_row_iter(FolderView *folderview,
				       GtkTreeIter *iter, FolderItem *orgitem)
{
	gboolean expanded;
	gboolean use_bold, use_color;
	gboolean add_unread_mark, add_sub_match_mark;
	GdkPixbuf *pixbuf_closed = NULL, *pixbuf_open = NULL;
	GdkRGBA black = { 0, 0, 0, 1 };
	gchar *name_org;
	gchar *name;

	cm_return_if_fail(iter != NULL);

	debug_print("folderview_update_row_iter\n");

	GtkTreeModel *model =
		gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));
	FolderItem *item = orgitem;

	if (item == NULL) {
		/* Get the FolderItem belonging to this row */
		gtk_tree_model_get(model, iter,
				F_COL_FOLDERITEM, &item,
				-1);
	}
	cm_return_if_fail(item != NULL);

	/* Prepare correct folder icons. */
	get_row_pixbufs(folderview, iter, &pixbuf_closed, &pixbuf_open);

	/* Find out if this row is expanded in the treeview. */
	GtkTreePath *path = gtk_tree_model_get_path(model, iter);
	expanded =
		gtk_tree_view_row_expanded(GTK_TREE_VIEW(folderview->treeview), path);
	gtk_tree_path_free(path);

	if (!expanded) {
		add_unread_mark = folder_has_unread_children(item);
		add_sub_match_mark = folder_has_matching_children(item);
	} else {
		add_unread_mark = add_sub_match_mark = FALSE;
	}

	/* Bold & color */
	if (folder_has_parent_of_type(item, F_OUTBOX) ||
			folder_has_parent_of_type(item, F_TRASH)) {
		use_bold = use_color = FALSE;
	} else if (folder_has_parent_of_type(item, F_QUEUE)) {
		GSList *list = folder_item_get_msg_list(item);
		GSList *cur;
		use_bold = use_color = FALSE;
		for (cur = list; cur; cur = cur->next) {
			MsgInfo *msginfo = (MsgInfo *)cur->data;
			if (!MSG_IS_DELETED(msginfo->flags)) {
				/* highlight queue folder if there are any messages */
				use_bold = use_color = TRUE;
				break;
			}
		}

		if (!expanded && !use_bold && folder_has_children(item))
			use_bold = use_color = TRUE;
		procmsg_msg_list_free(list);
	} else {
		/* if unread messages exist, print with bold font */
		use_bold = (item->unread_msgs > 0 || item->new_msgs > 0)
			|| add_unread_mark || item->op_count > 0;
		/* if new messages exist, print with colored letter */
		use_color =
			item->new_msgs > 0 ||
			(add_unread_mark && folder_has_new_children(item));
	}

	GdkRGBA *color = NULL;
	if (use_color) {
		color = &folderview->color_new;
	} else if (item->op_count > 0) {
		color = &folderview->color_op;
	} else if (!gdk_rgba_equal(&item->prefs->color, &black)) {
		color = &item->prefs->color;
	}

	/* Add suffixes to folder name:
	 * (unread#)	(if set in prefs)
	 * "+"		(if collapsed subfolders contain search matches)
	 * "!"		(if marked message has unread replies) */
	name = NULL;
	name_org = folder_item_get_name(item);
	if (prefs_common.display_folder_unread) {
		if (folder_has_parent_of_type(item, F_QUEUE)) {
			/* only total_msgs matters here */
			if (item->total_msgs > 0) {
				/* show total number (should be equal to the unread number)
				   and signs if any */
				name = g_strdup_printf("%s (%d%s%s)",
							name_org, item->total_msgs,
							(add_unread_mark || add_sub_match_mark) ? "+" : "",
							(item->unreadmarked_msgs > 0) ? "!" : "");
			}
		} else {
			if (prefs_common.display_folder_unread == 1) {
				if (item->unread_msgs > 0) {
					/* show unread number and signs */
					name = g_strdup_printf("%s (%d%s%s)",
								name_org, item->unread_msgs,
								(add_unread_mark || add_sub_match_mark) ? "+" : "",
								(item->unreadmarked_msgs > 0) ? "!" : "");
				}
			} else {
				if (item->total_msgs > 0) {
					/* show unread number, total number and signs if any */
					name = g_strdup_printf("%s (%d/%d%s%s)",
								name_org, item->unread_msgs, item->total_msgs,
								(add_unread_mark || add_sub_match_mark) ? "+" : "",
								(item->unreadmarked_msgs > 0) ? "!" : "");
				}
			}
		}
		if ((name == NULL) &&
			(add_unread_mark || add_sub_match_mark || (item->unreadmarked_msgs > 0))) {
			/* no unread/total numbers, but at least one sign */
			name = g_strdup_printf("%s (%s%s)",
						name_org,
						(add_unread_mark || add_sub_match_mark) ? "+" : "",
						(item->unreadmarked_msgs > 0) ? "!" : "");
		}
	}
	if (name == NULL) {
		/* last fallback, folder name_org only or with +! sign */
		if (item->unreadmarked_msgs > 0 && add_sub_match_mark) {
			name = g_strdup_printf("%s%s",
						name_org, " (+!)");
		} else if (item->unreadmarked_msgs > 0) {
			name = g_strdup_printf("%s%s",
						name_org, " (!)");
		} else if (add_sub_match_mark) {
			name = g_strdup_printf("%s%s",
						name_org, " (+)");
		} else {
			name = g_strdup_printf("%s", name_org);
		}
	}

	gchar n[32], u[32], t[32];
	g_snprintf(n, 32, "%d", item->new_msgs);
	g_snprintf(u, 32, "%d", item->unread_msgs);
	g_snprintf(t, 32, "%d", item->total_msgs);

	gboolean has_parent = !!folder_item_parent(item);

	gtk_tree_store_set(GTK_TREE_STORE(model), iter,
			F_COL_FOLDER, name,
			F_COL_NEW, has_parent ? n : "-",
			F_COL_UNREAD, has_parent ? u : "-",
			F_COL_TOTAL, has_parent ? t : "-",
			F_COL_ICON, (expanded || 
				(folderview->open_item == item))
				? pixbuf_open : pixbuf_closed,
			F_COL_FONT, use_bold ?
				folderview->bold_font : folderview->normal_font,
			F_COL_COLOR, color,
			-1);
	g_free(name);
	g_free(name_org);
}

void folderview_update_row(FolderView *folderview,
			   FolderItem *item)
{
	cm_return_if_fail(folderview != NULL);
	cm_return_if_fail(item != NULL);

	struct finditer_ctx *ctx = g_new0(struct finditer_ctx, 1);
	ctx->item = item;
	ctx->iter = NULL;
	GtkTreeModel *model =
		gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));

	debug_print("folderview_update_row for '%s'\n", item->name);

	gtk_tree_model_foreach(model, folderview_find_iter_foreach_func, ctx);

	if (ctx->iter != NULL)
		folderview_update_row_iter(folderview, ctx->iter, item);

	/* We have to free the iter, since we heap-allocated it inside
	 * the "foreach func". */
	gtk_tree_iter_free(ctx->iter);
	g_free(ctx);
}

static gchar *get_scan_str(FolderItem *item)
{
	if (item->path)
		return g_strdup_printf(_("Scanning folder %s/%s..."),
				      item->folder->name, item->path);
	else
		return g_strdup_printf(_("Scanning folder %s..."),
				      item->folder->name);
}

static void folderview_scan_tree_func(Folder *folder, FolderItem *item,
				      gpointer data)
{
	GList *list;
	for (list = folderview_list; list != NULL; list = list->next) {
		FolderView *folderview = (FolderView *)list->data;
		MainWindow *mainwin = folderview->mainwin;
		gchar *str = get_scan_str(item);

		STATUSBAR_PUSH(mainwin, str);
		STATUSBAR_POP(mainwin);
		g_free(str);
	}
}

void folderview_rescan_tree(Folder *folder, gboolean rebuild)
{
	GtkWidget *window;
	MainWindow *mainwin = mainwindow_get_mainwindow();
	FolderView *folderview = NULL;
	GtkAdjustment *pos = NULL;
	gint height = 0;

	cm_return_if_fail(folder != NULL);

	if (!folder->klass->scan_tree) return;

	if (rebuild && 
	    alertpanel_full(_("Rebuild folder tree"), 
	    		 _("Rebuilding the folder tree will remove "
			   "local caches. Do you want to continue?"),
		       	 NULL, _("_No"), NULL, _("_Yes"), NULL, NULL,
			 ALERTFOCUS_FIRST, FALSE, NULL, ALERT_WARNING)
		!= G_ALERTALTERNATE) {
		return;
	}

	inc_lock();
	if (rebuild)
		window = label_window_create(_("Rebuilding folder tree..."));
	else 
		window = label_window_create(_("Scanning folder tree..."));

	if (mainwin)
		folderview = mainwin->folderview;
	
	if (folderview) {
		pos = gtk_scrolled_window_get_vadjustment(
					GTK_SCROLLED_WINDOW(folderview->scrolledwin));
		height = gtk_adjustment_get_value(pos);
	}

	folder_set_ui_func(folder, folderview_scan_tree_func, NULL);
	folder_scan_tree(folder, rebuild);
	folder_set_ui_func(folder, NULL, NULL);

	folderview_set_all();

	if (folderview) {
		pos = gtk_scrolled_window_get_vadjustment(
					GTK_SCROLLED_WINDOW(folderview->scrolledwin));
		gtk_adjustment_set_value(pos, height);
	}
	label_window_destroy(window);
	inc_unlock();
}

struct check_ctx {
	FolderView *folderview;
	Folder *folder;
	gint new_msgs;
	gint former_new_msgs;
	gboolean found_our_folder;
};

static gboolean _check_folder_foreach_func(GtkTreeModel *model,
					   GtkTreePath *path,
					   GtkTreeIter *iter,
					   gpointer user_data)
{
	struct check_ctx *ctx = (struct check_ctx *)user_data;

	cm_return_val_if_fail(ctx != NULL, FALSE);

	FolderView *folderview = ctx->folderview;
	Folder *folder = ctx->folder;

	gchar *str;
	FolderItem *item = NULL;
	gtk_tree_model_get(model, iter,
			F_COL_FOLDERITEM, &item, -1);

	if (!item || !item->path || !item->folder)
		return FALSE;
	if (folder && folder != item->folder) {
		/* If we already found our Folder, then this is the first
		 * row below it belonging to next Folder, so we stop
		 * iterating. */
		return ctx->found_our_folder;
	}
	if (item->no_select)
		return FALSE;
	if (!folder && !FOLDER_IS_LOCAL(item->folder))
		return FALSE;
	if (!item->prefs->newmailcheck)
		return FALSE;
	if (item->processing_pending) {
		debug_print("skipping %s, processing pending\n",
				item->path ? item->path : item->name);
		return FALSE;
	}
	if (item->scanning != ITEM_NOT_SCANNING) {
		debug_print("skipping %s, scanning\n",
				item->path ? item->path : item->name);
		return FALSE;
	}

	str = get_scan_str(item);

	STATUSBAR_PUSH(folderview->mainwin, str);
	GTK_EVENTS_FLUSH();
	g_free(str);

	gint former_new = item->new_msgs;
	gint former_unread = item->unread_msgs;
	gint former_total = item->total_msgs;

	if (folder->klass->scan_required &&
			(folder->klass->scan_required(folder, item) ||
			 folder->inbox == item ||
			 item->opened ||
			 item->processing_pending)) {
		if (folder_item_scan(item) < 0) {
			if (folder) {
				summaryview_unlock(folderview->summaryview, item);
				if (FOLDER_TYPE(folder) == F_NEWS ||
							FOLDER_IS_LOCAL(folder)) {
					log_error(LOG_PROTOCOL, _("Couldn't scan folder %s\n"),
							item->path ? item->path : item->name);
					STATUSBAR_POP(folderview->mainwin);
					return FALSE;
				} else if (!FOLDER_IS_LOCAL(folder)) {
					STATUSBAR_POP(folderview->mainwin);
					return TRUE;
				}
			}
		}
	} else if (!folder->klass->scan_required) {
		if (folder_item_scan(item) < 0) {
			summaryview_unlock(folderview->summaryview, item);
			if (folder && !FOLDER_IS_LOCAL(folder)) {
				STATUSBAR_POP(folderview->mainwin);
				return TRUE;
			}
		}
	}

	if (former_new != item->new_msgs ||
			former_unread != item->unread_msgs ||
			former_total != item->total_msgs)
		folderview_update_row_iter(folderview, iter, item);

	ctx->new_msgs += item->new_msgs;
	ctx->former_new_msgs += former_new;
	STATUSBAR_POP(folderview->mainwin);

	return FALSE;
}

gint folderview_check_new(Folder *folder)
{
	debug_print("folderview_check_new\n");

 	GList *list;
	gint new_msgs = 0;
	gint former_new_msgs = 0;
	FolderView *folderview;
	GtkTreeModel *model;;

	for (list = folderview_list; list != NULL; list = list->next) {
		folderview = (FolderView *)list->data;
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));

		struct check_ctx *ctx = g_new0(struct check_ctx, 1);
		ctx->folderview = folderview;
		ctx->folder = folder;
		ctx->new_msgs = 0;
		ctx->former_new_msgs = 0;
		ctx->found_our_folder = FALSE;

		inc_lock();
		main_window_lock(folderview->mainwin);

		gtk_tree_model_foreach(model, _check_folder_foreach_func, ctx);
		main_window_unlock(folderview->mainwin);
		inc_unlock();

		new_msgs += ctx->new_msgs;
		former_new_msgs = ctx->former_new_msgs;
		g_free(ctx);
	}

	folder_write_list();

	/* Number of new messages since last check is the just the difference 
	 * between former_new_msgs and new_msgs. If new_msgs is less than
	 * former_new_msgs, that would mean another session accessed the folder
	 * and the result is not well defined.
	 */
	new_msgs = (former_new_msgs < new_msgs ? new_msgs - former_new_msgs : 0);

	return new_msgs;
}

void folderview_check_new_all(void)
{
	GList *list;
	GtkWidget *window;
	FolderView *folderview;

	debug_print("folderview_check_new_all\n");

	folderview = (FolderView *)folderview_list->data;

	inc_lock();
	main_window_lock(folderview->mainwin);
	window = label_window_create
		(_("Checking for new messages in all folders..."));

	list = folder_get_list();
	for (; list != NULL; list = list->next) {
		Folder *folder = list->data;
		folderview_check_new(folder);
	}

	folder_write_list();
	folderview_set_all();

	label_window_destroy(window);
	main_window_unlock(folderview->mainwin);
	inc_unlock();
}

void folderview_move_folder(FolderView *folderview, FolderItem *from_folder,
			    FolderItem *to_folder, gboolean copy)
{
	FolderItem *new_folder = NULL;
	gchar *buf;
	gint status;

	cm_return_if_fail(folderview != NULL);
	cm_return_if_fail(from_folder != NULL);
	cm_return_if_fail(to_folder != NULL);

	if (prefs_common.warn_dnd) {
		buf = g_strdup_printf(copy ? _("Do you really want to copy folder '%s' in '%s'?"):
					     _("Do you really want to make folder '%s' a subfolder of '%s'?"),
					from_folder->name, to_folder->name);
		status = alertpanel_full(copy ? _("Copy folder"):_("Move folder"), buf,
				       	 NULL, _("_No"), NULL, _("_Yes"), NULL, NULL,
					 ALERTFOCUS_FIRST, TRUE, NULL, ALERT_QUESTION);
		g_free(buf);

		if ((status & ~G_ALERTDISABLE) != G_ALERTALTERNATE)
			return;
		else if (status & G_ALERTDISABLE)
			prefs_common.warn_dnd = FALSE;
	}

	buf = g_strdup_printf(copy ? _("Copying %s to %s..."):_("Moving %s to %s..."),
				from_folder->name, to_folder->name);
	STATUSBAR_PUSH(folderview->mainwin, buf);
	g_free(buf);
	summary_clear_all(folderview->summaryview);
	gtk_widget_set_sensitive(GTK_WIDGET(folderview->treeview), FALSE);
	inc_lock();
	main_window_cursor_wait(folderview->mainwin);

	statusbar_verbosity_set(FALSE);
	folder_item_update_freeze();
	folderview_freeze(folderview);
	if ((status = folder_item_move_to(from_folder, to_folder, &new_folder, copy)) == F_MOVE_OK) {
		statusbar_verbosity_set(FALSE);
		main_window_cursor_normal(folderview->mainwin);
		STATUSBAR_POP(folderview->mainwin);
		folder_item_update_thaw();
		folder_item_update_recursive(new_folder, F_ITEM_UPDATE_MSGCNT);
		folderview_select(folderview, new_folder);
		folderview_thaw(folderview);
	} else {
		statusbar_verbosity_set(FALSE);
		main_window_cursor_normal(folderview->mainwin);
		STATUSBAR_POP(folderview->mainwin);
		folderview_thaw(folderview);
		folder_item_update_thaw();
		switch (status) {
		case F_MOVE_FAILED_DEST_IS_PARENT:
			alertpanel_error(_("Source and destination are the same."));
			break;
		case F_MOVE_FAILED_DEST_IS_CHILD:
			alertpanel_error(copy ? _("Can't copy a folder to one of its children."):
						_("Can't move a folder to one of its children."));
			break;
		case F_MOVE_FAILED_DEST_OUTSIDE_MAILBOX:
			alertpanel_error(_("A folder cannot be moved between different mailboxes."));
			break;
		default:
			alertpanel_error(copy ? _("Copy failed!"):_("Move failed!"));
			break;
		}
	}
	inc_unlock();
	gtk_widget_set_sensitive(GTK_WIDGET(folderview->treeview), TRUE);
}

void folderview_set_target_folder_color(GdkRGBA color_op)
{
	GList *list;
	FolderView *folderview;

	debug_print("folderview_set_target_folder_color\n");

	for (list = folderview_list; list != NULL; list = list->next) {
		folderview = (FolderView *)list->data;
		folderview->color_op = color_op;
	}
}

void folderview_reinit_fonts(FolderView *folderview)
{
/* TM: not necessary (and useless, unless folderview_reflect_prefs() is called)
	debug_print("========STUB======== folderview_reinit_fonts\n");
*/
}

/* TODO: this function will eventually set normal or bold fonts,
 * text color, etc. */
static gboolean update_prefs_func(GtkTreeModel *model, GtkTreePath *path,
				  GtkTreeIter *iter, gpointer user_data)
{
	FolderView *folderview = (FolderView *)user_data;

	folderview_update_row_iter(folderview, iter, NULL);
//	gtk_tree_store_set(GTK_TREE_STORE(model), iter,
//			F_COL_FONT, folderview->normal_font,
//			-1);

	return FALSE;
}

void folderview_reflect_prefs(FolderView *folderview)
{
	gboolean update_font = FALSE;

	debug_print("folderview_reflect_prefs\n");

	cm_return_if_fail(folderview != NULL);

	/* Check if fonts have changed */
	gchar *last_normal_font =
		pango_font_description_to_string(folderview->normal_font);
	if (strcmp(last_normal_font, NORMAL_FONT))
		update_font = TRUE;
	g_free(last_normal_font);
	
	folderview->color_new = prefs_common.color[COL_NEW];
	folderview->color_op = prefs_common.color[COL_TGT_FOLDER];

	gchar *last_bold_font =
		pango_font_description_to_string(folderview->bold_font);
	if (strcmp(last_bold_font, BOLD_FONT))
		update_font = TRUE;
	g_free(last_bold_font);

// 	/* Check if color_new has changed */
// 	gint last_color_new = gtkut_convert_gdk_color_to_int(&folderview->color_new);
// 	if (last_color_new != prefs_common.color_new)
// 		update_color = TRUE;

	if (update_font) {
		debug_print("updating folderview fonts\n");
		pango_font_description_free(folderview->normal_font);
		pango_font_description_free(folderview->bold_font);
		folderview->normal_font = pango_font_description_from_string(NORMAL_FONT);
		folderview->bold_font = pango_font_description_from_string(BOLD_FONT);
	}

// 	if (update_color) {
// 		debug_print("updating folderview colors\n");
// 
// 		GdkColormap *colormap =
// 			gdk_drawable_get_colormap(gtk_widget_get_window(
// 						folderview->mainwin->window));
// 		gdk_colormap_alloc_color(colormap, &folderview->color_new, TRUE, FALSE);
// 	}

// 	if (update_font || update_color) {
	if (update_font) {
		GtkTreeModel *model =
			gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));

		/* Apply new fonts to all rows */
		gtk_tree_model_foreach(model, update_prefs_func, folderview);
	}
}

static GtkActionGroup *create_action_group(FolderView *folderview,
					   FolderViewPopup *fpopup)
{
	FolderViewPopup *fpopup_common;
	GtkActionGroup *action_group;

	action_group = cm_menu_create_action_group(
				fpopup->path,
				fpopup->entries, fpopup->n_entries,
				(gpointer)folderview);

	if (fpopup->toggle_entries)
		gtk_action_group_add_toggle_actions(action_group, fpopup->toggle_entries,
				fpopup->n_toggle_entries,
				(gpointer)folderview);
	if (fpopup->radio_entries)
		gtk_action_group_add_radio_actions(action_group, fpopup->radio_entries,
				fpopup->n_radio_entries, fpopup->radio_default,
				G_CALLBACK(fpopup->radio_callback),
				(gpointer)folderview);

	fpopup_common = g_hash_table_lookup(folderview_popups, "common");
	if (fpopup_common != fpopup) {
		gtk_action_group_add_actions(action_group, fpopup_common->entries,
				fpopup_common->n_entries,
				(gpointer)folderview);
		if (fpopup_common->toggle_entries)
			gtk_action_group_add_toggle_actions(action_group, fpopup_common->toggle_entries,
					fpopup_common->n_toggle_entries,
					(gpointer)folderview);
		if (fpopup_common->radio_entries)
			gtk_action_group_add_radio_actions(action_group, fpopup_common->radio_entries,
					fpopup_common->n_radio_entries, fpopup_common->radio_default,
					G_CALLBACK(fpopup_common->radio_callback),
					(gpointer)folderview);
	}

	return action_group;
}

static void create_action_groups(gpointer key, gpointer value, gpointer data)
{
	FolderView *folderview = data;
	FolderViewPopup *fpopup = value;
	GtkActionGroup *group;

	group = create_action_group(folderview, fpopup);
	g_hash_table_insert(folderview->popups, fpopup->klass, group);
}

void folderview_register_popup(FolderViewPopup *fpopup)
{
	GList *folderviews;

	cm_return_if_fail(fpopup != NULL);

	debug_print("folderview_register_popup %s (%s)\n",
			fpopup->klass, fpopup->path);

	for (folderviews = folderview_list; folderviews != NULL; folderviews = g_list_next(folderviews)) {
		FolderView *folderview = folderviews->data;
		GtkActionGroup *factory;

		factory = create_action_group(folderview, fpopup);
		g_hash_table_insert(folderview->popups, fpopup->klass, factory);
	}

	g_hash_table_insert(folderview_popups, fpopup->klass, fpopup);
}

void folderview_unregister_popup(FolderViewPopup *fpopup)
{
	GList *folderviews;

	cm_return_if_fail(fpopup != NULL);

	debug_print("folderview_unregister_popup %s (%s)\n",
			fpopup->klass, fpopup->path);

	for (folderviews = folderview_list; folderviews != NULL; folderviews = g_list_next(folderviews)) {
		FolderView *folderview = folderviews->data;
		g_hash_table_remove(folderview->popups, fpopup->klass);
	}

	g_hash_table_remove(folderview_popups, fpopup->klass);
}

void free_paths_to_recollapse(GSList *paths_to_recollapse)
{
	GSList *list;
	for (list=paths_to_recollapse; list; list = g_slist_next(list))
		gtk_tree_path_free(list->data);
}
static void folderview_recollapse_paths(FolderView *folderview,
					GtkTreePath *path)
{
	GSList *list = NULL;
	GSList *done = NULL;

	for (list = folderview->paths_to_recollapse; list != NULL; list = g_slist_next(list)) {
		GtkTreePath *listpath = list->data;
		if (!gtk_tree_path_is_ancestor(listpath, path)
		&&  listpath != path) {
			gtk_tree_view_collapse_row(GTK_TREE_VIEW(folderview->treeview),
						   listpath);
			done = g_slist_append(done, listpath);
			gtk_tree_path_free(listpath);
		}
	}
	for (list = done; list != NULL; list = g_slist_next(list)) {
		folderview->paths_to_recollapse = g_slist_remove(folderview->paths_to_recollapse,
								 list->data);
	}
	g_slist_free(done);
}

static void drag_state_stop(FolderView *folderview)
{
	if (folderview->drag_timer_id)
		g_source_remove(folderview->drag_timer_id);
	folderview->drag_timer_id = 0;
	if (folderview->drag_dst_path) {
		gtk_tree_path_free(folderview->drag_dst_path);
		folderview->drag_dst_path = NULL;
	}
}

static gboolean folderview_defer_expand(FolderView *folderview)
{
	if (folderview->drag_dst_path) {
		folderview_recollapse_paths(folderview, folderview->drag_dst_path);
		if (folderview->drag_dst_item->collapsed) {
			gtk_tree_view_expand_row(GTK_TREE_VIEW(folderview->treeview),
						 folderview->drag_dst_path, FALSE);
			/* reverse order: child doesn't get closed, if parent is already closed. */
			folderview->paths_to_recollapse = g_slist_insert
				(folderview->paths_to_recollapse,
				 gtk_tree_path_copy(folderview->drag_dst_path), 0);
		}
	}
	folderview->drag_dst_item  = NULL;
	folderview->drag_timer_id = 0;
	return FALSE;
}

static void drag_state_start(FolderView *folderview, GtkTreePath *path, FolderItem *item)
{
	/* the idea is that we call drag_state_start() whenever we want expansion to
	 * start after 'prefs_common.hover_time' msecs. if we want to cancel expansion,
	 * we need to call drag_state_stop() */
	drag_state_stop(folderview);
	/* request expansion */
	if (0 != (folderview->drag_timer_id = g_timeout_add
			(prefs_common.hover_timeout,
			 (GSourceFunc)folderview_defer_expand,
			 folderview))) {
		/* free path from earlier folderview_recollapse_paths() invocations */
		if (folderview->drag_dst_path)
			gtk_tree_path_free(folderview->drag_dst_path);
		folderview->drag_dst_path = path;
		folderview->drag_dst_item = item;
	}
}

#ifndef GENERIC_UMPC
/* only called for dragged treeview nodes, not for summaryview items */
static void folderview_drag_begin(GtkWidget *widget, GdkDragContext *context,
			          FolderView *folderview)
{
	free_paths_to_recollapse(folderview->paths_to_recollapse);
	if (folderview->paths_to_recollapse)
		g_slist_free(folderview->paths_to_recollapse);
	folderview->paths_to_recollapse = NULL;
	gtk_drag_set_icon_default(context);
}
#endif
/* only called for dragged treeview nodes, not for summaryview items */
static void folderview_drag_data_get(GtkWidget        *widget,
				     GdkDragContext   *drag_context,
				     GtkSelectionData *selection_data,
				     guint             info,
				     guint             time,
				     FolderView       *folderview)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	FolderItem *item;
	if (info == TARGET_DUMMY) {
		if (!folderview->open_path)
			return;

		model = gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));
		if (!model)
			return;

		gtk_tree_model_get_iter(model, &iter, folderview->open_path);
		gtk_tree_model_get(model, &iter, F_COL_FOLDERITEM, &item, -1);
		if (item) {
			gchar *source = NULL;
			gchar *name = folder_item_get_identifier(item);
			source = g_strdup_printf ("FROM_OTHER_FOLDER%s", name);
			g_free(name);
			gtk_selection_data_set(selection_data,
					       gtk_selection_data_get_target(selection_data), 8,
					       source, strlen(source));
		}
	} else {
		g_warning("unknown info %d", info);
	}
}

static gboolean folderview_dnd_scroll_cb(gpointer data)
{
	FolderView *folderview = (FolderView *)data;
	GtkAdjustment *pos = gtk_scrolled_window_get_vadjustment(
				GTK_SCROLLED_WINDOW(folderview->scrolledwin));
	gint new_val = (int)gtk_adjustment_get_value(pos) + folderview->scroll_value;
	gint max = (int)gtk_adjustment_get_upper(pos) -
               (int)gtk_adjustment_get_page_size(pos);

	if (folderview->scroll_value == 0) {
		folderview->scroll_timeout_id = 0;
		return FALSE;
	}

	if (folderview->scroll_value > 0 && new_val > max) {
		new_val = max;
	} else if (folderview->scroll_value < 0 && new_val < 0) {
		new_val = 0;
	}
	gtk_adjustment_set_value(pos, new_val);

	return TRUE;
}

/* called for dragged treeview nodes and summaryview items */
static gboolean folderview_drag_motion_cb(GtkWidget      *widget,
					  GdkDragContext *context,
					  gint            x,
					  gint            y,
					  guint           time,
					  FolderView     *folderview)
{
	GtkTreePath *path;
	GtkTreeViewDropPosition drop_pos;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));
	FolderItem *item = NULL, *src_item = NULL;
	gboolean acceptable = FALSE;
	GtkAdjustment *pos = gtk_scrolled_window_get_vadjustment(
				GTK_SCROLLED_WINDOW(folderview->scrolledwin));
	int height = (int)gtk_adjustment_get_page_size(pos);
	int total_height = (int)gtk_adjustment_get_upper(pos);
	int vpos = (int)gtk_adjustment_get_value(pos);
	int offset = prefs_common.show_col_headers ? 24:0;
	int dist;

	gtk_tree_view_get_dest_row_at_pos(GTK_TREE_VIEW(folderview->treeview),
					  x, y, &path, &drop_pos);
	if (path) {
		GtkWidget *srcwidget;

		if (y > height - (48 - offset) && height + vpos < total_height) {
			dist = -(height - (48 - offset) - y);
			folderview->scroll_value = 1.41f * (1+(dist / 6));
		} else if (y < 72 - (24 - offset) && y >= 0) {
			dist = 72 - (24 - offset) - y;
			folderview->scroll_value = -1.41f * (1+(dist / 6));
		} else {
			folderview->scroll_value = 0;
		}
		if (folderview->scroll_value != 0 && folderview->scroll_timeout_id == 0) {
			folderview->scroll_timeout_id =
				g_timeout_add(30, folderview_dnd_scroll_cb,
					      folderview);
		}

		gtk_tree_model_get_iter(model, &iter, path);
		gtk_tree_model_get(model, &iter, F_COL_FOLDERITEM, &item, -1);
		src_item = folderview->summaryview->folder_item;

		srcwidget = gtk_drag_get_source_widget(context);
		if (srcwidget == summary_get_main_widget(folderview->summaryview)) {
			/* comes from summaryview */
			/* we are copying messages, so only accept folder items that are not
			   the source item, are no root items and can copy messages */
			if (item && item->folder && folder_item_parent(item) != NULL && src_item &&
			    src_item != item && FOLDER_CLASS(item->folder)->copy_msg != NULL &&
			    FOLDER_TYPE(item->folder) != F_UNKNOWN)
				acceptable = TRUE;
		} else if (srcwidget == folderview->treeview) {
			/* comes from folderview */
			/* we are moving folder items, only accept folders that are not
                           the source items and can copy messages and create folder items */
			if (item && item->folder && src_item && src_item != item &&
			    FOLDER_CLASS(item->folder)->copy_msg != NULL &&
			    FOLDER_CLASS(item->folder)->create_folder != NULL &&
			    ((FOLDER_TYPE(item->folder) != F_UNKNOWN &&  FOLDER_TYPE(src_item->folder) != F_UNKNOWN)
			     || item->folder == src_item->folder))
				acceptable = TRUE;
		} else {
			/* comes from another app */
			/* we are adding messages, so only accept folder items that are
			   no root items and can copy messages */
			if (item && item->folder && folder_item_parent(item) != NULL
			    && FOLDER_CLASS(item->folder)->add_msg != NULL &&
			    FOLDER_TYPE(item->folder) != F_UNKNOWN)
				acceptable = TRUE;
		}
	}

	if (acceptable || (src_item && src_item == item))
		drag_state_start(folderview, path, item);

	if (acceptable) {
//TM:		g_signal_handlers_block_by_func
//			(G_OBJECT(widget),
//			 G_CALLBACK(folderview_selected), folderview);
//		gtk_cmctree_select(GTK_CMCTREE(widget), node);
		gtk_tree_view_set_cursor(GTK_TREE_VIEW(folderview->treeview),
					 path, NULL, FALSE);
//		g_signal_handlers_unblock_by_func
//			(G_OBJECT(widget),
//			 G_CALLBACK(folderview_selected), folderview);
		gdk_drag_status(context,
					(gdk_drag_context_get_actions(context) == GDK_ACTION_COPY ?
					GDK_ACTION_COPY : GDK_ACTION_MOVE) , time);
	} else {
//TM:		if (folderview->opened)
//			gtk_cmctree_select(GTK_CMCTREE(widget), folderview->opened);
		gdk_drag_status(context, 0, time);
	}

	return acceptable;
}

/* called for dragged treeview nodes and summaryview items
 * TM: for dragged folders, folderview_drag_data_get() and folderview_drag_end_cb() are called hereafter
 *     for dragged messages, folderview_drag_received_cb() is called hereafter */
static void folderview_drag_leave_cb(GtkWidget      *widget,
				     GdkDragContext *context,
				     guint           time,
				     FolderView     *folderview)
{
/* TM: unnecessary
	debug_print("========STUB======== folderview_drag_leave_cb\n");
*/
}

static void free_info (gpointer stuff, gpointer data)
{
	g_free(stuff);
}

/* only for TARGET_MAIL_URI_LIST */
void folderview_finish_dnd(const gchar *data, GdkDragContext *drag_context,
			   guint time, FolderItem *item)
{
	GList *list, *tmp;
	GSList *msglist = NULL;
	list = uri_list_extract_filenames(data);
	if (!(item && item->folder && folder_item_parent(item) != NULL
		    && FOLDER_CLASS(item->folder)->add_msg != NULL))
	{
		gtk_drag_finish(drag_context, FALSE, FALSE, time);
		debug_print("item doesn't fit\n");
		return;
	}
	if (!list) {
		gtk_drag_finish(drag_context, FALSE, FALSE, time);
		debug_print("list is empty\n");
		return;
	}
	for (tmp = list; tmp != NULL; tmp = tmp->next) {
		MsgFileInfo *info = NULL;

		if (file_is_email((gchar *)tmp->data)) {
			info = g_new0(MsgFileInfo, 1);
			info->msginfo = NULL;
			info->file = (gchar *)tmp->data;
			msglist = g_slist_prepend(msglist, info);
			debug_print("file is a mail\n");
		} else {
			debug_print("file isn't a mail\n");
		}
	}
	if (msglist) {
		msglist = g_slist_reverse(msglist);
		folder_item_add_msgs(item, msglist, FALSE);
		g_slist_foreach(msglist, free_info, NULL);
		g_slist_free(msglist);
		gtk_drag_finish(drag_context, TRUE, FALSE, time);
	} else {
		gtk_drag_finish(drag_context, FALSE, FALSE, time);
	}
	list_free_strings_full(list);
}

/* called for dragged treeview nodes and summaryview items */
static void folderview_drag_received_cb(GtkWidget        *widget,
					GdkDragContext   *drag_context,
					gint              x,
					gint              y,
					GtkSelectionData *data,
					guint             info,
					guint             time,
					FolderView       *folderview)
{
	FolderItem *item = NULL, *src_item;
	GtkTreePath *path;
	GtkTreeModel *model;
	GtkTreeIter iter;

	folderview->scroll_value = 0;

	gtk_tree_view_get_cursor(GTK_TREE_VIEW(widget), &path, NULL);
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
	if (!model)
		return;

	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get(model, &iter, F_COL_FOLDERITEM, &item, -1);
	if (!item)
		return;

	if (info == TARGET_DUMMY) {
		drag_state_stop(folderview);
		const gchar *ddata = (const gchar *)gtk_selection_data_get_data(data);
		if ((gchar *)strstr(ddata, "FROM_OTHER_FOLDER") != ddata) {
			/* comes from summaryview */
			src_item = folderview->summaryview->folder_item;

			if (item && item->no_select) {
				alertpanel_error(_("The destination folder can only be used to "
						   "store subfolders."));
				return;
			}
			/* re-check (due to acceptable possibly set for folder moves */
			if (!(item && item->folder && item->path && !item->no_select &&
			      src_item && src_item != item && FOLDER_CLASS(item->folder)->copy_msg != NULL)) {
				return;
			}

			switch (gdk_drag_context_get_selected_action(drag_context)) {
			case GDK_ACTION_COPY:
				summary_copy_selected_to(folderview->summaryview, item);
				gtk_drag_finish(drag_context, TRUE, FALSE, time);
				break;
			case GDK_ACTION_MOVE:
			case GDK_ACTION_DEFAULT:
			default:
				if (FOLDER_CLASS(src_item->folder)->remove_msg == NULL)
					summary_copy_selected_to(folderview->summaryview, item);
				else
					summary_move_selected_to(folderview->summaryview, item);
				gtk_drag_finish(drag_context, TRUE, TRUE, time);
			}
			/* TM: drop done - cleanup */
			drag_state_stop(folderview);
			folderview->scroll_value = 0;
			if (folderview->open_path) {
				gtk_tree_view_set_cursor
					(GTK_TREE_VIEW(folderview->treeview),
					 folderview->open_path, NULL, FALSE);
				summary_grab_focus(folderview->summaryview);
			}
		} else {
			/* comes from folderview */
			char *source;
			gboolean folder_is_normal = TRUE;
			gboolean copy = (GDK_ACTION_COPY ==
				gdk_drag_context_get_selected_action(drag_context));

			source = (char *)gtk_selection_data_get_data(data) + 17;
			src_item = folder_find_item_from_identifier(source);
			folder_is_normal =
				src_item != NULL &&
				src_item->stype == F_NORMAL &&
				!folder_has_parent_of_type(src_item, F_OUTBOX) &&
				!folder_has_parent_of_type(src_item, F_DRAFT) &&
				!folder_has_parent_of_type(src_item, F_QUEUE) &&
				!folder_has_parent_of_type(src_item, F_TRASH);
			if (!item || !src_item || !folder_is_normal) {
				gtk_drag_finish(drag_context, FALSE, FALSE, time);
				return;
			}

			folderview_move_folder(folderview, src_item, item, copy);
			gtk_drag_finish(drag_context, TRUE, TRUE, time);
		}
		if (folderview->paths_to_recollapse) {
			free_paths_to_recollapse(folderview->paths_to_recollapse);
			g_slist_free(folderview->paths_to_recollapse);
			folderview->paths_to_recollapse = NULL;
		}
	} else if (info == TARGET_MAIL_URI_LIST) {
		if (!item) {
			gtk_drag_finish(drag_context, FALSE, FALSE, time);
			debug_print("no item\n");
			return;
		}
		folderview_finish_dnd(gtk_selection_data_get_data(data),
			drag_context, time, item);
	}
}

/* Only called for dragged treeview nodes, not for summaryview items */
static void folderview_drag_end_cb(GtkWidget	    *widget,
				   GdkDragContext   *drag_context,
                                   FolderView	    *folderview)
{
	drag_state_stop(folderview);
	folderview->scroll_value = 0;
	if (folderview->paths_to_recollapse) {
		free_paths_to_recollapse(folderview->paths_to_recollapse);
		g_slist_free(folderview->paths_to_recollapse);
		folderview->paths_to_recollapse = NULL;
	}
}


void folderview_update_search_icon(FolderItem *item, gboolean matches)
{
	GList *list;
	FolderView *folderview;
	GtkTreeView *treeview;
	GtkTreeModel *model;

	struct finditer_ctx *ctx = g_new0(struct finditer_ctx, 1);
	ctx->item = item;
	ctx->iter = NULL;

	g_return_if_fail(item != NULL);

	for (list = folderview_list; list != NULL; list = list->next) {
		folderview = (FolderView *)list->data;
		treeview = GTK_TREE_VIEW(folderview->treeview);
		model = gtk_tree_view_get_model(treeview);
		if (!model)
			continue;

		gtk_tree_model_foreach(model, folderview_find_iter_foreach_func, ctx);
		if (ctx->item) {
			ctx->item->search_match = matches;
			folderview_update_row(folderview, ctx->item);
			gtk_tree_iter_free(ctx->iter);
		}
	}
	g_free(ctx);
}

void folderview_set_column_order(FolderView *folderview)
{
	gint i;
	GtkTreeViewColumn *default_column;
	GtkTreeViewColumn *prev = NULL;
	FolderColumnState *col_state;

	/* apply new order
	 * col_state[i].type, when used as an index to the default_columns,
	 * points to the column, that should be displayed at position "i"
	 */
	col_state = prefs_folder_column_get_config();
	prev = NULL;
	for (i = F_COL_FOLDER; i <= F_COL_TOTAL; i++) {
		gint newtype = col_state[i].type;
		default_column = (GtkTreeViewColumn*)g_list_nth_data
				(treeview_default_columns_ordered,
				 newtype - F_COL_FOLDER);
		gtk_tree_view_column_set_visible(default_column,
						 col_state[i].visible);
		if (prev)
			gtk_tree_view_move_column_after
				(GTK_TREE_VIEW(folderview->treeview),
				 default_column, prev);
		prev = default_column;
	}
}

void folderview_close_opened(FolderView *folderview, gboolean dirty)
{
	debug_print("folderview_close_opened\n");

	if (folderview->open_item == NULL)
		return;

	FolderItem *item = folderview->open_item;

	if (!dirty) {
		gchar *buf = g_strdup_printf(_("Closing folder %s..."), item->name);
		STATUSBAR_PUSH(folderview->mainwin, buf);
		g_free(buf);
		main_window_cursor_wait(folderview->mainwin);

		summary_save_prefs_to_folderitem(folderview->summaryview, item);
		summary_show(folderview->summaryview, NULL, FALSE);
		folder_item_close(item);

		main_window_cursor_normal(folderview->mainwin);
		STATUSBAR_POP(folderview->mainwin);

		if (item->folder->klass->item_closed)
			item->folder->klass->item_closed(item);
	}

	folderview->open_item = NULL;
}

void folderview_remove_item(FolderView *folderview, FolderItem *item)
{
	struct finditer_ctx *ctx = g_new0(struct finditer_ctx, 1);
	ctx->item = item;
	ctx->iter = NULL;
	GtkTreeModel *model =
		gtk_tree_view_get_model(GTK_TREE_VIEW(folderview->treeview));

	if (!model)
		return;

	gtk_tree_model_foreach(model, folderview_find_iter_foreach_func, ctx);

	if (ctx->iter == NULL) {
		g_free(ctx);
		return;
	}

	gtk_tree_store_remove(GTK_TREE_STORE(model), ctx->iter);
	gtk_tree_iter_free(ctx->iter);
	g_free(ctx);
}

/* TM: There is no freeze/thaw. Detaching the model can be a workaround,
 * but that doesn't allow changes to the treeview (only direct changes to
 * the datastore are possible then). */
void folderview_freeze(FolderView *folderview)
{
	debug_print("========STUB======== folderview_freeze\n");
}

void folderview_thaw(FolderView *folderview)
{
	debug_print("========STUB======== folderview_thaw\n");
}

void folderview_grab_focus(FolderView *folderview)
{
	debug_print("folderview_grab_focus\n");
	cm_return_if_fail(folderview != NULL);
	cm_return_if_fail(folderview->treeview != NULL);

	gtk_widget_grab_focus(folderview->treeview);
}

gboolean folderview_is_drag_source(FolderView *folderview,
				   GdkDragContext *context)
{
	g_return_val_if_fail(context != NULL, FALSE);

	if (gtk_drag_get_source_widget(context) == folderview->treeview)
		return TRUE;

	return FALSE;
}

static gboolean folder_has_children_sub(FolderItem *item, gboolean in_sub)
{
	GNode *node = NULL;

	if (!item || !item->folder || !item->folder->node)
		return FALSE;

	node = item->folder->node;

	node = g_node_find(node, G_PRE_ORDER, G_TRAVERSE_ALL, item);
 	node = node->children;

	if (in_sub && item->total_msgs > 0) {
		return TRUE;
	}

	while (node != NULL) {
		if (node && node->data) {
			FolderItem *next_item = (FolderItem*) node->data;
			node = node->next;
			if (folder_has_children_sub(next_item, TRUE))
				return TRUE;
		}
	}

	return FALSE;
}

static gboolean folder_has_children(FolderItem *item)
{
	return folder_has_children_sub(item, FALSE);
}

static gboolean folder_has_new_children_sub(FolderItem *item, gboolean in_sub)
{
	GNode *node = NULL;

	if (!item || !item->folder || !item->folder->node)
		return FALSE;

	node = item->folder->node;

	node = g_node_find(node, G_PRE_ORDER, G_TRAVERSE_ALL, item);
	node = node->children;

	if (in_sub &&
	    (item->new_msgs > 0 ||
	    (folder_has_parent_of_type(item, F_QUEUE) && item->total_msgs > 0))) {
		return TRUE;
	}

	while (node != NULL) {
		if (node && node->data) {
			FolderItem *next_item = (FolderItem*) node->data;
			node = node->next;
			if (folder_has_new_children_sub(next_item, TRUE))
				return TRUE;
		}
	}

	return FALSE;
}

static gboolean folder_has_new_children(FolderItem *item)
{
	return folder_has_new_children_sub(item, FALSE);
}

static gboolean folder_has_unread_children_sub(FolderItem *item,
					       gboolean in_sub)
{
	GNode *node = NULL;

	if (!item || !item->folder || !item->folder->node)
		return FALSE;

	node = item->folder->node;

	node = g_node_find(node, G_PRE_ORDER, G_TRAVERSE_ALL, item);
	node = node->children;

	if (in_sub &&
	    (item->unread_msgs > 0 ||
	    (folder_has_parent_of_type(item, F_QUEUE) && item->total_msgs > 0))) {
		return TRUE;
	}

	while (node != NULL) {
		if (node && node->data) {
			FolderItem *next_item = (FolderItem*) node->data;
			node = node->next;
			if (folder_has_unread_children_sub(next_item, TRUE))
				return TRUE;
		}
	}

	return FALSE;
}

static gboolean folder_has_unread_children(FolderItem *item)
{
	return folder_has_unread_children_sub(item, FALSE);
}

static gboolean folder_has_read_children_sub(FolderItem *item, gboolean in_sub)
{
	GNode *node = NULL;

	if (!item || !item->folder || !item->folder->node) {
		return FALSE;
	}			

	node = item->folder->node;

	node = g_node_find(node, G_PRE_ORDER, G_TRAVERSE_ALL, item);
	node = node->children;

	if (in_sub &&
	    (((item->total_msgs > 0) &&
		(item->unread_msgs != (item->total_msgs - item->ignored_msgs))))) {
		return TRUE;
	}

	while (node != NULL) {
		if (node && node->data) {
			FolderItem *next_item = (FolderItem*) node->data;
			node = node->next;
			if (folder_has_read_children_sub(next_item, TRUE))
				return TRUE;
		}
	}

	return FALSE;
}

static gboolean folder_has_read_children(FolderItem *item)
{
	return folder_has_read_children_sub(item, FALSE);
}

static gboolean folder_has_matching_children_sub(FolderItem *item,
						 gboolean in_sub)
{
	GNode *node = NULL;

	if (!item || !item->folder || !item->folder->node)
		return FALSE;

	node = item->folder->node;

	node = g_node_find(node, G_PRE_ORDER, G_TRAVERSE_ALL, item);
	node = node->children;

	if (in_sub && item->search_match){
		return TRUE;
	}

	while (node != NULL) {
		if (node && node->data) {
			FolderItem *next_item = (FolderItem*) node->data;
			node = node->next;
			if (folder_has_matching_children_sub(next_item, TRUE))
				return TRUE;
		}
	}

	return FALSE;
}

static gboolean folder_has_matching_children(FolderItem *item)
{
	return folder_has_matching_children_sub(item, FALSE);
}

static gboolean folder_has_marked_children_sub(FolderItem *item,
					       gboolean in_sub)
{
	GNode *node = NULL;
	
	if (!item || !item->folder || !item->folder->node)
		return FALSE;

	node = item->folder->node;

	node = g_node_find(node, G_PRE_ORDER, G_TRAVERSE_ALL, item);
	node = node->children;

	if (item->marked_msgs != 0) {
		return TRUE;
	}

	while (node != NULL) {
		if (node && node->data) {
			FolderItem *next_item = (FolderItem*) node->data;
			node = node->next;
			if (folder_has_marked_children_sub(next_item, TRUE))
				return TRUE;
		}
	}

	return FALSE;
}

static gboolean folder_has_marked_children(FolderItem *item)
{
	return folder_has_marked_children_sub(item, FALSE);
}

/*
 * Handlers for popup menu.
 */

static void folderview_popup_close(GtkMenuShell *menu_shell,
				   FolderView *folderview)
{
	debug_print("folderview_popup_close\n");
	folderview->menu_item = NULL;
	if (folderview->open_path)
		gtk_tree_view_set_cursor(GTK_TREE_VIEW(folderview->treeview), folderview->open_path, NULL, FALSE);
}

static void set_sens_and_popup_menu(FolderView *folderview, FolderItem *item,
				    GdkEventButton *event)
{
	GtkUIManager *ui_manager = gtk_ui_manager_new();
	GtkActionGroup *action_group;
	FolderViewPopup *fpopup;
	GtkWidget *popup;
	PrefsAccount *ac;
	FolderItem *special_trash = NULL, *special_queue = NULL;

	cm_return_if_fail(item != NULL);
	cm_return_if_fail(item->folder != NULL);

	debug_print("popup menu for folder '%s'\n", item->name);

	Folder *folder = item->folder;

	fpopup = g_hash_table_lookup(folderview_popups, folder->klass->idstr);
	if (fpopup != NULL)
		action_group = g_hash_table_lookup(folderview->popups, folder->klass->idstr);
	else {
		fpopup = g_hash_table_lookup(folderview_popups, "common");
		action_group = g_hash_table_lookup(folderview->popups, "common");
	}

	gtk_ui_manager_insert_action_group(ui_manager, action_group, 0);
	MENUITEM_ADDUI_MANAGER(ui_manager, "/", "Popup", "Popup", GTK_UI_MANAGER_MENUBAR)
	MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup", "FolderViewPopup", "FolderViewPopup", GTK_UI_MANAGER_MENU)

	if (fpopup->add_menuitems)
		fpopup->add_menuitems(ui_manager, item);

	MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "MarkAllRead", "FolderViewPopup/MarkAllRead", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "MarkAllUnread", "FolderViewPopup/MarkAllUnread", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "MarkAllReadRec", "FolderViewPopup/MarkAllReadRec", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "MarkAllUnreadRec", "FolderViewPopup/MarkAllUnreadRec", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "Separator1", "FolderViewPopup/---", GTK_UI_MANAGER_SEPARATOR)
	MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "RunProcessing", "FolderViewPopup/RunProcessing", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "SearchFolder", "FolderViewPopup/SearchFolder", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "Properties", "FolderViewPopup/Properties", GTK_UI_MANAGER_MENUITEM)
	MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "Processing", "FolderViewPopup/Processing", GTK_UI_MANAGER_MENUITEM)

	if (fpopup->set_sensitivity != NULL)
		fpopup->set_sensitivity(ui_manager, item);

	if (NULL != (ac = account_find_from_item(item))) {
		special_trash = account_get_special_folder(ac, F_TRASH);
		special_queue = account_get_special_folder(ac, F_QUEUE);
	}

	if ((item == folder->trash || item == special_trash
	     || folder_has_parent_of_type(item, F_TRASH))) {
		MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "SeparatorTrash", "FolderViewPopup/---", GTK_UI_MANAGER_SEPARATOR)
		MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "EmptyTrash", "FolderViewPopup/EmptyTrash", GTK_UI_MANAGER_MENUITEM)
	} 

	if ((item == folder->queue || item == special_queue
	     || folder_has_parent_of_type(item, F_QUEUE))) {
		MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "SeparatorQueue", "FolderViewPopup/---", GTK_UI_MANAGER_SEPARATOR)
		MENUITEM_ADDUI_MANAGER(ui_manager, "/Popup/FolderViewPopup", "SendQueue", "FolderViewPopup/SendQueue", GTK_UI_MANAGER_MENUITEM)
	} 

#define SET_SENS(name, sens) \
	cm_menu_set_sensitive_full(ui_manager, "Popup/"name, sens)

	SET_SENS("FolderViewPopup/MarkAllRead", item->unread_msgs > 0);
	SET_SENS("FolderViewPopup/MarkAllUnread", (item->total_msgs > 0) &&
		(item->unread_msgs != (item->total_msgs - item->ignored_msgs)));
	SET_SENS("FolderViewPopup/MarkAllReadRec", folder_has_unread_children(item));
	SET_SENS("FolderViewPopup/MarkAllUnreadRec", folder_has_read_children(item));
	SET_SENS("FolderViewPopup/SearchFolder", item->total_msgs > 0 && 
		 item == folderview->open_item);
	SET_SENS("FolderViewPopup/Properties", TRUE);

	SET_SENS("FolderViewPopup/RunProcessing", item->prefs->processing &&
		 item->total_msgs >= 1 && !item->processing_pending);
	SET_SENS("FolderViewPopup/Processing", item->node->parent != NULL && 
		!item->no_select && !item->processing_pending);

	if (item == folder->trash || item == special_trash
	    || folder_has_parent_of_type(item, F_TRASH)) {
		GSList *msglist = folder_item_get_msg_list(item);
		SET_SENS("FolderViewPopup/EmptyTrash", msglist != NULL);
		procmsg_msg_list_free(msglist);
	}
	if (item == folder->queue || item == special_queue
	    || folder_has_parent_of_type(item, F_QUEUE)) {
		GSList *msglist = folder_item_get_msg_list(item);
		SET_SENS("FolderViewPopup/SendQueue", msglist != NULL);
		procmsg_msg_list_free(msglist);
	}
#undef SET_SENS

	folderview->menu_item = item;

	popup = gtk_menu_item_get_submenu(GTK_MENU_ITEM(
			gtk_ui_manager_get_widget(ui_manager, "/Popup/FolderViewPopup")) );
        g_signal_connect(G_OBJECT(popup), "selection_done",
                         G_CALLBACK(folderview_popup_close),
                         folderview);
	gtk_menu_popup_at_pointer(GTK_MENU(popup), NULL);
}

static void folderview_send_queue_cb(GtkAction *action, gpointer data)
{
	FolderView *folderview = (FolderView *)data;
	FolderItem *item;
	FolderItem *special_queue = NULL;
	PrefsAccount *ac;
	gchar *errstr = NULL;

	item = folderview->menu_item;
	cm_return_if_fail(item != NULL);
	cm_return_if_fail(item->folder != NULL);

	if (NULL != (ac = account_find_from_item(item)))
		special_queue = account_get_special_folder(ac, F_QUEUE);

	if (item != item->folder->queue && item != special_queue
	&&  !folder_has_parent_of_type(item, F_QUEUE)) return;

	if (procmsg_queue_is_empty(item))
		return;

	if (prefs_common.work_offline)
		if (alertpanel(_("Offline warning"), 
			       _("You're working offline. Override?"),
			       NULL, _("_No"), NULL, _("_Yes"),
			       NULL, NULL, ALERTFOCUS_FIRST) != G_ALERTALTERNATE)
		return;

	/* ask for confirmation before sending queued messages only
	   in online mode and if there is at least one message queued
	   in any of the folder queue
	*/
	if (prefs_common.confirm_send_queued_messages) {
		if (!prefs_common.work_offline) {
			if (alertpanel(_("Send queued messages"), 
			    	   _("Send all queued messages?"),
			    	   NULL, _("_Cancel"), NULL, _("_Send"),
				   NULL, NULL, ALERTFOCUS_FIRST) != G_ALERTALTERNATE)
				return;
		}
	}

	if (procmsg_send_queue(item, prefs_common.savemsg, &errstr) < 0) {
		if (!errstr)
			alertpanel_error_log(_("Some errors occurred while "
					   "sending queued messages."));
		else {
			alertpanel_error_log(_("Some errors occurred "
					"while sending queued messages:\n%s"), errstr);
			g_free(errstr);
		}
	}
}

static void folderview_empty_trash_cb(GtkAction *action, gpointer data)
{
	FolderView *folderview = (FolderView *)data;
	FolderItem *item;
	GSList *mlist = NULL;
	GSList *cur = NULL;
	FolderItem *special_trash = NULL;
	PrefsAccount *ac;

	item = folderview->menu_item;
	cm_return_if_fail(item != NULL);
	cm_return_if_fail(item->folder != NULL);

	if (NULL != (ac = account_find_from_item(item)))
		special_trash = account_get_special_folder(ac, F_TRASH);

	if (item != item->folder->trash && item != special_trash
	&&  !folder_has_parent_of_type(item, F_TRASH)) return;

	if (prefs_common.ask_on_clean) {
		if (alertpanel(_("Empty trash"),
			       _("Delete all messages in trash folders?"),
			       NULL, _("_Cancel"), NULL, _("_Empty trash"), NULL, NULL,
				ALERTFOCUS_SECOND) != G_ALERTALTERNATE)
			return;
	}

	mlist = folder_item_get_msg_list(item);

	for (cur = mlist ; cur != NULL ; cur = cur->next) {
		MsgInfo * msginfo = (MsgInfo *) cur->data;
		if (MSG_IS_LOCKED(msginfo->flags))
			continue;
		/* is it partially received? (partial_recv isn't cached) */
		if (msginfo->total_size != 0 &&
		    msginfo->size != (off_t)msginfo->total_size)
			partial_mark_for_delete(msginfo);
	}
	procmsg_msg_list_free(mlist);

	folder_item_remove_all_msg(item);
}

static void folderview_run_processing_cb(GtkAction *action, gpointer data)
{
	FolderView *folderview = (FolderView *)data;
	FolderItem *item;

	item = folderview->menu_item;
	cm_return_if_fail(item != NULL);
	cm_return_if_fail(item->folder != NULL);

	item->processing_pending = TRUE;
	folder_item_apply_processing(item);
	item->processing_pending = FALSE;
}

void folderview_run_processing(FolderItem *item)
{
	cm_return_if_fail(item != NULL);
	cm_return_if_fail(item->folder != NULL);

	item->processing_pending = TRUE;
	folder_item_apply_processing(item);
	item->processing_pending = FALSE;
}

static void folderview_processing_cb(GtkAction *action, gpointer data)
{
	FolderView *folderview = (FolderView *)data;
	FolderItem *item;
	gchar *id, *title;

	item = folderview->menu_item;
	cm_return_if_fail(item != NULL);
	cm_return_if_fail(item->folder != NULL);

	id = folder_item_get_identifier(item);
	title = g_strdup_printf (_("Processing configuration for folder %s"), id);
	g_free (id);

	prefs_filtering_open(&item->prefs->processing, title,
			MANUAL_ANCHOR_PROCESSING, NULL, NULL, FALSE);
	g_free (title);
}

static void folderview_property_cb(GtkAction *action, gpointer data)
{
	FolderView *folderview = (FolderView *)data;
	FolderItem *item;
	item = folderview->menu_item;
	cm_return_if_fail(item != NULL);
	cm_return_if_fail(item->folder != NULL);

	prefs_folder_item_open(item);
}

static void folderview_search_cb(GtkAction *action, gpointer data)
{
	FolderView *folderview = (FolderView *)data;
	summary_search(folderview->summaryview);
}

static void mark_all_read_unread_handler(GtkAction *action, gpointer data,
					 gboolean recursive, gboolean read)
{
	FolderView *folderview = (FolderView *)data;
	FolderItem *item;
	AlertValue val;
	gchar *message;
	gchar *title;

	item = folderview->menu_item;
	if (item == NULL)
		return;

	if (read) {
		title = _("Mark all as read");
		message = recursive? _("Do you really want to mark all mails in this "
							"folder and its sub-folders as read?") :
							_("Do you really want to mark all mails in this "
							"folder as read?");
	} else {
		title = _("Mark all as unread");
		message = recursive? _("Do you really want to mark all mails in this "
							"folder and its sub-folders as unread?") :
							_("Do you really want to mark all mails in this "
							"folder as unread?");
	}
	if (prefs_common.ask_mark_all_read) {
		val = alertpanel_full(title, message,
				      NULL, _("_No"), NULL, _("_Yes"), NULL, NULL,
				      ALERTFOCUS_FIRST, TRUE, NULL, ALERT_QUESTION);

		if ((val & ~G_ALERTDISABLE) != G_ALERTALTERNATE)
			return;
		else if (val & G_ALERTDISABLE)
			prefs_common.ask_mark_all_read = FALSE;
	}

	folder_item_update_freeze();
	if (folderview->summaryview->folder_item != item && !recursive)
		summary_lock(folderview->summaryview);
	else
		summary_freeze(folderview->summaryview);

	if (read) {
		if (recursive)
			folderutils_mark_all_read_recursive(item, TRUE);
		else {
			if (prefs_common.run_processingrules_before_mark_all)
				folderview_run_processing(item);
			folderutils_mark_all_read(item, TRUE);
		}
	} else {
		if (recursive)
			folderutils_mark_all_read_recursive(item, FALSE);
		else {
			folderutils_mark_all_read(item, FALSE);
			if (prefs_common.run_processingrules_before_mark_all)
				folderview_run_processing(item);
		}
	}
	if (folderview->summaryview->folder_item != item && !recursive)
		summary_unlock(folderview->summaryview);
	else
		summary_thaw(folderview->summaryview);
	folder_item_update_thaw();
}

static void mark_all_read_cb(GtkAction *action, gpointer data)
{
	mark_all_read_unread_handler(action, data, FALSE, TRUE);
}

static void mark_all_unread_cb(GtkAction *action, gpointer data)
{
	mark_all_read_unread_handler(action, data, FALSE, FALSE);
}

static void mark_all_read_recursive_cb(GtkAction *action, gpointer data)
{
	mark_all_read_unread_handler(action, data, TRUE, TRUE);
}

static void mark_all_unread_recursive_cb(GtkAction *action, gpointer data)
{
	mark_all_read_unread_handler(action, data, TRUE, FALSE);
}

static void folderview_header_set_displayed_columns_cb(GtkAction *gaction,
		gpointer data)
{
	prefs_folder_column_open();
}

static gboolean folderview_header_button_pressed(GtkWidget *widget,
						 GdkEvent *orgevent,
						 gpointer user_data)
{
	GdkEventButton *event = (GdkEventButton *)orgevent;
	FolderView *folderview = (FolderView *)user_data;

	cm_return_val_if_fail(folderview != NULL, FALSE);

	/* Only handle single button presses. */
	if (event->type == GDK_2BUTTON_PRESS ||
			event->type == GDK_3BUTTON_PRESS)
		return FALSE;

	/* Handle right-click for context menu */
	if (event->button == 3) {
		gtk_menu_popup_at_pointer(GTK_MENU(folderview->headerpopupmenu), NULL);
		return TRUE;
	}

	return FALSE;
}
