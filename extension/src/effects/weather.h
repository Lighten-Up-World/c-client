//
// Created by User on 14/06/2018.
//

#ifndef WEATHER_H
#define WEATHER_H

#include "../apimanager.h"
#include "../effect_runner.h"

#define WEATHER_HOST_2 "api.darksky.net"
#define WEATHER_PATH_2 "forecast/bf050bc7f491ed6167290c1fe09c95d7/%f,%f"

#define WILL_OWM_API_KEY "6ee4372288ed6d49c7dea5ed1f39a118"
#define DANIEL_OWM_API_KEY "b9442812ef8ec5f8f52d4cca809b36b9"

#define WEATHER_HOST "api.openweathermap.org"
#define WEATHER_PATH "data/2.5/weather?lat=%f&lon=%f&appid=%s"

#define TEMP_TIMELAPSE_FILE "temp_timelapse.txt"

//TEMPERATURE
int temp_get_pixel(effect_runner_t *self, int pos);
effect_t *get_temp_effect(void);

int temp_timelapse_get_pixel(effect_runner_t *self, int pos);
effect_t *get_temp_timelapse_effect(void);

//WIND SPEED
int windspeed_get_pixel(effect_runner_t *self, int pos);
effect_t *get_windspeed_effect(void);

#endif
