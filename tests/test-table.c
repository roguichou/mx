#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <clutter/clutter.h>
#include <nbtk/nbtk.h>

int
main (int argc, char *argv[])
{
  ClutterActor *stage;
  NbtkWidget *table;
  ClutterActor *button1, *button2, *button3, *button4, *button5;
  GError *error = NULL;
  gint res;
  ClutterBehaviour *b;
  ClutterTimeline *timeline;

  clutter_init (&argc, &argv);

  stage = clutter_stage_get_default ();

  table = nbtk_table_new ();

  button1 = nbtk_button_new_with_label ("button1");
  button2 = nbtk_button_new_with_label ("button2");
  button3 = nbtk_button_new_with_label ("button3");
  button4 = nbtk_button_new_with_label ("button4");
  button5 = nbtk_button_new_with_label ("button5");

  nbtk_table_add_widget (table, button1, 0, 0);
  nbtk_table_add_widget (table, button2, 0, 1);
  nbtk_table_add_widget (table, button3, 1, 0);
  nbtk_table_add_widget (table, button4, 1, 1);
  nbtk_table_add_widget (table, button5, 2, 0);

  clutter_container_add_actor (CLUTTER_CONTAINER (stage), CLUTTER_ACTOR (table));
  clutter_actor_set_size (CLUTTER_ACTOR (table), 300, 300);

  clutter_actor_show (stage);

  timeline = clutter_timeline_new_for_duration (2000);
  clutter_timeline_set_loop (timeline, TRUE);

  b = clutter_behaviour_scale_new (
      clutter_alpha_new_full (timeline, clutter_sine_func, NULL, NULL),
      1, 2, 2, 1);


  clutter_timeline_start (timeline);
  clutter_behaviour_apply (b, table);

  clutter_main ();

  return EXIT_SUCCESS;
}
