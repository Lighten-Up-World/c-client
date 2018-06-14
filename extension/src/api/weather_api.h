//
// Created by User on 14/06/2018.
//

#ifndef weather_api_h
#define weather_api_h

#include "../apimanager.h"

#define WEATHER_HOST "api.openweathermap.org"
#define WEATHER_PATH "data/2.5/weather?lat=%f&lon=%f&appid=6ee4372288ed6d49c7dea5ed1f39a118"

//TEMP
int temp_construct(api_manager_t *self);
int temp_destruct(api_manager_t *self);
int temp_get_pixel_for_xy(api_manager_t *self, pixel_t *pixel, void *obj);

//WIND SPEED
int windspeed_construct(api_manager_t *self);
int windspeed_destruct(api_manager_t *self);
int windspeed_get_pixel_for_xy(pixel_t *pixel, void *obj);


#endif
