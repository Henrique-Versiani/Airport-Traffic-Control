#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "airport.h"
#include "logger.h"
#include "config.h"

static sem_t runways;
static sem_t gates;
static sem_t tower;

static pthread_mutex_t priority_lock;
static pthread_cond_t domestic_can_proceed;
static int international_waiting_count = 0;

void airport_init(void) {
    sem_init(&runways, 0, NUM_RUNWAYS);
    sem_init(&gates, 0, NUM_GATES);
    sem_init(&tower, 0, TOWER_CAPACITY);

    pthread_mutex_init(&priority_lock, NULL);
    pthread_cond_init(&domestic_can_proceed, NULL);

    write_log("INFO: Modulo do aeroporto inicializado.\n");
}

void airport_destroy(void) {
    sem_destroy(&runways);
    sem_destroy(&gates);
    sem_destroy(&tower);

    pthread_mutex_destroy(&priority_lock);
    pthread_cond_destroy(&domestic_can_proceed);

    write_log("INFO: Modulo do aeroporto finalizado.\n");
}

static void domestic_wait_with_priority(PlaneData* data) {
    struct timespec timeout;
    int wait_result = 0;

    pthread_mutex_lock(&priority_lock);

    data->init_time_wait = time(NULL);

    while (international_waiting_count > 0) {
        char log_buffer[256];
        snprintf(log_buffer, sizeof(log_buffer), "PRIORIDADE: Voo DOM %d esperando. %d voo(s) INTL na frente.\n", data->id, international_waiting_count);
        write_log(log_buffer);
        
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 10;

        wait_result = pthread_cond_timedwait(&domestic_can_proceed, &priority_lock, &timeout);

        if (wait_result != 0) {
            time_t elapsed_time = time(NULL) - data->init_time_wait;

            if (elapsed_time > FALL_WAIT_TIME) {
                snprintf(log_buffer, sizeof(log_buffer), "STARVATION: Voo DOM %d caiu por excesso de tempo de espera! (%lds)\n", data->id, elapsed_time);
                write_log(log_buffer);
                pthread_mutex_unlock(&priority_lock);
                pthread_exit(NULL); 
            } else if (elapsed_time > ALERT_WAIT_TIME) {
                snprintf(log_buffer, sizeof(log_buffer), "ALERTA: Voo DOM %d em estado critico de espera! (%lds)\n", data->id, elapsed_time);
                write_log(log_buffer);
            }
        }
    }

    pthread_mutex_unlock(&priority_lock);
}

// --- POUSO ---

void airport_request_landing_resources(PlaneData* data) {
    if (data->type == DOMESTIC) {
        domestic_wait_with_priority(data);
        sem_wait(&tower);
        sem_wait(&runways);
    } else { // INTERNATIONAL
        pthread_mutex_lock(&priority_lock);
        international_waiting_count++;
        pthread_mutex_unlock(&priority_lock);

        sem_wait(&runways);
        sem_wait(&tower);

        pthread_mutex_lock(&priority_lock);
        international_waiting_count--;
        if (international_waiting_count == 0) {
            pthread_cond_broadcast(&domestic_can_proceed);
        }
        pthread_mutex_unlock(&priority_lock);
    }
}

void airport_release_landing_resources() {
    sem_post(&runways);
    sem_post(&tower);
}

// --- DESEMBARQUE ---

void airport_request_disembarking_resources(PlaneData* data) {
    if (data->type == DOMESTIC) {
        domestic_wait_with_priority(data);
        sem_wait(&tower);
        sem_wait(&gates);
    } else { // INTERNATIONAL
        pthread_mutex_lock(&priority_lock);
        international_waiting_count++;
        pthread_mutex_unlock(&priority_lock);

        sem_wait(&gates);
        sem_wait(&tower);

        pthread_mutex_lock(&priority_lock);
        international_waiting_count--;
        if (international_waiting_count == 0) {
            pthread_cond_broadcast(&domestic_can_proceed);
        }
        pthread_mutex_unlock(&priority_lock);
    }
}

void airport_release_disembarking_resources() {
    sem_post(&gates);
    sem_post(&tower);
}

// --- DECOLAGEM ---

void airport_request_takeoff_resources(PlaneData* data) {
    if (data->type == DOMESTIC) {
        domestic_wait_with_priority(data);
        sem_wait(&gates);
        sem_wait(&runways);
        sem_wait(&tower);
    } else { // INTERNATIONAL
        pthread_mutex_lock(&priority_lock);
        international_waiting_count++;
        pthread_mutex_unlock(&priority_lock);

        sem_wait(&tower);
        sem_wait(&gates);
        sem_wait(&runways);

        pthread_mutex_lock(&priority_lock);
        international_waiting_count--;
        if (international_waiting_count == 0) {
            pthread_cond_broadcast(&domestic_can_proceed);
        }
        pthread_mutex_unlock(&priority_lock);
    }
}

void airport_release_takeoff_resources() {
    sem_post(&gates);
    sem_post(&runways);
    sem_post(&tower);
}