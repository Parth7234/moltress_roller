#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// generates exponentially distributed random variables with rate lambda to model random inter arrival times between network packets
double exp_rand(double lambda) {
    double u = rand() / (RAND_MAX + 1.0);
    return -log(1.0 - u) / lambda;
}

int main() {
    // Input parameters
    int packet_length_bytes = 1000;          // Packet length in bytes
    double bw_src_router = 10000000.0;       // Link 1 Bandwidth: 10 Mbps
    double bw_router_dest = 1000000.0;       // Link 2 Bandwidth (R): 1 Mbps
    double prop_delay_src_router = 0.01;     // Prop delay 1: 10 ms
    double prop_delay_router_dest = 0.01;    // Prop delay 2: 10 ms
    double proc_delay = 0.001;               // Processing delay at router: 1 ms
    int queue_capacity = 50;                 // Max packets in queue
    int num_packets = 100000;                // Number of packets to simulate
    int seed = 12345;                        // Random seed

    srand(seed);

    // Constant calculations
    double L_bits = packet_length_bytes * 8.0;

    double t_tx1 = L_bits / bw_src_router; //transmission delay for sending a packet over link1 (source->router)
    double t_tx2 = L_bits / bw_router_dest; //transmission delay for sending a packet over link2 (router->destination)

    // Traffic intensities (rate of bits generated/rate of bits transmitted)
    double rhos[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.95, 1.0, 1.1, 1.2};
    int num_experiments = sizeof(rhos) / sizeof(rhos[0]);

    // ---------------------------------------------------------
    // 2. OUTPUT FILE SETUP
    // ---------------------------------------------------------
    FILE *csv_file = fopen("QueueSim_Results.csv", "w");
    if (csv_file == NULL) {
        printf("Error creating CSV file!\n");
        return 1;
    }
    
    // Write CSV Headers
    fprintf(csv_file, "Traffic Intensity (rho),Packet Gen Rate (lambda),Packets Generated,Packets Delivered,Packets Dropped,Drop Probability,Avg Queueing Delay,Avg End-to-End Delay,Max Queue Occupancy\n");

    printf("Running Simulations...\n");

    // ---------------------------------------------------------
    // 3. MAIN EXPERIMENT LOOP
    // ---------------------------------------------------------
    for (int exp = 0; exp < num_experiments; exp++) {
        double rho = rhos[exp];
        
        // Calculate lambda based on rho: rho = (lambda * L) / R  => lambda = (rho * R) / L
        double lambda = (rho * bw_router_dest) / L_bits;

        // Reset simulation variables for this run
        double *queue_departures = (double *)malloc(queue_capacity * sizeof(double));
        int head = 0, tail = 0, current_q_size = 0, max_q_occupancy = 0;
        int dropped_packets = 0, delivered_packets = 0;
        
        double total_queueing_delay = 0.0;
        double total_e2e_delay = 0.0;
        double current_source_time = 0.0;
        double last_departure_time = 0.0;

        for (int p = 0; p < num_packets; p++) {
            // Generate inter-arrival time
            double inter_arrival = exp_rand(lambda);
            current_source_time += inter_arrival;

            // Packet arrives completely at the router queue
            double arr_router_time = current_source_time + t_tx1 + prop_delay_src_router;

            // Process departures from the queue that occurred before this packet arrived
            while (current_q_size > 0 && queue_departures[head] <= arr_router_time) {
                head = (head + 1) % queue_capacity;
                current_q_size--;
            }

            // Check queue capacity
            if (current_q_size == queue_capacity) {
                // Queue full, drop packet
                dropped_packets++;
            } else {
                // Accept packet
                delivered_packets++;
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
        }

        // Calculate Averages
        double drop_probability = (double)dropped_packets / num_packets;
        double avg_q_delay = (delivered_packets > 0) ? (total_queueing_delay / delivered_packets) : 0.0;
        double avg_e2e_delay = (delivered_packets > 0) ? (total_e2e_delay / delivered_packets) : 0.0;

        // Write to CSV
        fprintf(csv_file, "%.2f,%.2f,%d,%d,%d,%.6f,%.6f,%.6f,%d\n",
                rho, lambda, num_packets, delivered_packets, dropped_packets, 
                drop_probability, avg_q_delay, avg_e2e_delay, max_q_occupancy);

        free(queue_departures);
    }

    fclose(csv_file);
    printf("Simulation complete. Results saved to 'QueueSim_Results.csv'.\n");

    return 0;
}