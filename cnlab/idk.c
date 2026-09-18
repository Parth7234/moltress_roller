#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Generates exponentially distributed random variables with rate lambda
double exp_rand(double lambda) {
    double u = rand() / (RAND_MAX + 1.0);
    return -log(1.0 - u) / lambda;
}

// Function to run a single simulation scenario
void run_simulation(int num_sources, double lambdas[], int num_packets, FILE *csv_file, const char* scenario_name) {
    // System Parameters
    int packet_length_bytes = 1000;          
    double bw_src_router = 10000000.0;       // Link 1 Bandwidth: 10 Mbps
    double bw_router_dest = 1000000.0;       // Link 2 Bandwidth (R): 1 Mbps
    double prop_delay_src_router = 0.01;     // Prop delay 1: 10 ms
    double prop_delay_router_dest = 0.01;    // Prop delay 2: 10 ms
    double proc_delay = 0.001;               // Processing delay at router: 1 ms
    int queue_capacity = 50;                 // Max packets in queue

    double L_bits = packet_length_bytes * 8.0;
    double t_tx1 = L_bits / bw_src_router; 
    double t_tx2 = L_bits / bw_router_dest; 

    // Aggregate statistics
    double aggregate_lambda = 0.0;
    for (int i = 0; i < num_sources; i++) {
        aggregate_lambda += lambdas[i];
    }
    
    // rho = (lambda * L) / R
    double rho = (aggregate_lambda * L_bits) / bw_router_dest;

    // Simulation Variables
    double *queue_departures = (double *)malloc(queue_capacity * sizeof(double));
    double *next_arrival = (double *)malloc(num_sources * sizeof(double));
    
    int head = 0, tail = 0, current_q_size = 0, max_q_occupancy = 0;
    int dropped_packets = 0, delivered_packets = 0;
    
    double total_queueing_delay = 0.0;
    double total_e2e_delay = 0.0;
    double last_departure_time = 0.0;

    // Initialize first arrival for each independent source
    for (int i = 0; i < num_sources; i++) {
        next_arrival[i] = exp_rand(lambdas[i]);
    }

    for (int p = 0; p < num_packets; p++) {
        // Find the source with the earliest next arrival time
        int min_idx = 0;
        for (int i = 1; i < num_sources; i++) {
            if (next_arrival[i] < next_arrival[min_idx]) {
                min_idx = i;
            }
        }
        
        double current_source_time = next_arrival[min_idx];

        // Packet arrives completely at the router queue
        double arr_router_time = current_source_time + t_tx1 + prop_delay_src_router;

        // Process departures from the queue that occurred before this packet arrived
        while (current_q_size > 0 && queue_departures[head] <= arr_router_time) {
            head = (head + 1) % queue_capacity;
            current_q_size--;
        }

        // Check queue capacity
        if (current_q_size == queue_capacity) {
            dropped_packets++; // Queue full, drop packet
        } else {
            delivered_packets++; // Accept packet
            if (current_q_size + 1 > max_q_occupancy) {
                max_q_occupancy = current_q_size + 1;
            }

            // Calculate when processing starts for this packet
            double start_service_time;
            if (current_q_size == 0) {
                start_service_time = arr_router_time; // Server is idle
            } else {
                start_service_time = last_departure_time; // Wait for previous packet
            }

            // Compute Delays
            double q_delay = start_service_time - arr_router_time;
            double e2e_delay = t_tx1 + prop_delay_src_router + q_delay + proc_delay + t_tx2 + prop_delay_router_dest;
            
            total_queueing_delay += q_delay;
            total_e2e_delay += e2e_delay;

            // Calculate departure time and add to queue
            double departure_time = start_service_time + proc_delay + t_tx2;
            queue_departures[tail] = departure_time;
            tail = (tail + 1) % queue_capacity;
            current_q_size++;
            last_departure_time = departure_time;
        }

        // Generate the NEXT arrival time for the source we just processed
        next_arrival[min_idx] += exp_rand(lambdas[min_idx]);
    }

    // Calculate Averages
    double drop_probability = (double)dropped_packets / num_packets;
    double avg_q_delay = (delivered_packets > 0) ? (total_queueing_delay / delivered_packets) : 0.0;
    double avg_e2e_delay = (delivered_packets > 0) ? (total_e2e_delay / delivered_packets) : 0.0;

    // Write to CSV: Scenario, rho, aggregate_lambda, generated, delivered, dropped, drop_prob, avg_q, avg_e2e, max_q
    fprintf(csv_file, "%s,%.2f,%.2f,%d,%d,%d,%.6f,%.6f,%.6f,%d\n",
            scenario_name, rho, aggregate_lambda, num_packets, delivered_packets, dropped_packets, 
            drop_probability, avg_q_delay, avg_e2e_delay, max_q_occupancy);

    free(queue_departures);
    free(next_arrival);
}

int main() {
    int seed = 12345;
    srand(seed);
    int num_packets = 100000;
    int num_sources = 4;

    FILE *csv_file = fopen("QueueSim_Results.csv", "w");
    if (csv_file == NULL) {
        printf("Error creating CSV file!\n");
        return 1;
    }
    
    // Write CSV Headers exactly as requested in output requirements
    fprintf(csv_file, "Scenario,Traffic Intensity (rho),Aggregate Packet Gen Rate,Packets Generated,Packets Delivered,Packets Dropped,Packet-Drop Probability,Avg Queueing Delay,Avg End-to-End Delay,Max Queue Occupancy\n");

    printf("Running Part 1: Equal Packet Generation Rates...\n");
    // Part 1: Vary common rate so rho = 0.2, 0.4, 0.6, 0.8, 0.9, 1.0, 1.2
    double rhos[] = {0.2, 0.4, 0.6, 0.8, 0.9, 1.0, 1.2};
    int num_rhos = sizeof(rhos) / sizeof(rhos[0]);
    double L_bits = 8000.0; // 1000 bytes
    double R = 1000000.0;   // 1 Mbps

    for (int i = 0; i < num_rhos; i++) {
        // Calculate the aggregate lambda needed to achieve this rho
        double agg_lambda = (rhos[i] * R) / L_bits;
        // Split equally among 4 sources
        double lambdas[4] = {agg_lambda / 4, agg_lambda / 4, agg_lambda / 4, agg_lambda / 4};
        
        char scenario_name[32];
        sprintf(scenario_name, "Part 1 (rho=%.1f)", rhos[i]);
        run_simulation(num_sources, lambdas, num_packets, csv_file, scenario_name);
    }

    printf("Running Part 2: Specific Packet Generation Rates...\n");
    // Part 2: Specific varying distributions. 
    // Notice the sum for all these arrays is 1000, keeping aggregate generation rate the same.
    double part2_rates[4][4] = {
        {100, 100, 100, 700},
        {150, 150, 250, 450},
        {250, 250, 250, 250},
        {400, 300, 200, 100}
    };

    for (int i = 0; i < 4; i++) {
        char scenario_name[32];
        sprintf(scenario_name, "Part 2 (Test %d)", i + 1);
        run_simulation(num_sources, part2_rates[i], num_packets, csv_file, scenario_name);
    }

    fclose(csv_file);
    printf("Simulations complete. Results saved to 'QueueSim_Results.csv'.\n");

    return 0;
}