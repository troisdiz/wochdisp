#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "debug.h"
#include "workspace-change-manager.h"

static GObjectClass *parent_class = NULL;

static void workspace_change_manager_inst_init(GTypeInstance* instance,
					       gpointer       g_class);
static void workspace_change_manager_finalize (GObject *obj);
static void workspace_change_manager_dispose (GObject *obj);

static gboolean draw_area_on_expose_sevent(GtkWidget *widget,
					   GdkEvent  *event,
					   gpointer   user_data);

static gboolean hide_window_func(gpointer data);
gboolean workspace_change_manager_is_window_displayed(WorkspaceChangeManager* self);
void workspace_change_manager_hide_window(WorkspaceChangeManager* self);
void workspace_change_manager_set_window_displayed(WorkspaceChangeManager* self,
						   gboolean is_displayed);

static void workspace_change_manager_class_init (gpointer g_class,
						 gpointer g_class_data) {
  
  GObjectClass      *gobject_class = G_OBJECT_CLASS (g_class);
  
  parent_class = g_type_class_peek_parent (gobject_class);

  gobject_class->dispose = workspace_change_manager_dispose;
  gobject_class->finalize = workspace_change_manager_finalize;
}

GType workspace_change_manager_get_type (void) {
  static GType type = 0;
  if (type == 0) {
    static const GTypeInfo info = {
      sizeof (WorkspaceChangeManagerClass),
      NULL,   /* base_init */
      NULL,   /* base_finalize */
      workspace_change_manager_class_init,   /* class_init */
      NULL,   /* class_finalize */
      NULL,   /* class_data */
      sizeof (WorkspaceChangeManager),
      0,      /* n_preallocs */
      workspace_change_manager_inst_init    /* instance_init */
    };
    //TODO
    type = g_type_register_static (G_TYPE_OBJECT,
				   "WorkspaceChangeManagerType",
				   &info, 0);
  }
  return type;
}

