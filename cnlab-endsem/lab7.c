#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct entry{
    uint32_t addr;
    int pref;
    int interface;
};


struct entry table[100];
int ct=0;

int parse(const char* str,uint32_t *ip){
    unsigned int a,b,c,d;
    if(sscanf(str,"%u.%u.%u.%u",&a,&b,&c,&d)!=4){
        return 0;
    }
    if(a>255||b>255||c>255||d>255)return 0;

    *ip=(a<<24)|(b<<16)|(c<<8)|d;
    return 1;
}

void format(uint32_t ip,char* str){
    sprintf(str,"%u.%u.%u.%u",(ip>>24)&0xFF,(ip>>16)&0xFF,(ip>>8)&0xFF,ip&0xFF);
}

uint32_t mask(int pref){
    if(pref==0) return 0;
    return (0xFFFFFFFF<<(32-pref))&0xFFFFFFFF;
}

void insert(const char* cidr,int interface){
    if(ct>=100){
        printf("Max capcacity of table reached\n");
        return ;
    }

    char str[20];
    int pref;

    if(sscanf(cidr,"%19[^/]/%d",str,&pref)!=2){
        printf("Invalid CIDR format\n");
        return;
    }

    if(pref<0||pref>32){
        printf("Invalid prefix length\n");
        return;
    }
    
    uint32_t ip;
    if(!parse(str,&ip)){
        printf("Invalid IP Address\n");
        return;
    }

    uint32_t addr = ip&mask(pref);
    for(int i=0;i<ct;i++){
        if(table[i].addr==addr&&table[i].pref==pref){
            table[i].interface=interface;
            printf("Duplicate entry, Updated interface\n");
            return ;
        }
    }

    table[ct].addr=addr;
    table[ct].pref=pref;
    table[ct].interface=interface;
    ct++;
    printf("Entry inserted successfully\n");
}

void display(){
    for(int i=0;i<ct;i++){
        char str[20];
        format(table[i].addr,str);
        printf("%s/%d -> Interface %d\n",str,table[i].pref,table[i].interface);
    }
}

void lookup(const char *dest_str){
    uint32_t dest_ip;
    if(!parse(dest_str, &dest_ip)){
        printf("Invalid destination ip\n");
        return;
    }

    printf("Matching entries:\n");
    int ind=-1;
    int len=-1;
    int found=0;

    for(int i=0;i<ct;i++){
        uint32_t mask_addr=dest_ip&mask(table[i].pref);
        if(mask_addr==table[i].addr){
            found=1;
            char str[20];
            format(table[i].addr,str);
            printf("%s/%d -> Interface %d\n",str,table[i].pref,table[i].interface);

            if(table[i].pref>len){
                len=table[i].pref;
                ind=i;
            }
        }
    }

    if(found&&ind!=-1){
        char str[20];
        format(table[ind].addr,str);
        printf("\nLongest match: %s/%d\n",str,table[ind].pref);
        printf("Interface: %d\n",table[ind].interface);
    }
    else{
        printf("\nNo matching route\n");
    }
}

void delete(const char* cidr){
    char ip_str[20];
    int pref;
    
    if (sscanf(cidr, "%19[^/]/%d", ip_str, &pref) != 2 || pref < 0 || pref > 32) {
        printf("Invalid CIDR format\n");
        return;
    }

    uint32_t ip;
    if(!parse(ip_str,&ip)){
        printf("Invalid IP Address\n");
        return;
    }

    uint32_t addr=ip&mask(pref);

    for(int i=0;i<ct;i++){
        if(table[i].addr==addr&&table[i].pref==pref){
            for(int j=i;j<ct-1;j++){
                table[j]=table[j+1];
            }
            ct--;
            printf("Deleted successfully\n");
            return ;
        }
    }

    printf("Entry not found\n");
}

int main(){
    insert("192.168.0.0/16", 1);
    insert("192.168.10.0/24", 2);
    insert("192.168.10.128/25", 3);
    insert("10.0.0.0/8", 4);
    insert("0.0.0.0/0", 5);

    int choice;
    char input[50];
    int interface;

    while(1){
        printf("Menu\n");
        printf("1. Display forwarding table\n");
        printf("2. Lookup destination\n");
        printf("3. Insert forwarding entry\n");
        printf("4. Delete forwarding entry\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input\n");
            while (getchar() != '\n'); 
            continue;
        }

        switch(choice){
            case 1:
                display();
                break;
            case 2:
                printf("Enter destination IP: ");
                scanf("%49s", input);
                lookup(input);
                break;
            case 3:
                printf("Enter CIDR (e.g., 192.168.10.0/24): ");
                scanf("%49s", input);
                printf("Enter interface number: ");
                scanf("%d", &interface);
                insert(input, interface);
                break;
            case 4:
                printf("Enter CIDR to delete: ");
                scanf("%49s", input);
                delete(input);
                break;
            case 5:
                printf("Bye !\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}