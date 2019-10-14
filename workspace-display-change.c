#include <stdio.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE

#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include "debug.h"
#include "workspace-change-manager.h"

static GtkWidget* build_status_icon_context_menu() {
  
  //TODO Denis : take care of freeing memory of menu items
  GtkWidget* status_icon_context_menu = gtk_menu_new();
  GtkWidget* quit_menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
  g_signal_connect (quit_menu_item, "activate",
		    G_CALLBACK (gtk_main_quit),
		    NULL);
  

  gtk_menu_shell_append(GTK_MENU_SHELL(status_icon_context_menu),
			quit_menu_item);
  
  gtk_widget_show_all(status_icon_context_menu);
  return status_icon_context_menu;
}


static void status_icon_popup_menu(GtkStatusIcon *status_icon,
			    guint          button,
			    guint          activate_time,
			    gpointer       user_data) {
  
  GtkMenu *menu = GTK_MENU(user_data);
  gtk_menu_popup(menu,
		 NULL,
		 NULL,
		 NULL, //gtk_status_icon_position_menu,
		 NULL,
		 button,
		 activate_time);
}

static GtkStatusIcon* build_status_icon() {
  
  GtkStatusIcon* status_icon;
  GtkWidget* status_icon_context_menu;

  status_icon = gtk_status_icon_new_from_stock(GTK_STOCK_FULLSCREEN);
  gtk_status_icon_set_tooltip_text(status_icon,
				   "Workspace Name Display");
  status_icon_context_menu = build_status_icon_context_menu();
  g_signal_connect (status_icon, "popup-menu",
		    G_CALLBACK (status_icon_popup_menu),
		    status_icon_context_menu);
  return status_icon;
}


static void workspace_changed(WnckScreen    *screen,
			      WnckWorkspace *previously_active_space,
			      gpointer       user_data) {
  
  WnckWorkspace* current_workspace = wnck_screen_get_active_workspace(screen);
  const char* workspace_name = wnck_workspace_get_name(current_workspace);
  int workspace_number = wnck_workspace_get_number(current_workspace);
    
  WorkspaceChangeManager* wcmgr = WORKSPACE_CHANGE_MANAGER(user_data);
  
  workspace_change_manager_display_workspace_name(wcmgr,
						  workspace_number,
						  workspace_name);
}


int main(int argc, char** argv) {
  
  WorkspaceChangeManager* wcmgr;

  gtk_init (&argc, &argv);

  build_status_icon();
  
  wcmgr = workspace_change_manager_new();
  
  WnckScreen* wnck_screen = wnck_screen_get(0);
  wnck_screen_force_update(wnck_screen);
  

#ifdef DEBUG
  const char* wm_name = wnck_screen_get_window_manager_name(wnck_screen);
  printf("Window Manager Name : %s\n", wm_name);
#endif
  g_signal_connect (wnck_screen,
		    "active-workspace-changed",
		    G_CALLBACK (workspace_changed),
		    wcmgr);
  
  gtk_main ();

  g_object_unref(wcmgr);
  return 0;
}