static void workspace_change_manager_finalize (GObject *obj) {
  
  //WorkspaceChangeManager* self = WORKSPACE_CHANGE_MANAGER(obj);

  /*
   * Here, complete object destruction.
   * You might not need to do much...
   */
  
  /* Chain up to the parent class */
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void workspace_change_manager_dispose (GObject *obj) {

  //WorkspaceChangeManager* self = WORKSPACE_CHANGE_MANAGER(obj);
  
  /* 
   * In dispose, you are supposed to free all types referenced from this
   * object which might themselves hold a reference to self. Generally,
   * the most simple solution is to unref all members on which you own a 
   * reference.
   */
  
  /* Chain up to the parent class */
  G_OBJECT_CLASS (parent_class)->dispose (obj);
}

static void workspace_change_manager_inst_init(GTypeInstance* instance,
					       gpointer       g_class) {
  
  WorkspaceChangeManager* wcm = WORKSPACE_CHANGE_MANAGER(instance);
  wcm->_isWorkspaceDisplayed = FALSE;
  wcm->window                = NULL;
  wcm->display_string        = NULL;
  wcm->timer_func_uid        = 0;
}

WorkspaceChangeManager* workspace_change_manager_new() {
  
  WorkspaceChangeManager* returnValue = (WorkspaceChangeManager*) g_object_new(workspace_change_manager_get_type(), NULL);
  
  return returnValue;
}

void workspace_change_manager_display_workspace_name(WorkspaceChangeManager* self,
						     int workspace_index,
						     const gchar* workspace_name) {
  
  GtkWidget* darea;
  int display_string_max_length = 4 + strlen(workspace_name) + 1;
#ifdef DEBUG
  printf("\nCurrent workspace : idx = %d, name = %s\n", workspace_index+1, workspace_name);
#endif

  if (workspace_change_manager_is_window_displayed(self)) {
    workspace_change_manager_hide_window(self);
    //gtk_widget_destroy(self->window);
  }

  self->display_string = (char *) malloc(display_string_max_length * sizeof(char));
  snprintf(self->display_string, display_string_max_length, "#%d %s", workspace_index+1, workspace_name);
  
  self->_isWorkspaceDisplayed = TRUE;
  
  self->window = gtk_window_new(GTK_WINDOW_POPUP);
  gtk_window_set_default_size(GTK_WINDOW(self->window),
			      1,
			      1);
  
  GdkScreen *screen = gtk_widget_get_screen(self->window);
  GdkColormap *colormap = gdk_screen_get_rgba_colormap(screen);
  
  if (!colormap) {
#ifdef DEBUG
    printf("Your screen does not support alpha channels!\n");
#endif
    colormap = gdk_screen_get_rgb_colormap(screen);
  }
  // Now we have a colormap appropriate for the screen, use it 
  gtk_widget_set_colormap(self->window, colormap);
  
  gtk_widget_set_app_paintable(self->window, TRUE);

  gtk_window_set_position(GTK_WINDOW(self->window),
			  GTK_WIN_POS_CENTER_ALWAYS);
  darea = gtk_drawing_area_new();
  
  gtk_container_add(GTK_CONTAINER(self->window),
		    darea);
  
  
  g_signal_connect (darea, "expose-event",
		    G_CALLBACK (draw_area_on_expose_sevent),
		    self);
  gtk_widget_realize (self->window);
  gdk_window_set_back_pixmap(self->window->window, NULL, FALSE);
  gtk_widget_show(darea);
  gtk_widget_show(self->window);

  self->timer_func_uid = g_timeout_add(500,
				       &hide_window_func,
				       self);
}

#define PADDING_LEFT   20
#define PADDING_RIGHT  20
#define PADDING_TOP    10
#define PADDING_BOTTOM 10

static gboolean draw_area_on_expose_sevent(GtkWidget *widget,
					   GdkEvent  *event,
					   gpointer   user_data) {
  GdkWindow* darea_window;
  cairo_t * cairo_ctx;
  PangoLayout* pangoLayout;
  PangoFontDescription *fontDesc = NULL;
  int width;
  int height;
  WorkspaceChangeManager* wcm = WORKSPACE_CHANGE_MANAGER(user_data);
  
  char* display_string = wcm->display_string;
  
  darea_window = gtk_widget_get_window(widget);
  
  cairo_ctx = gdk_cairo_create(GDK_DRAWABLE(darea_window));
  
  pangoLayout = pango_cairo_create_layout(cairo_ctx);
  pango_layout_set_text(pangoLayout,
			display_string,
			-1);
    
  fontDesc = pango_font_description_from_string("Monospace 48");
#ifdef DEBUG
  if (fontDesc == NULL) {
    printf("fontDesc is null\n");
  }
#endif
  pango_layout_set_font_description(pangoLayout,
				    fontDesc);
  pango_font_description_free(fontDesc);

  pango_layout_get_pixel_size(pangoLayout,
			      &width,
			      &height);
  
  gtk_widget_set_size_request(widget,
			      width+PADDING_LEFT+PADDING_RIGHT,
			      height+PADDING_TOP+PADDING_BOTTOM);
#ifdef DEBUG
  printf("    pango layout get pixel size : width = %d, height = %d\n",
	 width,
	 height);
  printf("    gtk widget requester : width = %d, height = %d\n",
	 width+PADDING_LEFT+PADDING_RIGHT,
	 height+PADDING_TOP+PADDING_BOTTOM);
#endif
  cairo_set_source_rgba(cairo_ctx, 1.0, 1.0, 1.0, 0.0);
  cairo_set_operator (cairo_ctx, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cairo_ctx);
  
  /*
  cairo_set_antialias(cairo_ctx, CAIRO_ANTIALIAS_NONE);
  cairo_set_source_rgba(cairo_ctx, 0.3, 0.4, 0.6, 1.0);
  cairo_set_line_width(cairo_ctx, 8.0);
  cairo_rectangle(cairo_ctx,
		  4.0,
		  4.0,
		  width+PADDING_LEFT+PADDING_RIGHT-8,
		  height+PADDING_TOP+PADDING_BOTTOM-8);
  cairo_stroke(cairo_ctx);
  */

  //cairo_set_source_rgba(cairo_ctx, 0.3, 0.4, 0.6, 1.0);
  //cairo_set_source_rgba(cairo_ctx, 0.0, 0.0, 0.0, 1.0);
  cairo_set_source_rgba(cairo_ctx, 1.0, 1.0, 1.0, 1.0);
  
  cairo_translate(cairo_ctx, PADDING_LEFT, PADDING_TOP);
  
  pango_cairo_update_layout(cairo_ctx, pangoLayout);  
  pango_cairo_layout_path(cairo_ctx, pangoLayout);
  cairo_fill_preserve(cairo_ctx);

  //cairo_set_line_width(cairo_ctx, 3.0);
  ////cairo_set_source_rgba(cairo_ctx, 0.0, 0.0, 0.0, 1.0);
  cairo_set_source_rgba(cairo_ctx, 1.0, 1.0, 1.0, 1.0);
  cairo_set_source_rgba(cairo_ctx, 0.3, 0.4, 0.6, 1.0);
  pango_cairo_layout_path(cairo_ctx, pangoLayout);
  cairo_stroke(cairo_ctx);
  
  g_object_unref(pangoLayout);
  cairo_destroy(cairo_ctx);
  
  return TRUE;
}

void workspace_change_manager_hide_window(WorkspaceChangeManager* self) {
  gtk_widget_destroy(self->window);
  if (self->timer_func_uid != 0) {
    gboolean success = g_source_remove(self->timer_func_uid);
    self->timer_func_uid = 0;
  }
  free(self->display_string);
  workspace_change_manager_set_window_displayed(self,
						FALSE);
}

gboolean workspace_change_manager_is_window_displayed(WorkspaceChangeManager* self) {
  return self->_isWorkspaceDisplayed;
}

void workspace_change_manager_set_window_displayed(WorkspaceChangeManager* self,
						   gboolean is_displayed) {
  self->_isWorkspaceDisplayed = is_displayed;
  self->window = NULL;
  self->timer_func_uid = 0;
  self->display_string = NULL;
}

static gboolean hide_window_func(gpointer data) {
  
  WorkspaceChangeManager* wcm = WORKSPACE_CHANGE_MANAGER(data);
  workspace_change_manager_hide_window(wcm);
  return FALSE;
}
