#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xE3, 0x2A, 0x8B, 0xDA, 0xE8, 0x2D, 0x45, 0x17, 0xA6, 0x9D, 0xA9, 0x29, 0x63, 0xFA, 0x84, 0x20 }
PBL_APP_INFO(MY_UUID,
             "Astronomical Analog", "Antgiant's Watches",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Main Window");
  window_stack_push(&window, true /* Animated */);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
