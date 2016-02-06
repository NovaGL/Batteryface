#include <pebble.h>
#define KEY_DATE_FORMAT 0
#define KEY_BT_FORMAT 1
#define KEY_VIBE_ON_DISCONNECT 2
#define KEY_VIBE_ON_CONNECT 3
#define KEY_COLOR_RED     4
#define KEY_COLOR_GREEN   5
#define KEY_COLOR_BLUE    6
#define LOW_POWER 50

GColor time_color;

static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer;
static Layer *s_battery_layer;

static GFont s_time_font, s_date_font;
static BitmapLayer *s_background_layer, *s_bt_icon_layer;
static GBitmap *s_background_bitmap, *s_bt_icon_bitmap;

static int s_battery_level, parent_bounds;

static bool bt_toggle = 0;
static bool euro_date = 1;
static bool charge = 0;
static bool vibe_on_disconnect = 0;
static bool vibe_on_connect = 0;


/* Grab settings
enum {
  CONFIG_dateformat = 0x0,
  CONFIG_bt = 0x1, 
  CONFIG_bt_disconnect = 0x2,
  CONFIG_bt_connect = 0x3
};
*/

// Record Bluetooth connection state
static void bluetooth_callback(bool connected) {
	
  if (bt_toggle == 1) {   
      APP_LOG(APP_LOG_LEVEL_INFO, "Hide Bluetooth connection symbol"); 
      if		(!connected) {bitmap_layer_set_bitmap(s_bt_icon_layer, gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_0));}
      else	{bitmap_layer_set_bitmap(s_bt_icon_layer,gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLANK_WHITE));}			    			
  } else {
      APP_LOG(APP_LOG_LEVEL_INFO, "Show Bluetooth connection symbol"); 
      if		(!connected) {bitmap_layer_set_bitmap(s_bt_icon_layer, gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_0));}
	    else	{bitmap_layer_set_bitmap(s_bt_icon_layer,gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_1));}			    
  }

  // set vibrations
  if (!connected) {		
		if (vibe_on_disconnect == 1) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Vibrate on Bluetooth disconnect"); 
			vibes_long_pulse();
		}
	} else {
		if (vibe_on_connect == 1) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Vibrate on Bluetooth reconnection"); 
			vibes_double_pulse();
		}
	}
  
}
////////////////// Time and Date Status//////////////////////
static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer, and show the time
  static char time_buffer[] = "00:00";
  
  if(clock_is_24h_style()) {
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  text_layer_set_text(s_time_layer, time_buffer);
  
  // Show the date
  
  static char date_buffer[] = "WWW MMM DD"; // Buffer for entire date to display
  static char datn_buffer[] = "DD"; // Buffer for date number
  strftime(datn_buffer, sizeof("DD"), "%d", tick_time); // Write current date to buffer
  
  if (euro_date == 1) { // If the user has selected the WWW DD MMM date format
		strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);
	} else {
		strftime(date_buffer, sizeof(date_buffer), "%a %b %d", tick_time);
	}
  
  text_layer_set_text(s_date_layer, date_buffer);
}
//////////////////////////////// Battery Level Graphics ///////////////////////////////////
static void battery_update_proc(Layer *layer, GContext *ctx) {
   
 GRect bounds = layer_get_bounds(layer);
  
  // Find the width of the bar
  int width = (int)(float)(((float)s_battery_level / 100.0F) * parent_bounds);
  
  // Draw the background
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, bounds, GCornerNone, 0);
  
  // Draw the battery bar  
  
  graphics_context_set_fill_color(ctx, GColorMediumSpringGreen);
     
  
  if (s_battery_level <= 20 && charge == 0)  {
      graphics_context_set_fill_color(ctx, GColorRed);      
  }  
  
   if (charge == 1)  {
       graphics_context_set_fill_color(ctx, GColorIcterine);    
  }  
   graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), GCornerNone, 0);
 
}
//////////////////////////////// Battery Charge Graphics ///////////////////////////////////
static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
  
    if (state.is_plugged) {
      charge = 1;
    } else {
      charge = 0;
    }  
    
  // Update meter
  layer_mark_dirty(s_battery_layer);
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

