#ifndef PR3_PR3_H
#define PR3_PR3_H

#endif //PR3_PR3_H

#define FALSE 0
#define TRUE 1

// Stores passenger data
typedef struct passengers_data {
    int time_of_arrival;
    int ride_length;
    int wait_length;
    int origin_ID;
    int destination_ID;
    struct passengers_data *next;
}passenger;

// Stores Bus Status data
typedef struct bus_data {
    int current_ID; // Bus's current stop
    int num_passengers;
    passenger *riders;
}bus;

// Stores Bus Stop data
typedef struct bus_stops_data {
    int stop_ID; // Only used for printing bus stop number
    int num_waiting;
    passenger *waiting;
}stops;


// Get total bus stops per trip
int input_num_bus_stops(FILE *passenger_file);

// Print log file
void print(FILE *log_file, int num_bus_stops, int time_step, stops *stops, bus *bus_info);