#ifndef PLANE_H
#define PLANE_H

#include <time.h>
#include "config.h"


typedef struct {
    int id;
    FlightType type;
    time_t init_time_wait; 

} PlaneData;


void* plane_routine(void* arg);

#endif