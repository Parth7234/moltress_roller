#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    double val;
} Rec;

int main() {
    FILE *fp;
    char buf[128];

    fp = fopen("data.txt", "w");
    if (!fp) return 1;
    fprintf(fp, "10 3.14\n20 6.28\n");
    fclose(fp);

    fp = fopen("data.txt", "a");
    if (!fp) return 1;
    fputs("30 9.42\n", fp);
    fclose(fp);

    fp = fopen("data.txt", "r");
    if (!fp) return 1;

    int id;
    double val;
    while (fscanf(fp, "%d %lf", &id, &val) == 2) {
        printf("Parsed: %d %.2f\n", id, val);
    }

    rewind(fp);
    while (fgets(buf, sizeof(buf), fp)) {
        printf("Line: %s", buf);
    }

    fseek(fp, 0, SEEK_END);
    printf("Size: %ld bytes\n", ftell(fp));
    fclose(fp);

    Rec out[2] = {{1, 1.5}, {2, 2.5}};
    fp = fopen("data.bin", "wb");
    if (!fp) return 1;
    fwrite(out, sizeof(Rec), 2, fp);
    fclose(fp);

    Rec in;
    fp = fopen("data.bin", "rb");
    if (!fp) return 1;
    fseek(fp, sizeof(Rec), SEEK_SET);
    fread(&in, sizeof(Rec), 1, fp);
    printf("2nd binary record: %d %.1f\n", in.id, in.val);
    fclose(fp);

    rename("data.txt", "backup.txt");
    remove("backup.txt");
    remove("data.bin");

    return 0;
}