// #pragma once
#include <pebble.h>

#undef APP_LOG
#define APP_LOG(level, fmt, args... )

#define FastTest false

static uint32_t GetImageByNumber(int number, bool isBlack){
  if(!isBlack){
	  switch(number){
	  case 1:
		return RESOURCE_ID_IMAGE_NUMBER_1;
		break;
	  case 2:
		return RESOURCE_ID_IMAGE_NUMBER_2;
		break;
	  case 3:
		return RESOURCE_ID_IMAGE_NUMBER_3;
		break;
	  case 4:
		return RESOURCE_ID_IMAGE_NUMBER_4;
		break;
	  case 5:
		return RESOURCE_ID_IMAGE_NUMBER_5;
		break;
	  case 6:
		return RESOURCE_ID_IMAGE_NUMBER_6;
		break;
	  case 7:
		return RESOURCE_ID_IMAGE_NUMBER_7;
		break;
	  case 8:
		return RESOURCE_ID_IMAGE_NUMBER_8;
		break;
	  case 9:
		return RESOURCE_ID_IMAGE_NUMBER_9;
		break;
	  case 0:
	  default:
		return RESOURCE_ID_IMAGE_NUMBER_0;
		break;
	  }
  }else{
	  switch(number){
	  case 1:
		return RESOURCE_ID_IMAGE_BLACK1;
		break;
	  case 2:
		return RESOURCE_ID_IMAGE_BLACK2;
		break;
	  case 3:
		return RESOURCE_ID_IMAGE_BLACK3;
		break;
	  case 4:
		return RESOURCE_ID_IMAGE_BLACK4;
		break;
	  case 5:
		return RESOURCE_ID_IMAGE_BLACK5;
		break;
	  case 6:
		return RESOURCE_ID_IMAGE_BLACK6;
		break;
	  case 7:
		return RESOURCE_ID_IMAGE_BLACK7;
		break;
	  case 8:
		return RESOURCE_ID_IMAGE_BLACK8;
		break;
	  case 9:
		return RESOURCE_ID_IMAGE_BLACK9;
		break;
	  case 0:
	  default:
		return RESOURCE_ID_IMAGE_BLACK0;
		break;
	  }
  }
}

