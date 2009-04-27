
#include <nbtk/nbtk.h>

static void
clicked_cb (NbtkButton *button,
            gpointer    data)
{
  printf ("%s()\n", __FUNCTION__);
}

static void
changed_cb (NbtkAdjustment *adjustment,
            gpointer        data)
{
  printf ("%s() %.2f\n", __FUNCTION__,
                         nbtk_adjustment_get_value (adjustment));
}

int
main (int argc, char *argv[])
{
  ClutterActor    *stage;
  NbtkWidget      *button, *scroll;
  NbtkAdjustment  *adjustment;

  clutter_init (&argc, &argv);

  nbtk_style_load_from_file (nbtk_style_get_default (),
                             "style/default.css", NULL);

  stage = clutter_stage_get_default ();
  clutter_actor_set_size (stage, 400, 200);

  button = nbtk_scroll_button_new ();
  clutter_actor_set_position (CLUTTER_ACTOR (button), 50, 50);
  clutter_actor_set_size (CLUTTER_ACTOR (button), 24, 24);
  g_signal_connect (button, "clicked", 
                    G_CALLBACK (clicked_cb), NULL);
  clutter_container_add (CLUTTER_CONTAINER (stage), CLUTTER_ACTOR (button), NULL);

  adjustment = nbtk_adjustment_new (0., 0., 100., 1., 10., 10.);
  g_signal_connect (adjustment, "notify::value", 
                    G_CALLBACK (changed_cb), NULL);  

  scroll = nbtk_scroll_bar_new (adjustment);
  clutter_actor_set_position (CLUTTER_ACTOR (scroll), 50, 100);
  clutter_actor_set_size (CLUTTER_ACTOR (scroll), 200, 30);
  clutter_container_add (CLUTTER_CONTAINER (stage), CLUTTER_ACTOR (scroll), NULL);

  clutter_actor_show (stage);

  clutter_main ();

  return EXIT_SUCCESS;
}
