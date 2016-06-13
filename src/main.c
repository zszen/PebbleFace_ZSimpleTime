#include <pebble.h>
#include "defineZ.c"

static Window *s_main_window;
static GSize s_windowSize, s_windowSizeHalf, s_numberSize;
static BitmapLayer *layer_number_hour, *layer_number_minute;
static int time_hour_old;

static void update_time() {
  time_t timeNow = time(NULL); 
  struct tm *timeLocal = localtime(&timeNow);
  int offsetMinute = -((timeLocal->tm_min-30)/30.0)*s_windowSizeHalf.h;
  //update minute position
  layer_set_frame(bitmap_layer_get_layer(layer_number_minute), GRect(s_windowSizeHalf.w, s_windowSizeHalf.h-offsetMinute, s_numberSize.w, s_numberSize.h));
//   if(timeLocal->tm_sec==0){
    //update hour position
    if(timeLocal->tm_min==0){
      int offsetHour = -((timeLocal->tm_hour-12)/12.0)*s_windowSizeHalf.h;
      layer_set_frame(bitmap_layer_get_layer(layer_number_hour), GRect(s_windowSizeHalf.w-s_numberSize.w, s_windowSizeHalf.h-offsetHour, s_numberSize.w, s_numberSize.h));
      //update hour number
      GBitmap* g_frame_bitmap;
      g_frame_bitmap = gbitmap_create_with_resource(getDigitalImageByNumber(timeLocal->tm_hour%10));
      bitmap_layer_set_bitmap(layer_number_minute, g_frame_bitmap);
      if(time_hour_old/10 != timeLocal->tm_hour/10){
        g_frame_bitmap = gbitmap_create_with_resource(getDigitalImageByNumber(timeLocal->tm_hour/10));
        bitmap_layer_set_bitmap(layer_number_hour, g_frame_bitmap);
      }
    }
//   }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  time_t timeNow = time(NULL); 
  struct tm *timeLocal = localtime(&timeNow);
  time_hour_old = timeLocal->tm_hour;
  int offsetHour = -((timeLocal->tm_hour-12)/12.0)*s_windowSizeHalf.h;
  int offsetMinute = -((timeLocal->tm_min-30)/30.0)*s_windowSizeHalf.h;
//   int offsetSecond = -((timeLocal->tm_sec-30)/30.0)*s_windowSizeHalf.h;
  
//   APP_LOG(APP_LOG_LEVEL_DEBUG, ">>>>>>>>%d %d", timeLocal->tm_hour,offsetHour);
  
  GBitmap* g_frame_bitmap;
  g_frame_bitmap = gbitmap_create_with_resource(getDigitalImageByNumber(timeLocal->tm_hour/10));
  layer_number_hour = bitmap_layer_create(GRect(s_windowSizeHalf.w-s_numberSize.w, s_windowSizeHalf.h-offsetHour, s_numberSize.w, s_numberSize.h));
  bitmap_layer_set_compositing_mode(layer_number_hour, GCompOpSet);
  bitmap_layer_set_bitmap(layer_number_hour, g_frame_bitmap);
  layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(layer_number_hour));
  
  g_frame_bitmap = gbitmap_create_with_resource(getDigitalImageByNumber(timeLocal->tm_hour%10));
  layer_number_minute = bitmap_layer_create(GRect(s_windowSizeHalf.w, s_windowSizeHalf.h-offsetMinute, s_numberSize.w, s_numberSize.h));
  bitmap_layer_set_compositing_mode(layer_number_minute, GCompOpSet);
  bitmap_layer_set_bitmap(layer_number_minute, g_frame_bitmap);
  layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(layer_number_minute));
}

static void main_window_unload(Window *window) {
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  
  s_numberSize = GSize(57, 69);
  
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_bounds(window_layer);
  s_windowSize = bounds.size;
  s_windowSizeHalf = GSize(s_windowSize.w/2, s_windowSize.h/2-s_numberSize.h/2);
  
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
//   tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}