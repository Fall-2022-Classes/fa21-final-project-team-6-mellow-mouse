#include "vga_core.h"
#include "types.h"
#include "gpio_cores.h"

void draw_sprite(SpriteCore *hampster_p);
void draw_bars(OsdCore *osd_p, health *bars);
void draw_timer(uint64_t max_time, uint64_t time, GpiCore *sw, OsdCore *osd_p);
void change_scene(int scene, FrameCore *frame_p);