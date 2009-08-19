#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clutter/clutter.h>
#include <nbtk/nbtk.h>

/* Droppable */

#define DROPPABLE_TYPE_GROUP            (droppable_group_get_type ())
#define DROPPABLE_GROUP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DROPPABLE_TYPE_GROUP, DroppableGroup))
#define DROPPABLE_IS_GROUP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DROPPABLE_TYPE_GROUP))

typedef struct _DroppableGroup          DroppableGroup;
typedef struct _DroppableGroupClass     DroppableGroupClass;

struct _DroppableGroup
{
  ClutterGroup parent_instance;

  ClutterActor *background;

  guint is_enabled : 1;
};

struct _DroppableGroupClass
{
  ClutterGroupClass parent_class;
};

enum
{
  DROP_PROP_0,

  DROP_PROP_ENABLED
};

static const ClutterColor default_background_color = { 204, 204, 0, 255 };

static void nbtk_droppable_iface_init (NbtkDroppableIface *iface);
static GType droppable_group_get_type ();

G_DEFINE_TYPE_WITH_CODE (DroppableGroup, droppable_group, CLUTTER_TYPE_GROUP,
                         G_IMPLEMENT_INTERFACE (NBTK_TYPE_DROPPABLE,
                                                nbtk_droppable_iface_init));

static void
droppable_group_dispose (GObject *gobject)
{
  DroppableGroup *group = DROPPABLE_GROUP (gobject);

  if (group->background)
    {
      clutter_actor_destroy (group->background);
      group->background = NULL;
    }

  G_OBJECT_CLASS (droppable_group_parent_class)->dispose (gobject);
}

static void
droppable_group_over_in (NbtkDroppable *droppable,
                         NbtkDraggable *draggable)
{
  g_debug (G_STRLOC ": over-in");
  clutter_actor_animate (CLUTTER_ACTOR (droppable),
                         CLUTTER_EASE_IN_CUBIC,
                         250,
                         "opacity", 255,
                         NULL);
}

static void
droppable_group_over_out (NbtkDroppable *droppable,
                          NbtkDraggable *draggable)
{
  g_debug (G_STRLOC ": over-out");
  clutter_actor_animate (CLUTTER_ACTOR (droppable),
                         CLUTTER_EASE_IN_CUBIC,
                         250,
                         "opacity", 128,
                         NULL);
}

static void
droppable_group_drop (NbtkDroppable       *droppable,
                      NbtkDraggable       *draggable,
                      gfloat               event_x,
                      gfloat               event_y,
                      gint                 button,
                      ClutterModifierType  modifiers)
{
  ClutterActor *self = CLUTTER_ACTOR (droppable);
  ClutterActor *child = CLUTTER_ACTOR (draggable);

  g_debug ("%s: dropped %s on '%s' (%s) at %.2f, %.2f",
           G_STRLOC,
           G_OBJECT_TYPE_NAME (draggable),
           clutter_actor_get_name (self),
           G_OBJECT_TYPE_NAME (droppable),
           event_x, event_y);

  g_object_ref (draggable);

  clutter_actor_reparent (child, self);
  clutter_actor_set_position (CLUTTER_ACTOR (draggable),
                              (event_x < 100) ? 50 : 100,
                              (event_y < 100) ? 50 : 100);

  g_object_unref (draggable);
}

static void
nbtk_droppable_iface_init (NbtkDroppableIface *iface)
{
  iface->over_in = droppable_group_over_in;
  iface->over_out = droppable_group_over_out;
  iface->drop = droppable_group_drop;
}

static void
on_actor_added (ClutterContainer *container,
                ClutterActor     *actor)
{
  g_debug ("%s: added child `%s'", G_STRLOC, G_OBJECT_TYPE_NAME (actor));
}

