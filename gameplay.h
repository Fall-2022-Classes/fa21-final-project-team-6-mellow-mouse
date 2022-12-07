#include "vga_core.h"
#include "types.h"
#include "gpio_cores.h"
#include "ps2_core.h"

void feed(health *bars, FrameCore *frame_p, OsdCore *osd_p);
void shower(health *bars, FrameCore *frame_p, OsdCore *osd_p);
void pet(health *bars, FrameCore *frame_p, OsdCore *osd_p);
void game_over(FrameCore *frame_p, OsdCore *osd_p, Ps2Core *ps2_p, health *bars, TimerCore *timer_p);