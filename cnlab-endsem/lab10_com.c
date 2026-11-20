#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main() {
    int N = 4, T = 1000;
    double p = 0.25;
    int successful_slots = 0, collision_slots = 0, idle_slots = 0;
    int choice, has_run = 0;

    // Seed the random number generator
    srand(time(NULL));

    while (1) {
        printf("--- Menu ---\n");
        printf("1. Configure Network\n");
        printf("2. Run Slotted ALOHA\n");
        printf("3. Display Simulation Statistics\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        printf("\n");

        if (choice == 1) {
            printf("Enter number of nodes (N): ");
            scanf("%d", &N);
            printf("Enter transmission probability (p): ");
            scanf("%lf", &p);
            printf("Enter total number of slots (T): ");
            scanf("%d", &T);
            has_run = 0;
            printf("Network configured successfully.\n\n");
        } 
        else if (choice == 2) {
            successful_slots = 0;
            collision_slots = 0;
            idle_slots = 0;

            for (int t = 0; t < T; t++) {
                int transmitting_nodes = 0;
                for (int i = 0; i < N; i++) {
                    double rand_val = (double)rand() / RAND_MAX;
                    if (rand_val < p) {
                        transmitting_nodes++;
                    }
                }

                if (transmitting_nodes == 1) {
                    successful_slots++;
                } else if (transmitting_nodes > 1) {
                    collision_slots++;
                } else {
                    idle_slots++;
                }
            }
            has_run = 1;
            printf("Slotted ALOHA simulation completed.\n\n");
            
            double observed_eff = ((double)successful_slots / T) * 100.0;
            double theoretical_eff = (N * p * pow(1.0 - p, N - 1)) * 100.0;

            printf("Nodes: %d\n", N);
            printf("Transmission probability: %.2f\n", p);
            printf("Total slots: %d\n", T);
            printf("Successful slots: %d\n", successful_slots);
            printf("Collision slots: %d\n", collision_slots);
            printf("Idle slots: %d\n", idle_slots);
            printf("Observed efficiency: %.2f%%\n", observed_eff);
            printf("Theoretical efficiency: %.2f%%\n\n", theoretical_eff);
        } 
        else if (choice == 3) {
            if (!has_run) {
                printf("Please run the simulation first.\n\n");
            } else {
                double observed_eff = ((double)successful_slots / T) * 100.0;
                double theoretical_eff = (N * p * pow(1.0 - p, N - 1)) * 100.0;

                printf("Nodes: %d\n", N);
                printf("Transmission probability: %.2f\n", p);
                printf("Total slots: %d\n", T);
                printf("Successful slots: %d\n", successful_slots);
                printf("Collision slots: %d\n", collision_slots);
                printf("Idle slots: %d\n", idle_slots);
                printf("Observed efficiency: %.2f%%\n", observed_eff);
                printf("Theoretical efficiency: %.2f%%\n\n", theoretical_eff);
            }
        } 
        else if (choice == 4) {
            printf("Exiting simulator.\n");
            break;
        } 
        else {
            printf("Invalid choice. Please try again.\n\n");
        }
    }

    return 0;
}