//////////////////////////////////////////////////// Main Visuals /////////////////////////////
static void main_window_load(Window *window) {
  
  window = window_create();
  window_stack_push(window, true);
	window_set_background_color(window, GColorBlack);
  time_color = GColorFromRGB(0, 170, 255);
  
	Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  parent_bounds = bounds.size.w -20;
    
  // Create GFonts
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_46));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
   
    
  // Create TIME TextLayer and set colour
  s_time_layer = text_layer_create(GRect(0, 40, bounds.size.w, bounds.size.h));
  text_layer_set_background_color(s_time_layer, GColorClear);
  // Read saved config
  #if defined(PBL_BW)
    text_layer_set_text_color(s_time_layer, GColorWhite);
  #elif defined(PBL_COLOR)
    // Use background color setting
    int red = persist_read_int(KEY_COLOR_RED);
    int green = persist_read_int(KEY_COLOR_GREEN);
    int blue = persist_read_int(KEY_COLOR_BLUE);
    time_color = GColorFromRGB(red, green, blue);    
    text_layer_set_text_color(s_time_layer, time_color);
  #endif
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  
  // Create DATE TextLayer
  s_date_layer = text_layer_create(GRect(0, 115, bounds.size.w, bounds.size.h));
  text_layer_set_text_color(s_date_layer, GColorFromHEX(0xFFFFFF));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text(s_date_layer, "Sept 23");
  text_layer_set_font(s_date_layer, s_date_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));

  // Create BATTERY meter Layer
  s_battery_layer = layer_create(GRect(10, 102, bounds.size.w -20, 2));
  layer_set_update_proc(s_battery_layer, battery_update_proc);
  layer_add_child(window_get_root_layer(window), s_battery_layer);
  
  
  // Create the BitmapLayer to display the Bluetooth icon
  s_bt_icon_layer = bitmap_layer_create(GRect(115, 10, 11, 10));
  bitmap_layer_set_bitmap(s_bt_icon_layer, s_bt_icon_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bt_icon_layer));
  
  // Initialize the display
  update_time();
  battery_callback(battery_state_service_peek());

  bluetooth_callback(connection_service_peek_pebble_app_connection());
}

static void main_window_unload(Window *window) {
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  
  gbitmap_destroy(s_background_bitmap);
  gbitmap_destroy(s_bt_icon_bitmap);

  bitmap_layer_destroy(s_background_layer);
  bitmap_layer_destroy(s_bt_icon_layer);
  
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  
  layer_destroy(s_battery_layer);
}
  
static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  
  // Register with Event Services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_callback);

  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });
}

static void deinit() {
  window_destroy(s_main_window);
}

/////////  Recieved Settings from JS ////////////////////
void in_received_handler(DictionaryIterator *received, void *context) {
  
  bt_toggle = 0;
  vibe_on_connect = 0;
  vibe_on_disconnect = 0;
  
  // incoming message received
  Tuple *date_tuple = dict_find(received, KEY_DATE_FORMAT);
  Tuple *bt_tuple = dict_find(received, KEY_BT_FORMAT);
  Tuple *vibe_on_connect_t = dict_find(received, KEY_VIBE_ON_CONNECT); // int8
  Tuple *vibe_on_disconnect_t = dict_find(received, KEY_VIBE_ON_DISCONNECT); // int8
  
  
  ////// Colour Scheme
    Tuple *color_red_t = dict_find(received, KEY_COLOR_RED);
    Tuple *color_green_t = dict_find(received, KEY_COLOR_GREEN);
    Tuple *color_blue_t = dict_find(received, KEY_COLOR_BLUE);
  
    if(color_red_t && color_green_t && color_blue_t) {
    // Apply the color if available
    #if defined(PBL_BW)
      text_layer_set_text_color(s_time_layer, GColorWhite);
    #elif defined(PBL_COLOR)
      int red = color_red_t->value->int32;
      int green = color_green_t->value->int32;
      int blue = color_blue_t->value->int32;

    // Persist values
      persist_write_int(KEY_COLOR_RED, red);
      persist_write_int(KEY_COLOR_GREEN, green);
      persist_write_int(KEY_COLOR_BLUE, blue);

       time_color = GColorFromRGB(red, green, blue);      
      text_layer_set_text_color(s_time_layer, time_color);
    #endif
  }
  
  // Set date format either MM DD or DD MM
   if (date_tuple) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_DATE_FORMAT received!");

  	if (strcmp(date_tuple->value->cstring, "edate") == 0) {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using European date");
  		euro_date = 1;
  		persist_write_int(KEY_DATE_FORMAT, euro_date);
  	} else {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using US date");
  		euro_date = 0;
  		persist_write_int(KEY_DATE_FORMAT, euro_date);
  	}     
   }
  
  // Set if BT Symbol is shown when phone is connected
  if (bt_tuple){
      if (strcmp(bt_tuple->value->cstring, "on") == 0) {        
        bt_toggle = 1;
        persist_write_int(KEY_BT_FORMAT, bt_toggle);
      } else {
        APP_LOG(APP_LOG_LEVEL_INFO, "Showing Bluetooth connection symbol"); 
        bt_toggle = 0;
        persist_write_int(KEY_BT_FORMAT, bt_toggle);
      }
    }  
  
  // Vibration settings
   if (vibe_on_connect_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_VIBE_ON_CONNECT received!");
  	vibe_on_connect = vibe_on_connect_t->value->int8;
    persist_write_int(KEY_VIBE_ON_CONNECT, vibe_on_connect);
  }

  if (vibe_on_disconnect_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_VIBE_ON_DISCONNECT received!");
  	vibe_on_disconnect = vibe_on_disconnect_t->value->int8;
    persist_write_int(KEY_VIBE_ON_DISCONNECT, vibe_on_disconnect);
  }
  
  update_time();
  bluetooth_callback(connection_service_peek_pebble_app_connection());
}


int main(void) {
  init();
  
   APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_main_window);

  //register for messages
  app_message_register_inbox_received(in_received_handler);
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
  
  app_event_loop();
  deinit();
}
