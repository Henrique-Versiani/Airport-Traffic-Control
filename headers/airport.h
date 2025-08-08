#ifndef AIRPORT_H
#define AIRPORT_H

#include "plane.h"

void airport_init(void);
void airport_destroy(void);

void airport_request_landing_resources(PlaneData* data);
void airport_release_landing_resources();

void airport_request_disembarking_resources(PlaneData* data);
void airport_release_disembarking_resources();

void airport_request_takeoff_resources(PlaneData* data);
void airport_release_takeoff_resources();


#endif // AIRPORT_H