#ifndef AIRPORT_H
#define AIRPORT_H
#include "config.h"

void init_airport();
void destroy_airport();

void request_landing(int id, FlightType type);
void release_landing(int id, FlightType type);

void request_disembark(int id, FlightType type);
void release_disembark(int id, FlightType type);

void request_takeoff(int id, FlightType type);
void release_takeoff(int id, FlightType type);

#endif