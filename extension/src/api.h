//
//  api.h
//  LightenUpWorld
//
//  Created by User on 13/06/2018.
//  Copyright © 2018 User. All rights reserved.
//

#ifndef api_h
#define api_h

//OPENWEATHERMAP API
#define LATITUDE_START 21
#define LONGITUDE_START 29

#define WEATHER_HOST "api.openweathermap.org"
#define WEATHER_PATH "data/2.5/weather?lat=000&lon=000&appid=6ee4372288ed6d49c7dea5ed1f39a118"

//GENERAL

typedef struct geolocation{
    int latitude;
    int longitude;
    int value;
} geolocation_t;

typedef struct pixel{
    int x;
    int y;
}pixel_t;

typedef struct location_map{
    geolocation_t *loc;
    pixel_t *pix;
}location_map_t;

#endif /* api_h */
