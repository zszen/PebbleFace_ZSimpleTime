#include <pebble.h>
#include "defineZ.c"
#include "ZApi.h"

typedef enum SAVEKEY{
	SAVEKEY_IS_HIGH_CONSTRACT,
	SAVEKEY_DURING_LIGHT_TIME,
	SAVEKEY_IS_COLORFUL,
	SAVEKEY_IS_ANIMATION_TURNING,
}SAVEKEY;

typedef enum TIMELIGHT{
	TIMELIGHT_3SEC,
	TIMELIGHT_5SEC,
	TIMELIGHT_8SEC,
	TIMELIGHT_15SEC,
	TIMELIGHT_30SEC,
	TIMELIGHT_1MIN,
	TIMELIGHT_2MIN,
	TIMELIGHT_5MIN,
	TIMELIGHT_10MIN,
	TIMELIGHT_30MIN,
	TIMELIGHT_1HOUR,
	TIMELIGHT_ALWAYS,//always highlight
	TIMELIGHT_SHAKETURN,//shake highlight, again darkness
}TIMELIGHT;

#define MOTION_ENABLE 1

static Window *s_main_window;
static GSize s_windowSize, s_windowSizeHalf, s_numberSize;
static BitmapLayer *layer_number_hourTen, *layer_number_hourBit;
static int time_hour_old;
static bool isHighConstract;
static bool is24H;
static AppTimer *appTimer;
static Animation* anmHourTen,*anmHourBit;
static bool isMotion;
static GColor8 currentBgColor;

static bool configDetecting[4];
static bool config_isHighConstract,config_isColorful,config_isAnimationTurning;
static TIMELIGHT config_duringLightTime;
static bool statusShakeTurnOn = false;

int fast_sec;
int fast_min;
int fast_hour;

static void initParam(){
	fast_hour = 22;
	fast_min = 0;
	fast_sec = 0;
	
	anmHourTen = NULL;
	anmHourBit = NULL;
	isHighConstract = false;
	is24H = clock_is_24h_style();
	appTimer = NULL;
	isMotion = false;

	configDetecting[SAVEKEY_IS_HIGH_CONSTRACT] = false;
	configDetecting[SAVEKEY_DURING_LIGHT_TIME] = false;
	configDetecting[SAVEKEY_IS_COLORFUL] = false;
	configDetecting[SAVEKEY_IS_ANIMATION_TURNING] = false;

	if (persist_exists(SAVEKEY_IS_HIGH_CONSTRACT) == true) {
		config_isHighConstract = persist_read_int(SAVEKEY_IS_HIGH_CONSTRACT)!=0;
	}else{
		config_isHighConstract = false;
	}

	if (persist_exists(SAVEKEY_DURING_LIGHT_TIME) == true) {
		config_duringLightTime = persist_read_int(SAVEKEY_DURING_LIGHT_TIME);
	}else{
		config_duringLightTime = false;
	}

	if (persist_exists(SAVEKEY_IS_COLORFUL) == true) {
		config_isColorful = persist_read_int(SAVEKEY_IS_COLORFUL)!=0;
	}else{
		config_isColorful = true;
	}

	if (persist_exists(SAVEKEY_IS_ANIMATION_TURNING) == true) {
		config_isAnimationTurning = persist_read_int(SAVEKEY_IS_ANIMATION_TURNING)!=0;
	}else{
		config_isAnimationTurning = true;
	}
}

static void saveParam(){
	if(configDetecting[SAVEKEY_IS_HIGH_CONSTRACT]){
		configDetecting[SAVEKEY_IS_HIGH_CONSTRACT] = false;
		persist_write_int(SAVEKEY_IS_HIGH_CONSTRACT, config_isHighConstract);
	}
	if(configDetecting[SAVEKEY_DURING_LIGHT_TIME]){
		configDetecting[SAVEKEY_DURING_LIGHT_TIME] = false;
		persist_write_int(SAVEKEY_DURING_LIGHT_TIME, config_duringLightTime);
	}
	if(configDetecting[SAVEKEY_IS_COLORFUL]){
		configDetecting[SAVEKEY_IS_COLORFUL] = false;
		persist_write_int(SAVEKEY_IS_COLORFUL, config_isColorful);
	}
	if(configDetecting[SAVEKEY_IS_ANIMATION_TURNING]){
		configDetecting[SAVEKEY_IS_ANIMATION_TURNING] = false;
		persist_write_int(SAVEKEY_IS_ANIMATION_TURNING, config_isAnimationTurning);
	}
}

