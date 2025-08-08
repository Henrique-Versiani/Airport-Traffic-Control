#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "config.h"
#include "logger.h"
#include "airport.h"
#include "plane.h"

static volatile int simulation_is_running = 1;

void* simulation_controller_thread(void* arg) {
    int plane_id_counter = 0;

    while (simulation_is_running) {
        plane_id_counter++;

        PlaneData* plane_data = (PlaneData*) malloc(sizeof(PlaneData));
        if (plane_data == NULL) {
            write_log("ERRO: Falha ao alocar memoria para um novo aviao.\n");
            continue;
        }

        plane_data->id = plane_id_counter;
        plane_data->type = (rand() % 2 == 0) ? DOMESTIC : INTERNATIONAL;
        
        pthread_t plane_thread_id;
        if (pthread_create(&plane_thread_id, NULL, plane_routine, (void*)plane_data) != 0) {
            char log_buffer[100];
            snprintf(log_buffer, sizeof(log_buffer), "ERRO: Falha ao criar a thread para o Voo %d.\n", plane_id_counter);
            write_log(log_buffer);
            free(plane_data);
        } else {
            pthread_detach(plane_thread_id);
        }
        sleep((rand() % 3) + 1);
    }

    write_log("CONTROLE: Loop de criacao de avioes finalizado.\n");
    return NULL;
}

int main(void) {
    srand(time(NULL));

    if (logger_init("simulation.log") != 0) {
        fprintf(stderr, "ERRO CRITICO: Nao foi possivel inicializar o logger. Saindo.\n");
        return 1;
    }
    
    airport_init();

    write_log("====================================================\n");
    write_log("INFO: Sistema inicializado. Iniciando simulacao...\n");
    write_log("====================================================\n");

    pthread_t controller_tid;

    pthread_create(&controller_tid, NULL, simulation_controller_thread, NULL);

    char log_buffer[100];
    snprintf(log_buffer, sizeof(log_buffer), "INFO: Simulacao rodando por %d segundos.\n", TOTAL_SIMULATION_TIME);
    write_log(log_buffer);
    
    sleep(TOTAL_SIMULATION_TIME);

    write_log("INFO: Tempo de simulacao encerrado. Finalizando a criacao de novos voos...\n");

    simulation_is_running = 0;

    pthread_join(controller_tid, NULL);
    write_log("INFO: Thread de controle finalizada. Os voos existentes continuarao ate o fim de suas operacoes.\n");
    sleep(15);
    airport_destroy();
    write_log("INFO: Simulacao finalizada.\n");
    logger_close();
    printf("Simulacao finalizada. Verifique o arquivo 'simulation.log' para detalhes.\n");

    return 0;
}