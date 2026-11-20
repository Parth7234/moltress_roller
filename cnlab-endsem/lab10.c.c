#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main() {
    int n = 4, t = 1000;
    double p = 0.25;
    int succ = 0, coll = 0, idle = 0;
    int ch, flag = 0;

    srand(time(NULL));

    while (1) {
        printf(" Menu \n");
        printf("1. Configure Network\n");
        printf("2. Run Slotted ALOHA\n");
        printf("3. Display Simulation Statistics\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &ch);
        printf("\n");

        if (ch == 1) {
            printf("Enter number of nodes (N): ");
            scanf("%d", &n);
            printf("Enter transmission probability (p): ");
            scanf("%lf", &p);
            printf("Enter total number of slots (T): ");
            scanf("%d", &t);
            flag = 0;
            printf("Network configured successfully.\n\n");
        } 
        else if (ch == 2) {
            succ = 0;
            coll = 0;
            idle = 0;

            for (int i = 0; i < t; i++) {
                int cnt = 0;
                for (int j = 0; j < n; j++) {
                    double r = (double)rand() / RAND_MAX;
                    if (r < p) {
                        cnt++;
                    }
                }

                if (cnt == 1) {
                    succ++;
                } else if (cnt > 1) {
                    coll++;
                } else {
                    idle++;
                }
            }
            flag = 1;
            printf("Slotted ALOHA simulation completed.\n\n");
            
            double obs = ((double)succ / t) * 100.0;
            double th = (n * p * pow(1.0 - p, n - 1)) * 100.0;

            printf("Nodes: %d\n", n);
            printf("Transmission probability: %.2f\n", p);
            printf("Total slots: %d\n", t);
            printf("Successful slots: %d\n", succ);
            printf("Collision slots: %d\n", coll);
            printf("Idle slots: %d\n", idle);
            printf("Observed efficiency: %.2f%%\n", obs);
            printf("Theoretical efficiency: %.2f%%\n\n", th);
        } 
        else if (ch == 3) {
            if (!flag) {
                printf("Please run the simulation first.\n\n");
            } else {
                double obs = ((double)succ / t) * 100.0;
                double th = (n * p * pow(1.0 - p, n - 1)) * 100.0;

                printf("Nodes: %d\n", n);
                printf("Transmission probability: %.2f\n", p);
                printf("Total slots: %d\n", t);
                printf("Successful slots: %d\n", succ);
                printf("Collision slots: %d\n", coll);
                printf("Idle slots: %d\n", idle);
                printf("Observed efficiency: %.2f%%\n", obs);
                printf("Theoretical efficiency: %.2f%%\n\n", th);
            }
        } 
        else if (ch == 4) {
            printf("Exiting simulator.\n");
            break;
        } 
        else {
            printf("Invalid choice. Please try again.\n\n");
        }
    }

    return 0;
}