static void setMotionMarkFalse(Animation *animation, bool finished, void *context  ){
	isMotion = false;
}

static void setMotionChangeHourTen(Animation *animation, void *context  ){
	APP_LOG(APP_LOG_LEVEL_WARNING, "setMotionChangeHourTen");
	time_t timeNow = time(NULL); 
	struct tm *timeLocal = localtime(&timeNow);
	
#if FastTest
	if(fast_min==0){
		int hourAuto1224 = fast_hour;
		if(!is24H && hourAuto1224>12){
			hourAuto1224%=12;
		}
		bool isNeedChangeHourTenPos = time_hour_old/10 != fast_hour/10;
		time_hour_old = fast_hour;
#else 
	if(timeLocal->tm_min==0){
		int hourAuto1224 = timeLocal->tm_hour;
		if(!is24H && hourAuto1224>12){
			hourAuto1224%=12;
		}
		bool isNeedChangeHourTenPos = time_hour_old/10 != timeLocal->tm_hour/10;
		time_hour_old = timeLocal->tm_hour;
#endif
		if(isNeedChangeHourTenPos){
			ZAPI_ImageChange(layer_number_hourTen, GetImageByNumber(hourAuto1224/10,isHighConstract));
		}
	}
}

static void setMotionChangeHourBit(Animation *animation, void *context  ){
	APP_LOG(APP_LOG_LEVEL_WARNING, "setMotionChangeHourBit");
	time_t timeNow = time(NULL); 
	struct tm *timeLocal = localtime(&timeNow);
#if FastTest
	if(fast_min==0){
		int hourAuto1224 = fast_hour;
		ZAPI_ImageChange(layer_number_hourBit, GetImageByNumber(hourAuto1224%10,isHighConstract));
#else
	if(timeLocal->tm_min==0){
		int hourAuto1224 = timeLocal->tm_hour;
#endif
		if(!is24H && hourAuto1224>12){
			hourAuto1224%=12;
		}
		ZAPI_ImageChange(layer_number_hourBit, GetImageByNumber(hourAuto1224%10,isHighConstract));
	}
}

static int32_t tweenEase_custom_backIn(int32_t progress) {
	float s = 1.20158f;
	float t = progress/(float)ANIMATION_NORMALIZED_MAX;
	return t*t*((s+1)*t - s)*ANIMATION_NORMALIZED_MAX;
}
	
static int32_t tweenEase_custom_backOut(int32_t progress) {
	float s = 1.20158f;
	float t = progress/(float)ANIMATION_NORMALIZED_MAX - 1;
	return (t*t*((s+1)*t + s) + 1)*ANIMATION_NORMALIZED_MAX;
}

static void motionHourStop(){
	if(!isMotion){
		return;
	}
	isMotion = false;
	if(anmHourTen!=NULL){
		animation_unschedule(anmHourTen);
		anmHourTen= NULL;
	}
	if(anmHourBit!=NULL){
		animation_unschedule(anmHourBit);
		anmHourBit= NULL;
	}
}

static void motionHourFadeIn(GRect rectTen, GRect rectBit){
	motionHourStop();
	isMotion = true;
	Layer* layerHourTen = bitmap_layer_get_layer(layer_number_hourTen);
	Layer* layerHourBit = bitmap_layer_get_layer(layer_number_hourBit);
	GRect rectFrom = GRect(rectTen.origin.x, -s_numberSize.h, rectTen.size.w, rectTen.size.h);
	layer_set_frame(layerHourTen, rectFrom);
	//
	PropertyAnimation* prop_anm = property_animation_create_layer_frame(layerHourTen, &rectFrom, &rectTen);
	anmHourTen = property_animation_get_animation(prop_anm);
	int delay_ms = 500;
	int during_ms = 500;
// 	animation_set_curve(anmHourTen, AnimationCurveEaseOut);
	animation_set_custom_curve(anmHourTen, tweenEase_custom_backOut);
	animation_set_delay(anmHourTen, delay_ms);
	animation_set_duration(anmHourTen, during_ms);
	animation_schedule(anmHourTen);
	//
	rectFrom = GRect(rectBit.origin.x, -s_numberSize.h, rectBit.size.w, rectBit.size.h);
	layer_set_frame(layerHourBit, rectFrom);
	prop_anm = property_animation_create_layer_frame(layerHourBit, &rectFrom, &rectBit);
	anmHourBit = property_animation_get_animation(prop_anm);
	delay_ms = 800;
	during_ms = 500;
// 	animation_set_curve(anmHourBit, AnimationCurveEaseOut);
	animation_set_custom_curve(anmHourBit, tweenEase_custom_backOut);
	animation_set_delay(anmHourBit, delay_ms);
	animation_set_duration(anmHourBit, during_ms);
	animation_set_handlers(anmHourBit, (AnimationHandlers) {
		.started = NULL,
		.stopped = setMotionMarkFalse
	}, NULL);
	animation_schedule(anmHourBit);
}

static void motionHourFadeOutAndIn(GRect rectTen, GRect rectBit){
	motionHourStop();
	isMotion = true;
	Layer* layerHourTen = bitmap_layer_get_layer(layer_number_hourTen);
	Layer* layerHourBit = bitmap_layer_get_layer(layer_number_hourBit);
	GRect rectFrom,rectEnd;
	PropertyAnimation* prop_anm;
	int delay_ms;
	int during_ms;
	//
	{
		rectFrom = layer_get_frame(layerHourTen);
		rectEnd = GRect(rectFrom.origin.x, s_windowSize.h+s_numberSize.h, rectFrom.size.w, rectFrom.size.h);
		prop_anm = property_animation_create_layer_frame(layerHourTen, &rectFrom, &rectEnd);
		Animation* anm1 = property_animation_get_animation(prop_anm);
		delay_ms = 200;
		during_ms = 350;
// 		animation_set_curve(anm1, AnimationCurveEaseIn);
		animation_set_custom_curve(anm1, tweenEase_custom_backOut);
		animation_set_delay(anm1, delay_ms);
		animation_set_duration(anm1, during_ms);
		//
		rectFrom = GRect(rectTen.origin.x, -s_numberSize.h, rectTen.size.w, rectTen.size.h);
		prop_anm = property_animation_create_layer_frame(layerHourTen, &rectFrom, &rectTen);
		Animation* anm2 = property_animation_get_animation(prop_anm);
		during_ms = 500;
// 		animation_set_curve(anm2, AnimationCurveEaseOut);
		animation_set_custom_curve(anm2, tweenEase_custom_backOut);
		animation_set_duration(anm2, during_ms);
		animation_set_handlers(anm2, (AnimationHandlers) {
			.started = setMotionChangeHourTen,
			.stopped = NULL
		}, NULL);
		//
		anmHourTen = animation_sequence_create(anm1, anm2, NULL);
		animation_schedule(anmHourTen);
	}
	{
		rectFrom = layer_get_frame(layerHourBit);
		rectEnd = GRect(rectFrom.origin.x, s_windowSize.h+s_numberSize.h, rectFrom.size.w, rectFrom.size.h);
		prop_anm = property_animation_create_layer_frame(layerHourBit, &rectFrom, &rectEnd);
		Animation* anm1 = property_animation_get_animation(prop_anm);
		delay_ms = 400;
		during_ms = 350;
// 		animation_set_curve(anm1, AnimationCurveEaseIn);
		animation_set_custom_curve(anm1, tweenEase_custom_backIn);
		animation_set_delay(anm1, delay_ms);
		animation_set_duration(anm1, during_ms);
		//
		rectFrom = GRect(rectBit.origin.x, -s_numberSize.h, rectBit.size.w, rectBit.size.h);
		prop_anm = property_animation_create_layer_frame(layerHourBit, &rectFrom, &rectBit);
		Animation* anm2 = property_animation_get_animation(prop_anm);
		during_ms = 500;
// 		animation_set_curve(anm2, AnimationCurveEaseOut);
		animation_set_custom_curve(anm2, tweenEase_custom_backIn);
		animation_set_duration(anm2, during_ms);
		animation_set_handlers(anm2, (AnimationHandlers) {
			.started = setMotionChangeHourBit,
			.stopped = setMotionMarkFalse
		}, NULL);
		//
		anmHourBit = animation_sequence_create(anm1, anm2, NULL);
		animation_schedule(anmHourBit);
	}
}

static void changeConstract(bool _isHighConstract){
	int hourAuto1224 = time_hour_old;
	if(!is24H && hourAuto1224>12){
		hourAuto1224%=12;
	}
	if(isHighConstract != _isHighConstract){
		isHighConstract = _isHighConstract;
		ZAPI_ImageChange(layer_number_hourBit, GetImageByNumber(hourAuto1224%10,isHighConstract));
		ZAPI_ImageChange(layer_number_hourTen, GetImageByNumber(hourAuto1224/10,isHighConstract));
	}
	GColor8 color = GColorBlack;
	if(isHighConstract){
		// 		https://developer.pebble.com/guides/tools-and-resources/color-picker/#005555
		if(config_isColorful){
			if(time_hour_old<4){
				color = GColorDarkGray;
				_isHighConstract = false;
			}else if(time_hour_old<6){
				color = GColorMidnightGreen;
				_isHighConstract = false;
			}else if(time_hour_old<8){
				color = GColorVividCerulean;
			}else if(time_hour_old<9){
				color = GColorCyan;
			}else if(time_hour_old<11){
				color = GColorSpringBud;
			}else if(time_hour_old<12){
				color = GColorPastelYellow;
			}else if(time_hour_old<15){
				color = GColorYellow;
			}else if(time_hour_old<16){
				color = GColorRajah;
			}else if(time_hour_old<17){
				color = GColorMelon;
			}else if(time_hour_old<18){
				color = GColorFashionMagenta;
			}else if(time_hour_old<20){
				color = GColorPurple;
				_isHighConstract = false;
			}else if(time_hour_old<22){
				color = GColorImperialPurple;
				_isHighConstract = false;
			}else{
				color = GColorOxfordBlue;
				_isHighConstract = false;
			}
		}else{
			color = GColorLightGray;
		}
	}
	if(currentBgColor.argb != color.argb){
		currentBgColor = color;
		window_set_background_color(s_main_window, color);
	}
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	if(isMotion){
		APP_LOG(APP_LOG_LEVEL_WARNING, "is motioning");
		return;
	}
	bool isHourChanged = units_changed & HOUR_UNIT;
#if FastTest
	bool isInNight = false;
#else
	bool isInNight = (tick_time->tm_hour>=22 || tick_time->tm_hour<6);
	if(isInNight && tick_time->tm_min%15!=0){
		APP_LOG(APP_LOG_LEVEL_WARNING, "night power save");
		return;
	}else if(tick_time->tm_min%5!=0){
		return;
	}
	APP_LOG(APP_LOG_LEVEL_WARNING, "update");
	time_t timeNow = time(NULL); 
	struct tm *timeLocal = localtime(&timeNow);
#endif
#if FastTest
	fast_min+=5;
	if(fast_min>=60){
		fast_min = 0;
		fast_hour++;
		isHourChanged = true;
		if(fast_hour>=24){
			fast_hour=0;
		}
	}
	APP_LOG(APP_LOG_LEVEL_WARNING, "fast_min %d:%d",fast_hour, fast_min);
	int offsetMinute = -((fast_min-30)/30.0)*s_windowSizeHalf.h;
#else
	int offsetMinute = -((timeLocal->tm_min-30)/30.0)*s_windowSizeHalf.h;
#endif
	//update minute position
	if(!isHourChanged){
		layer_set_frame(bitmap_layer_get_layer(layer_number_hourBit), GRect(s_windowSizeHalf.w, s_windowSizeHalf.h-offsetMinute, s_numberSize.w, s_numberSize.h));
	}else{
		if(!config_isAnimationTurning || isInNight){
			layer_set_frame(bitmap_layer_get_layer(layer_number_hourBit), GRect(s_windowSizeHalf.w, s_windowSizeHalf.h-offsetMinute, s_numberSize.w, s_numberSize.h));
		}
	}
	
#if FastTest
	if(fast_min==0){
		int offsetHour = -((fast_hour-12)/12.0)*s_windowSizeHalf.h;
#else
	if(units_changed & HOUR_UNIT){
		int offsetHour = -((timeLocal->tm_hour-12)/12.0)*s_windowSizeHalf.h;
#endif
		if(!isInNight && config_isAnimationTurning){
			//motion
			motionHourFadeOutAndIn(GRect(s_windowSizeHalf.w-s_numberSize.w, s_windowSizeHalf.h-offsetHour, s_numberSize.w, s_numberSize.h),GRect(s_windowSizeHalf.w, s_windowSizeHalf.h-offsetMinute, s_numberSize.w, s_numberSize.h));
		}else{
			layer_set_frame(bitmap_layer_get_layer(layer_number_hourTen), GRect(s_windowSizeHalf.w-s_numberSize.w, s_windowSizeHalf.h-offsetHour, s_numberSize.w, s_numberSize.h));
			//solid
			setMotionChangeHourTen(NULL,NULL);
			setMotionChangeHourBit(NULL,NULL);
		}
		if(isHighConstract && config_isColorful){
			changeConstract(isHighConstract);
		}
	}
}

static void sendSignalForInit(){
// 	APP_LOG(APP_LOG_LEVEL_WARNING, "request init %d %d %d", config_isHighConstract,config_duringLightTime,config_isColorful);
// 	// Begin dictionary
// 	DictionaryIterator *iter;
// 	app_message_outbox_begin(&iter);
// 	//     dict_write_uint8(iter, 0, 0);
// 	dict_write_int8(iter, MESSAGE_KEY_CONFIG_isHighConstract, config_isHighConstract);
// 	dict_write_int8(iter, MESSAGE_KEY_CONFIG_duringTime, config_duringLightTime);
// 	dict_write_int8(iter, MESSAGE_KEY_CONFIG_isColorful, config_isColorful);
// 	dict_write_cstring(iter, MESSAGE_KEY_TYPE, "init");
// 	// Send the message!
// 	app_message_outbox_send();
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void normalConstract(void *data){
	changeConstract(false);
	appTimer = NULL;
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction){
	bool isShakeTurn = false;
	int milsec = 0;
	switch(config_duringLightTime){
		case TIMELIGHT_3SEC:
		milsec = 3000;
		break;
		case TIMELIGHT_5SEC:
		milsec = 5000;
		break;
		case TIMELIGHT_15SEC:
		milsec = 15000;
		break;
		case TIMELIGHT_30SEC:
		milsec = 30000;
		break;
		case TIMELIGHT_1MIN:
		milsec = 60000;
		break;
		case TIMELIGHT_2MIN:
		milsec = 120000;
		break;
		case TIMELIGHT_5MIN:
		milsec = 300000;
		break;
		case TIMELIGHT_10MIN:
		milsec = 600000;
		break;
		case TIMELIGHT_30MIN:
		milsec = 1800000;
		break;
		case TIMELIGHT_1HOUR:
		milsec = 3600000;
		break;
		case TIMELIGHT_SHAKETURN:
			isShakeTurn = true;
		break;
		case TIMELIGHT_ALWAYS:
		default:
		break;
	}
	if(isShakeTurn){
		statusShakeTurnOn = !statusShakeTurnOn;
		changeConstract(statusShakeTurnOn);
	}else{
		changeConstract(true);
		if(appTimer==NULL){
			app_timer_register(milsec, normalConstract,NULL);
		}else{
			app_timer_reschedule(appTimer, milsec);
		}
	}
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context){
// 	Tuple *t_type = dict_find(iterator, MESSAGE_KEY_TYPE);
// 	if(!t_type){
// 		return;
// 	}
// 	const char* strType = t_type->value->cstring;
// 	APP_LOG(APP_LOG_LEVEL_WARNING, "request type %s", strType);
// 	if(!strcmp(strType,"ready")){//first request;
// 		sendSignalForInit();
// 	}else if(!strcmp(strType,"config")){
	
	Tuple *t_param = dict_find(iterator, MESSAGE_KEY_CONFIG_isHighConstract);
	if(t_param){
		config_isHighConstract = t_param->value->int8!=0;
		configDetecting[SAVEKEY_IS_HIGH_CONSTRACT] = true;
		APP_LOG(APP_LOG_LEVEL_WARNING, "config_isHighConstract %d",config_isHighConstract);
	}


	t_param = dict_find(iterator, MESSAGE_KEY_CONFIG_duringTime);
	if(t_param){
		config_duringLightTime = ZAPI_atoi(t_param->value->cstring);
		configDetecting[SAVEKEY_DURING_LIGHT_TIME] = true;
		APP_LOG(APP_LOG_LEVEL_WARNING, "config_duringLightTime %d",config_duringLightTime);
	}

	t_param = dict_find(iterator, MESSAGE_KEY_CONFIG_isColorful);
	if(t_param){
		config_isColorful = t_param->value->int8!=0;
		configDetecting[SAVEKEY_IS_COLORFUL] = true;
		APP_LOG(APP_LOG_LEVEL_WARNING, "config_isColorful %d",config_isColorful);
	}

	t_param = dict_find(iterator, MESSAGE_KEY_CONFIG_isAnimationTurning);
	if(t_param){
		config_isAnimationTurning = t_param->value->int8!=0;
		configDetecting[SAVEKEY_IS_ANIMATION_TURNING] = true;
		APP_LOG(APP_LOG_LEVEL_WARNING, "config_isAnimationTurning %d",config_isAnimationTurning);
	}

	if(config_duringLightTime==TIMELIGHT_ALWAYS){
		changeConstract(true);
		accel_tap_service_unsubscribe();
	}else if(config_isHighConstract){
		accel_tap_service_subscribe(accel_tap_handler);
	}else{
		accel_tap_service_unsubscribe();
		changeConstract(false);
	}

	saveParam();
	
// 	}
}

static void main_window_load(Window *window) {
	bool is24H = clock_is_24h_style();
	time_t timeNow = time(NULL); 
	struct tm *timeLocal = localtime(&timeNow);
	time_hour_old = timeLocal->tm_hour;
#if FastTest
	int offsetHour = -((fast_hour-12)/12.0)*s_windowSizeHalf.h;
	int offsetMinute = -((fast_min-30)/30.0)*s_windowSizeHalf.h;
#else
	int offsetHour = -((timeLocal->tm_hour-12)/12.0)*s_windowSizeHalf.h;
	int offsetMinute = -((timeLocal->tm_min-30)/30.0)*s_windowSizeHalf.h;
#endif
	//   int offsetSecond = -((timeLocal->tm_sec-30)/30.0)*s_windowSizeHalf.h;

	//   APP_LOG(APP_LOG_LEVEL_DEBUG, ">>>>>>>>%d %d", timeLocal->tm_hour,offsetHour);
	

	//   GBitmap* g_frame_bitmap;
	int hourAuto1224 = timeLocal->tm_hour;
#if FastTest
	hourAuto1224 = fast_hour;
#endif
	if(!is24H && hourAuto1224>12){
		hourAuto1224%=12;
	}
	if(config_isAnimationTurning){
		GRect rect1 = GRect(s_windowSizeHalf.w-s_numberSize.w, s_windowSizeHalf.h-offsetHour, s_numberSize.w, s_numberSize.h);
		layer_number_hourTen = ZAPI_ImageCreate(GetImageByNumber(hourAuto1224/10,isHighConstract), GRectZero, true);
		layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(layer_number_hourTen));
		GRect rect2 = GRect(s_windowSizeHalf.w, s_windowSizeHalf.h-offsetMinute, s_numberSize.w, s_numberSize.h);
		layer_number_hourBit = ZAPI_ImageCreate(GetImageByNumber(hourAuto1224%10,isHighConstract), GRectZero, true);
		layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(layer_number_hourBit));
		
		motionHourFadeIn(rect1, rect2);
	}else{
		GRect rect1 = GRect(s_windowSizeHalf.w-s_numberSize.w, s_windowSizeHalf.h-offsetHour, s_numberSize.w, s_numberSize.h);
		layer_number_hourTen = ZAPI_ImageCreate(GetImageByNumber(hourAuto1224/10,isHighConstract), rect1, true);
		layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(layer_number_hourTen));
		GRect rect2 = GRect(s_windowSizeHalf.w, s_windowSizeHalf.h-offsetMinute, s_numberSize.w, s_numberSize.h);
		layer_number_hourBit = ZAPI_ImageCreate(GetImageByNumber(hourAuto1224%10,isHighConstract), rect2, true);
		layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(layer_number_hourBit));
	}
	

	// 	update_time();
	// 	motionHourFadeOutAndIn(rect1, rect2);
}

static void main_window_unload(Window *window) {
	bitmap_layer_destroy(layer_number_hourTen);
	bitmap_layer_destroy(layer_number_hourBit);
	ZAPI_ImageDESTROYALL();
}


static void init() {
	initParam();
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
	//   update_time();

	// Register with TickTimerService
#if FastTest
		tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
#else
		tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
#endif

	if(config_duringLightTime==TIMELIGHT_ALWAYS){
		changeConstract(true);
	}else if(config_isHighConstract){
		accel_tap_service_subscribe(accel_tap_handler);
	}

	// Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);

	// Open AppMessage
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}

static void deinit() {
	accel_tap_service_unsubscribe();
	// Destroy Window
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}