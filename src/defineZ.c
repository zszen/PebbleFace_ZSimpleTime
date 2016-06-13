#include <pebble.h>

static uint32_t getDigitalImageByNumber(int number){
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
}