static void
droppable_group_set_property (GObject      *gobject,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  DroppableGroup *group = DROPPABLE_GROUP (gobject);

  switch (prop_id)
    {
    case DROP_PROP_ENABLED:
      group->is_enabled = g_value_get_boolean (value);
      if (group->is_enabled)
        nbtk_droppable_enable (NBTK_DROPPABLE (gobject));
      else
        nbtk_droppable_disable (NBTK_DROPPABLE (gobject));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
droppable_group_get_property (GObject    *gobject,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  DroppableGroup *group = DROPPABLE_GROUP (gobject);

  switch (prop_id)
    {
    case DROP_PROP_ENABLED:
      g_value_set_boolean (value, group->is_enabled);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
droppable_group_class_init (DroppableGroupClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = droppable_group_set_property;
  gobject_class->get_property = droppable_group_get_property;
  gobject_class->dispose = droppable_group_dispose;

  g_object_class_override_property (gobject_class,
                                    DROP_PROP_ENABLED,
                                    "enabled");
}

static void
droppable_group_init (DroppableGroup *group)
{
  clutter_actor_set_opacity (CLUTTER_ACTOR (group), 128);

  group->background = clutter_rectangle_new ();
  clutter_container_add_actor (CLUTTER_CONTAINER (group),
                               group->background);

  clutter_rectangle_set_color (CLUTTER_RECTANGLE (group->background),
                               &default_background_color);

  clutter_actor_set_size (group->background, 200, 200);

  g_signal_connect (group, "actor-added",
                    G_CALLBACK (on_actor_added), NULL);
}

/* Draggable */

#define DRAGGABLE_TYPE_RECTANGLE        (draggable_rectangle_get_type ())
#define DRAGGABLE_RECTANGLE(obj)        (G_TYPE_CHECK_INSTANCE_CAST ((obj), DRAGGABLE_TYPE_RECTANGLE, DraggableRectangle))
#define DRAGGABLE_IS_RECTANGLE(obj)     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DRAGGABLE_TYPE_RECTANGLE))

typedef struct _DraggableRectangle      DraggableRectangle;
typedef struct _DraggableRectangleClass DraggableRectangleClass;

struct _DraggableRectangle
{
  ClutterRectangle parent_instance;

  /* Draggable properties */
  guint threshold;

  NbtkDragAxis axis;

  NbtkDragContainment containment;
  ClutterActorBox area;

  guint is_enabled : 1;
};

struct _DraggableRectangleClass
{
  ClutterRectangleClass parent_class;
};

enum
{
  DRAG_PROP_0,

  DRAG_PROP_DRAG_THRESHOLD,
  DRAG_PROP_AXIS,
  DRAG_PROP_CONTAINMENT_TYPE,
  DRAG_PROP_CONTAINMENT_AREA,
  DRAG_PROP_ENABLED
};

static void nbtk_draggable_iface_init (NbtkDraggableIface *iface);
static GType draggable_rectangle_get_type ();

G_DEFINE_TYPE_WITH_CODE (DraggableRectangle,
                         draggable_rectangle,
                         CLUTTER_TYPE_RECTANGLE,
                         G_IMPLEMENT_INTERFACE (NBTK_TYPE_DRAGGABLE,
                                                nbtk_draggable_iface_init));

static void
draggable_rectangle_drag_begin (NbtkDraggable       *draggable,
                                gfloat               event_x,
                                gfloat               event_y,
                                gint                 event_button,
                                ClutterModifierType  modifiers)
{
  ClutterActor *self = CLUTTER_ACTOR (draggable);
  ClutterActor *stage = clutter_actor_get_stage (self);
  gfloat orig_x, orig_y;

  g_object_ref (self);

  clutter_actor_get_transformed_position (self, &orig_x, &orig_y);
  clutter_actor_reparent (self, stage);
  clutter_actor_set_position (self, orig_x, orig_y);

  g_object_unref (self);

  clutter_actor_animate (self, CLUTTER_EASE_OUT_CUBIC, 250,
                         "opacity", 224,
                         NULL);
}

static void
draggable_rectangle_drag_motion (NbtkDraggable *draggable,
                                 gfloat         delta_x,
                                 gfloat         delta_y)
{
  clutter_actor_move_by (CLUTTER_ACTOR (draggable), delta_x, delta_y);
}

static void
draggable_rectangle_drag_end (NbtkDraggable *draggable,
                              gfloat         event_x,
                              gfloat         event_y)
{
  ClutterActor *self = CLUTTER_ACTOR (draggable);

  clutter_actor_animate (self, CLUTTER_EASE_OUT_CUBIC, 250,
                         "opacity", 255,
                         NULL);
}

static void
nbtk_draggable_iface_init (NbtkDraggableIface *iface)
{
  iface->drag_begin = draggable_rectangle_drag_begin;
  iface->drag_motion = draggable_rectangle_drag_motion;
  iface->drag_end = draggable_rectangle_drag_end;
}

static void
draggable_rectangle_parent_set (ClutterActor *actor,
                                ClutterActor *old_parent)
{
  ClutterActor *new_parent = clutter_actor_get_parent (actor);

  g_debug ("%s: old_parent: %s, new_parent: %s (%s)",
           G_STRLOC,
           old_parent ? G_OBJECT_TYPE_NAME (old_parent) : "none",
           new_parent ? clutter_actor_get_name (new_parent) : "Unknown",
           new_parent ? G_OBJECT_TYPE_NAME (new_parent) : "none");
}

static void
draggable_rectangle_set_property (GObject      *gobject,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  DraggableRectangle *rect = DRAGGABLE_RECTANGLE (gobject);

  switch (prop_id)
    {
    case DRAG_PROP_DRAG_THRESHOLD:
      rect->threshold = g_value_get_uint (value);
      break;

    case DRAG_PROP_AXIS:
      rect->axis = g_value_get_enum (value);
      break;

    case DRAG_PROP_CONTAINMENT_TYPE:
      rect->containment = g_value_get_enum (value);
      break;

    case DRAG_PROP_CONTAINMENT_AREA:
      {
        ClutterActorBox *box = g_value_get_boxed (value);

        if (box)
          rect->area = *box;
        else
          memset (&rect->area, 0, sizeof (ClutterActorBox));
      }
      break;

    case DRAG_PROP_ENABLED:
      rect->is_enabled = g_value_get_boolean (value);
      if (rect->is_enabled)
        nbtk_draggable_enable (NBTK_DRAGGABLE (gobject));
      else
        nbtk_draggable_disable (NBTK_DRAGGABLE (gobject));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
draggable_rectangle_get_property (GObject    *gobject,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  DraggableRectangle *rect = DRAGGABLE_RECTANGLE (gobject);

  switch (prop_id)
    {
    case DRAG_PROP_DRAG_THRESHOLD:
      g_value_set_uint (value, rect->threshold);
      break;

    case DRAG_PROP_AXIS:
      g_value_set_enum (value, rect->axis);
      break;

    case DRAG_PROP_CONTAINMENT_TYPE:
      g_value_set_enum (value, rect->containment);
      break;

    case DRAG_PROP_CONTAINMENT_AREA:
      g_value_set_boxed (value, &rect->area);
      break;

    case DRAG_PROP_ENABLED:
      g_value_set_boolean (value, rect->is_enabled);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
draggable_rectangle_class_init (DraggableRectangleClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  gobject_class->set_property = draggable_rectangle_set_property;
  gobject_class->get_property = draggable_rectangle_get_property;

  actor_class->parent_set = draggable_rectangle_parent_set;

  g_object_class_override_property (gobject_class,
                                    DRAG_PROP_DRAG_THRESHOLD,
                                    "drag-threshold");
  g_object_class_override_property (gobject_class,
                                    DRAG_PROP_AXIS,
                                    "axis");
  g_object_class_override_property (gobject_class,
                                    DRAG_PROP_CONTAINMENT_TYPE,
                                    "containment-type");
  g_object_class_override_property (gobject_class,
                                    DRAG_PROP_CONTAINMENT_AREA,
                                    "containment-area");
  g_object_class_override_property (gobject_class,
                                    DRAG_PROP_ENABLED,
                                    "enabled");
}

static void
draggable_rectangle_init (DraggableRectangle *self)
{
  self->threshold = 0;
  self->axis = 0;
  self->containment = NBTK_DISABLE_CONTAINMENT;
  self->is_enabled = FALSE;
}

/* main */

int
main (int argc, char *argv[])
{
  ClutterActor *stage;
  ClutterActor *draggable, *droppable;
  ClutterColor rect_color1 = { 146, 123,  81, 255 };
  ClutterColor rect_color2 = { 128, 195,  28, 255 };
  ClutterColor rect_color3 = { 255, 122,   2, 255 };
  ClutterColor rect_color4 = { 141, 195, 233, 255 };

  clutter_init (&argc, &argv);

  stage = clutter_stage_get_default ();
  clutter_stage_set_title (CLUTTER_STAGE (stage), "Droppable Example");
  clutter_actor_set_size (stage, 800, 600);

  droppable = g_object_new (DROPPABLE_TYPE_GROUP, NULL);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), droppable);
  clutter_actor_set_position (droppable, 500, 50);
  clutter_actor_set_reactive (droppable, TRUE);
  clutter_actor_set_name (droppable, "Drop Target 1");
  nbtk_droppable_enable (NBTK_DROPPABLE (droppable));

  droppable = g_object_new (DROPPABLE_TYPE_GROUP, NULL);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), droppable);
  clutter_actor_set_position (droppable, 500, 350);
  clutter_actor_set_reactive (droppable, TRUE);
  clutter_actor_set_name (droppable, "Drop Target 2");
  nbtk_droppable_enable (NBTK_DROPPABLE (droppable));

  draggable = g_object_new (DRAGGABLE_TYPE_RECTANGLE,
                            "color", &rect_color1,
                            NULL);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), draggable);
  clutter_actor_set_size (draggable, 50, 50);
  clutter_actor_set_position (draggable, 75, 250);
  clutter_actor_set_reactive (draggable, TRUE);
  nbtk_draggable_enable (NBTK_DRAGGABLE (draggable));

  draggable = g_object_new (DRAGGABLE_TYPE_RECTANGLE,
                            "color", &rect_color2,
                            NULL);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), draggable);
  clutter_actor_set_size (draggable, 50, 50);
  clutter_actor_set_position (draggable, 125, 250);
  clutter_actor_set_reactive (draggable, TRUE);
  nbtk_draggable_enable (NBTK_DRAGGABLE (draggable));

  draggable = g_object_new (DRAGGABLE_TYPE_RECTANGLE,
                            "color", &rect_color3,
                            NULL);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), draggable);
  clutter_actor_set_size (draggable, 50, 50);
  clutter_actor_set_position (draggable, 75, 300);
  clutter_actor_set_reactive (draggable, TRUE);
  nbtk_draggable_enable (NBTK_DRAGGABLE (draggable));

  draggable = g_object_new (DRAGGABLE_TYPE_RECTANGLE,
                            "color", &rect_color4,
                            NULL);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), draggable);
  clutter_actor_set_size (draggable, 50, 50);
  clutter_actor_set_position (draggable, 125, 300);
  clutter_actor_set_reactive (draggable, TRUE);
  nbtk_draggable_enable (NBTK_DRAGGABLE (draggable));




  clutter_actor_show_all (stage);

  clutter_main ();

  return EXIT_SUCCESS;
}
