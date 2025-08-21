# Simulador de Controle de Tráfego Aéreo

Este projeto é uma simulação de um sistema de controle de tráfego aéreo para um aeroporto internacional com alta demanda, desenvolvido em C com o uso de PThreads. O objetivo é gerenciar recursos limitados do aeroporto, lidar com problemas de concorrência como deadlock e starvation, e simular o fluxo de voos domésticos e internacionais, que possuem regras distintas para aquisição de recursos.

Este trabalho foi desenvolvido para a disciplina de Sistemas Operacionais da Universidade Federal de Pelotas (UFPel), ministrada pelo Prof. Rafael Burlamaqui Amaral.

## Conceitos Implementados

* **Multithreading**: Cada avião é representado por uma thread (`pthread`), permitindo a simulação de operações concorrentes.
* **Gerenciamento de Recursos**: Controle de acesso a recursos exclusivos (pistas, portões) e compartilhados (torre de controle) utilizando semáforos.
* **Detecção e Tratamento de Deadlock**: Um algoritmo de detecção monitora o estado dos recursos e, para resolver impasses, preempta recursos de um avião, que recebe prioridade para requisitá-los novamente. A principal fonte de deadlock é a ordem de alocação de recursos invertida entre voos domésticos e internacionais.
* **Prevenção de Starvation**: Implementação de um sistema de "aging" em uma fila de prioridade. Voos que esperam por muito tempo têm sua prioridade aumentada para garantir que eventualmente acessem os recursos.
* **Sincronização**: Uso de mutexes e variáveis de condição para garantir a segurança no acesso a estruturas de dados compartilhadas, como as filas de requisição e as matrizes do detector de deadlock.
* **Relatórios e Logging**: Geração de um log detalhado de eventos em tempo de execução e um relatório estatístico completo ao final da simulação.

## Dinâmica da Simulação

A simulação modela um aeroporto com os seguintes recursos configuráveis:
* **Torres de Controle**: Recurso compartilhado que pode atender a um número limitado de operações simultaneamente.
* **Pistas**: Utilizadas para pousos e decolagens.
* **Portões de Embarque**: Utilizados para o desembarque de passageiros.

### Ciclo de Vida de um Avião

Cada avião (thread) executa a seguinte sequência de operações:
1.  **Pouso**: Requer uma pista e a torre de controle.
2.  **Desembarque**: Após o pouso, libera os recursos anteriores e solicita um portão e a torre de controle.
3.  **Decolagem**: Após o desembarque, o avião aguarda para decolar, solicitando um portão, uma pista e a torre de controle.

### Ordem de Alocação de Recursos

Voos internacionais têm prioridade sobre os domésticos. A diferença na ordem de solicitação de recursos é a principal causa de deadlocks:

| Operação | Voos Internacionais (Ordem de Solicitação) | Voos Domésticos (Ordem de Solicitação) |
| :--- | :--- | :--- |
| **Pouso** | Pista → Torre | Torre → Pista |
| **Desembarque** | Portão → Torre | Torre → Portão |
| **Decolagem** | Portão → Pista → Torre | Torre → Portão → Pista |

### Tratamento de Starvation

Para evitar que voos domésticos fiquem indefinidamente em espera, a simulação monitora o tempo de espera de cada avião:
* **Alerta Crítico**: Entra em estado de alerta crítico após 60 segundos de espera por um recurso.
* **Falha Operacional**: Após 90 segundos de espera, a thread do avião é finalizada para simular uma falha operacional.

## Como Compilar e Executar

O projeto utiliza um `Makefile` para simplificar o processo de compilação e execução.

### Pré-requisitos
* GCC
* Biblioteca pthread

### Comandos

1.  **Compilar o projeto**:
    ```bash
    make
    ```
    Este comando compila os arquivos-fonte e cria o executável no diretório `bin/`.

2.  **Executar a simulação**:
    ```bash
    make run
    ```
    Este comando executa o programa com um conjunto de parâmetros pré-definido no `Makefile`.

3.  **Executar manualmente com parâmetros customizados**:
    O executável aceita parâmetros via linha de comando para configurar a simulação:
    ```bash
    ./bin/Airport-Traffic-Control <torres> <pistas> <portoes> <op_torres> <tempo_total> <alerta_critico> <falha>
    ```
    Exemplo:
    ```bash
    ./bin/Airport-Traffic-Control 1 3 5 2 120 60 90
    ```
    **Significado dos parâmetros**:
    * `1`: Número de torres de controle.
    * `3`: Número de pistas.
    * `5`: Número de portões de embarque.
    * `2`: Operações simultâneas por torre.
    * `120`: Tempo total da simulação em segundos.
    * `60`: Tempo para alerta crítico em segundos.
    * `90`: Tempo para falha operacional (starvation) em segundos.

4.  **Limpar arquivos compilados**:
    ```bash
    make clean
    ```
    Este comando remove o executável e os arquivos objeto gerados.

## Relatório Final

Ao final do tempo de simulação estipulado, o programa para de criar novos aviões, aguarda as operações em andamento terminarem e exibe um relatório final contendo:
* Um resumo com o estado final de cada avião.
* Estatísticas de operações bem-sucedidas e falhas.
* Contagem total de deadlocks detectados e falhas por starvation.