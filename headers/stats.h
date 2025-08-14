#ifndef STATS_H
#define STATS_H

void stats_init(void);
void stats_destroy(void);
void stats_increment_successful_flights(void);
void stats_increment_crashed_flights(void);
void stats_print_final_report(void);

#endif // STATS_H