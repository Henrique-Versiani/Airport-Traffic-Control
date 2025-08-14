#include <stdio.h>
#include <pthread.h>
#include "stats.h"

typedef struct {
    int successful_flights;
    int crashed_flights;
} SimulationStats;

static SimulationStats stats;

static pthread_mutex_t stats_mutex;

void stats_init(void) {
    stats.successful_flights = 0;
    stats.crashed_flights = 0;

    if (pthread_mutex_init(&stats_mutex, NULL) != 0) {
        perror("Falha ao inicializar o mutex das estatísticas");
    }
}

void stats_destroy(void) {
    pthread_mutex_destroy(&stats_mutex);
}

void stats_increment_successful_flights(void) {
    pthread_mutex_lock(&stats_mutex);
    
    stats.successful_flights++;
    
    pthread_mutex_unlock(&stats_mutex);
}

void stats_increment_crashed_flights(void) {
    pthread_mutex_lock(&stats_mutex);
    
    stats.crashed_flights++;
    
    pthread_mutex_unlock(&stats_mutex);
}

void stats_print_final_report(void) {
    pthread_mutex_lock(&stats_mutex);

    printf("\n");
    printf("====================================================\n");
    printf("---       Relatório Final da Simulação           ---\n");
    printf("====================================================\n");
    printf("Voos que finalizaram o ciclo com sucesso: %d\n", stats.successful_flights);
    printf("Voos que 'caíram' por starvation:         %d\n", stats.crashed_flights);
    printf("----------------------------------------------------\n");
    printf("\n");

    pthread_mutex_unlock(&stats_mutex);
}