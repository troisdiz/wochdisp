#ifndef _WORKSPACE_CHANGE_MANAGER_H
#define _WORKSPACE_CHANGE_MANAGER_H

#include <glib-object.h>

G_BEGIN_DECLS /* rajoute les extern "C" et autres attibuts */

typedef struct _WorkspaceChangeManagerClass WorkspaceChangeManagerClass;
typedef struct _WorkspaceChangeManager WorkspaceChangeManager;

struct _WorkspaceChangeManagerClass {
  GObjectClass  parent_class;
};

struct _WorkspaceChangeManager {
  GObject       parent;
  /* private */
  int _isWorkspaceDisplayed;
  GtkWidget* window;
  char *display_string;
  int timer_func_uid;
  // timeout func
};

#define TYPE_WORKSPACE_CHANGE_MANAGER    (workspace_change_manager_get_type())
#define WORKSPACE_CHANGE_MANAGER(a)      ((WorkspaceChangeManager*)(a))
#define IS_WORKSPACE_CHANGE_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYPE_WORKSPACE_CHANGE_MANAGER))

GType workspace_change_manager_get_type (void);
WorkspaceChangeManager* workspace_change_manager_new();

void workspace_change_manager_display_workspace_name(WorkspaceChangeManager* wc_mgr, int workspace_index, const gchar* workspace_name);
 
G_END_DECLS

#endif /* _WORKSPACE_CHANGE_MANAGER_H */
