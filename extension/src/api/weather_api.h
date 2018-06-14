//
// Created by User on 14/06/2018.
//

#ifndef weather_api_h
#define weather_api_h

#include "../apimanager.h"


#define WEATHER_HOST_2 "api.darksky.net"
#define WEATHER_PATH_2 "forecast/bf050bc7f491ed6167290c1fe09c95d7/%f,%f"

#define WEATHER_HOST "api.openweathermap.org"
#define WEATHER_PATH "data/2.5/weather?lat=%f&lon=%f&appid=6ee4372288ed6d49c7dea5ed1f39a118"

//TEMP


int temp_get_pixel_for_xy(pixel_t *pixel);

//WIND SPEED

int windspeed_get_pixel_for_xy(pixel_t *pixel);


#endif
