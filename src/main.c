#include <pebble.h>
#include "main.h"
#include "config.c"

static Window *s_window;
static GFont s_res_gothic_14;
static TextLayer *s_textlayer;
static TextLayer *s_textlayer_instructions;
static AppTimer *s_timer;

/* UI */

static void initialise_ui(void) {
	s_window = window_create();
	window_set_fullscreen(s_window, true);
	
	s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
	
	s_textlayer = text_layer_create(GRect(10, 10, 124, 84));
	text_layer_set_text_alignment(s_textlayer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer);
	
	s_textlayer_instructions = text_layer_create(GRect(10, 104, 124, 54));
	text_layer_set_text(s_textlayer_instructions, "Click the select button to send message to mobile device");
	text_layer_set_text_alignment(s_textlayer_instructions, GTextAlignmentCenter);
	text_layer_set_font(s_textlayer_instructions, s_res_gothic_14);
	layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_instructions);
}

static void clear_message(void *data) {
	text_layer_set_text(s_textlayer, "");
}

static void destroy_ui(void) {
	window_destroy(s_window);
	text_layer_destroy(s_textlayer);
}

static void handle_window_unload(Window* window) {
	destroy_ui();
	
	app_timer_cancel(s_timer);
}

/* Communications */

void receive_message(Tuple *message_a) {
	text_layer_set_text(s_textlayer, message_a->value->cstring);
	
	// Clear messages after 3 seconds
	s_timer = app_timer_register(3000, clear_message, NULL);
}

void send_message() {	
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	
	if(iter == NULL) {
		return;
	}
	
	static char message[64] = "Hello, mobile app!";
	
	dict_write_cstring(iter, 0, message);
	dict_write_end(iter);
	
	app_message_outbox_send();
}

/* Event Handlers */

static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	send_message();
}

/* Window Management */

static void config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
}

void show_main(void) {
	initialise_ui();
	
	window_set_window_handlers(s_window, (WindowHandlers) {
		.unload = handle_window_unload,
	});
	
	window_stack_push(s_window, true);
	
	window_set_click_config_provider(s_window, config_provider);
}

void hide_main(void) {
	window_stack_remove(s_window, true);
}