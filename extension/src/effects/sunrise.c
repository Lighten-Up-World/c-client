#include "sunrise.h"

// NOTE: All times requested by sunrise api are in UTC

/***
 * GET SUNRISE/SUNSET INFO FOR X AND Y COORDS
 * Performs common processing for the get_pixel functions.
 *
 * @param pixel - pixel to write grid values to
 * @param geoloc - geolocation to request data from
 * @param attr - name of attribute being retrieved
 * @param object - object that attribute is contained in
 * @param val - double pointer to write value to
 * @return 0 for success; -1 for failure
 */
int get_sun_data_for_xy(opc_pixel_t *pixel, geolocation_t geoloc, char *attr, char *object, double *val) {
	assert(pixel != NULL);
  assert(attr != NULL);
  assert(object != NULL);

	int sockfd = socket_connect(SUNRISE_HOST, 80 /* PORT NUM*/);

	printf("Latitude: %f, Longitude: %f, ", geoloc.latitude, geoloc.longitude);

	if (get_value_for_geolocation(sockfd,&geoloc, SUNRISE_HOST, SUNRISE_PATH,
																0, attr, object, val) < 0){
    return -1;
  }

	socket_close(sockfd);

  return 0;
}

/**
 * SET PIXEL INTENSITY BASED ON CURRENT TIME
 *
 * @param pixel - pixel to set rgb values of
 * @param geoloc - geolocation of pixel
 * may need more params such as current time/sunrise data
 */
int sunrise_get_pixel_for_xy(opc_pixel_t *pixel, geolocation_t geoloc) {
  assert(pixel != NULL);

	// fetch data

	int red = 0;
	int blue = 0;
	int green = 0;

	// if (/* current time > sunrise time || current time < sunset time */) {
	// 	/*
	// 	gradually increase rgb as time gets closer to solar noon
	// 	requires function calcTimeDiff()
	// 	calcTimeDiff() returns int between 0 and 255 that gives rgb values as a
	// 	function of the time difference from the solalr noon
	// 	*/
	// }

	pixel->b = blue;
  pixel->g = green;
  pixel->r = red;
	return 0;
}
