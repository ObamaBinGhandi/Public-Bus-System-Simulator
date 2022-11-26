#include <stdio.h>
#include <stdlib.h>
#include "pr3.h"


int main(int argc, char *argv[]) {
    // Select inputs
    if (argc != 3) {
        printf("ERROR: Wrong number of arguments.\n");
        exit(0);
    }
    // Open files
    FILE *passenger_file;
    passenger_file = fopen(argv[1], "r");
    if (passenger_file == NULL) {
        printf("Invalid passenger file: %s.\n", argv[1]);
        exit(0);
    }
    FILE *log_file;
    log_file = fopen(argv[2], "w");
    if (log_file == NULL) {
        printf("Invalid log file: %s.\n", argv[2]);
        exit(0);
    }


    // --------------------------------------------------------------------------------------- //


    // Reads the first line of passenger file
    int num_bus_stops = input_num_bus_stops(passenger_file);


    // --------------------------------------------------------------------------------------- //


    // Allocate passenger linked list
    int arrival, origin, destination;
    passenger *head, *temp, *cur;
    head = (passenger *) calloc(1, sizeof(passenger));
    if (head == NULL){
        fprintf(log_file, "Unable to allocate memory.\n");
    } else {
        fscanf(passenger_file, "%d %d %d", &arrival, &origin, &destination);

        head->time_of_arrival = arrival;
        head->origin_ID = origin;
        head->destination_ID = destination;
        head->next = NULL;
        //printf("%d %d %d\n", head->time_of_arrival, head->origin_ID, head->destination_ID);
        temp = head;

        while (fscanf(passenger_file, "%d %d %d", &arrival, &origin, &destination) != EOF){
            cur = malloc(sizeof(passenger));
            if (cur == NULL){
                fprintf(log_file, "Unable to allocate memory.\n");
                break;
            } else {
                cur->time_of_arrival = arrival;
                cur->origin_ID = origin;
                cur->destination_ID = destination;
                cur->next = NULL;
                //printf("%d %d %d\n", cur->time_of_arrival, cur->origin_ID, cur->destination_ID);
                temp->next = cur;
                temp = temp->next;
            }
        }

    }


    // --------------------------------------------------------------------------------------- //


    // Allocates bus stops
    stops *bus_stop = (stops *)calloc(num_bus_stops, sizeof(passenger));
    int i, j;
    for (i = 0; i < num_bus_stops; i++){
        bus_stop[i].stop_ID = i;
        bus_stop[i].num_waiting = 0;
        bus_stop[i].waiting = NULL;
    }


    // --------------------------------------------------------------------------------------- //


    // Allocate bus list
    bus bus_info;
    bus_info.riders = (passenger *) calloc(25, sizeof(passenger));
    bus_info.num_passengers = 0;
    bus_info.current_ID = 0;


    // --------------------------------------------------------------------------------------- //


    // Create all necessary variables
    int time_step = 0;
    int journey_end = FALSE;
    int total_pass;
    int max_pass = 0;
    int wait = 0;
    int wait_stop = 0;
    int bus_location = 0;


    // --------------------------------------------------------------------------------------- //


    // Passenger data
    struct passengers_data *p = head;
    // Simulation loop
    while (journey_end == FALSE){
        time_step++;
        print(log_file, num_bus_stops, time_step, bus_stop, &bus_info);

        /// Put passengers waiting at bus stop
        if (time_step == p->time_of_arrival){
            if (p->next != NULL && p->next->time_of_arrival == time_step){ // If multiple inputs in timestep.
                while (p->next != NULL && p->next->time_of_arrival == time_step){
                    if (bus_stop[p->origin_ID].num_waiting == 1){
                        bus_stop[p->origin_ID].num_waiting++;
                        (bus_stop+1)[p->origin_ID].waiting = p;
                    } else if (bus_stop[p->origin_ID].num_waiting == 0){
                        bus_stop[p->origin_ID].num_waiting++;
                        bus_stop[p->origin_ID].waiting = p;
                        wait_stop++;
                    }
                    if (p->next != NULL){
                        p = p->next;
                    }
                    max_pass++;
                }
                if (bus_stop[p->origin_ID].num_waiting == 1){
                    bus_stop[p->origin_ID].num_waiting++;
                    (bus_stop+1)[p->origin_ID].waiting = p;
                } else if (bus_stop[p->origin_ID].num_waiting == 0){
                    bus_stop[p->origin_ID].num_waiting++;
                    bus_stop[p->origin_ID].waiting = p;
                    wait_stop++;
                }
                if (p->next != NULL){
                    p = p->next;
                }
                max_pass++;
            } else {
                if (bus_stop[p->origin_ID].num_waiting == 1){
                    bus_stop[p->origin_ID].num_waiting++;
                    (bus_stop+1)[p->origin_ID].waiting = p;
                } else if (bus_stop[p->origin_ID].num_waiting == 0){
                    bus_stop[p->origin_ID].num_waiting++;
                    bus_stop[p->origin_ID].waiting = p;
                }
                if (p->next != NULL){
                    p = p->next;
                }
                max_pass++;
            }

        }

        /// Scoop passengers
        if (bus_stop[bus_location].num_waiting == 1){ // If 1 passenger waiting at stop
            // Place passenger at bus stop on bus
            bus_info.riders[total_pass] = *bus_stop[bus_location].waiting;
            bus_info.num_passengers++;
            // Clears bus stop data
            bus_stop[bus_location].num_waiting--;
        } else if (bus_stop[bus_location].num_waiting == 2){ // If 2 passengers waiting at stop
            if (bus_info.num_passengers == 0){ // If no passengers in bus
                // Place passenger at bus stop on bus
                bus_info.riders[0] = *bus_stop[bus_location].waiting;
                bus_info.num_passengers++;
                bus_info.riders[1] = *(bus_stop+1)[bus_location].waiting;
                bus_info.num_passengers++;
            } else if (bus_info.num_passengers > 0){ // If passengers in bus
                // Place passenger at bus stop on bus
                bus_info.riders[total_pass] = *bus_stop[bus_location].waiting;
                bus_info.num_passengers++;
                bus_info.riders[total_pass+1] = *(bus_stop+1)[bus_location].waiting;
                bus_info.num_passengers++;
            } // Clears bus stop data
            bus_stop[bus_location].num_waiting--;
            bus_stop[bus_location].num_waiting--;
        }


        /// Drop passenger
        total_pass = bus_info.num_passengers;
        if (bus_info.num_passengers > 0){ // If passengers in bus
            for (i = 0; i < total_pass; i++){ // Checks each passenger in bus
                if (bus_info.riders[i].destination_ID == bus_location){ // Drops of passenger if at destination
                    bus_info.num_passengers--;
                    total_pass = bus_info.num_passengers;
                    for (j = i; j < total_pass; j++){
                        bus_info.riders[j] = bus_info.riders[j+1];
                    }
                    i = 0;
                }
            }
            // Double-checks the drop loop
            for (i = 0; i < total_pass; i++){ // Checks each passenger in bus
                if (bus_info.riders[i].destination_ID == bus_location){ // Drops of passenger if at destination
                    bus_info.num_passengers--;
                    total_pass = bus_info.num_passengers;
                    //printf("i = %d\n", i);
                    for (j = i; j < total_pass; j++){
                        bus_info.riders[j] = bus_info.riders[j+1];
                    }
                    i = 0;
                }
            }
        }


        /// Determine bus location
        if (num_bus_stops == bus_location+1){
            bus_location = 0;
            bus_info.current_ID = bus_location;
        } else {
            bus_location++;
            bus_info.current_ID = bus_location;
        }


        /// Check if any passengers to scoop or drop
        if (bus_info.num_passengers == 0){
            if (p->next == NULL){
                for (i = 0; i < num_bus_stops; i++){
                    if (bus_stop[i].num_waiting != 0){
                        wait++;
                    }
                }
                if (wait > 0){
                    wait = 0;
                } else {
                    journey_end = TRUE;
                }
            }

        }

    }

    
    float through_put = (float)max_pass/(float)(time_step+1);

    // Print statistics
    fprintf(log_file, "Total sim time: %d\n", time_step+1);
    fprintf(log_file, "Total passengers: %d\n", max_pass);
    fprintf(log_file, "Passenger throughput: %.2f\n", through_put);

    // End of script
    fclose(passenger_file);
    fclose(log_file);
    return 0;
}


int input_num_bus_stops(FILE *passenger_file){
    int num_bus_stops;
    fscanf(passenger_file, "%d", &num_bus_stops);
    return num_bus_stops;
}

void print(FILE *log_file, int num_bus_stops, int time_step, stops *stops, bus *bus_info){
    int i;
    fprintf(log_file, "*** TIME STEP = %d ***\n", time_step);
    fprintf(log_file, "Bus: Bus Stop %d, passengers %d\n", bus_info->current_ID, bus_info->num_passengers);
    // Lists data of all bus stops per time step
    for (i = 0; i < num_bus_stops; i++){
        // Bus Stop X: waiting Y
        fprintf(log_file, "Bus Stop %d:  waiting %d\n", stops[i].stop_ID, stops[i].num_waiting);
    }
}