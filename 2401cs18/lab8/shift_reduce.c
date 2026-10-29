
// Name: Parth Singla
// Roll Number: 2401CS18

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char st[1000];
int tp = 0;
char in_str[1000];
int pos = 0;

char peek() {
    while (in_str[pos] == ' ' || in_str[pos] == '\t') {
        pos++;
    }
    if (in_str[pos] == 'i' && in_str[pos+1] == 'd') {
        return 'i';
    }
    return in_str[pos];
}

void shift() {
    char look = peek();
    if (look == 'i') {
        printf("SHIFT: id\n");
        tp++;
        st[tp] = 'i';
        pos += 2;
    } else {
        printf("SHIFT: %c ", look);
        tp++;
        st[tp] = look;
        pos++;
    }
}

int get_prio(char op) {
    if (op == '*') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

int reduce() {
    char look = peek();

    if (tp >= 1 && st[tp] == 'i') {
        printf("REDUCE: id -> E\n");
        st[tp] = 'E';
        return 1;
    }

    if (tp >= 3 && st[tp-2] == '(' && st[tp-1] == 'E' && st[tp] == ')') {
        printf("REDUCE: (E) -> E\n");
        tp -= 2;
        st[tp] = 'E';
        return 1;
    }

    if (tp >= 3 && st[tp-2] == 'E' && 
        (st[tp-1] == '*' || st[tp-1] == '+' || st[tp-1] == '-') && 
        st[tp] == 'E') {
        
        char op = st[tp-1];
        
        if (look == '*' || look == '+' || look == '-') {
            int p_op = get_prio(op);
            int p_lk = get_prio(look);

            if (p_op >= p_lk) {
                printf("REDUCE: E%cE -> E\n", op);
                tp -= 2;
                st[tp] = 'E';
                return 1;
            } else {
                return 0;
            }
        } else {
            printf("REDUCE: E%cE -> E\n", op);
            tp -= 2;
            st[tp] = 'E';
            return 1;
        }
    }

    return 0;
}

int main() {
    if (fgets(in_str, sizeof(in_str), stdin) == NULL) {
        return 0;
    }
    
    size_t len = strlen(in_str);
    if (len > 0 && in_str[len-1] == '\n') {
        in_str[len-1] = '\0';
        len--;
    }

    int has_eof = 0;
    for (size_t i = 0; i < len; i++) {
        if (in_str[i] == '$') {
            has_eof = 1;
            break;
        }
    }
    if (!has_eof) {
        strcat(in_str, " $");
    }

    st[0] = '$';
    tp = 0;

    while (1) {
        int did_red = reduce();
        if (did_red) {
            continue; 
        }

        char look = peek();
        if (look == '$') {
            break; 
        } else {
            shift(); 
        }
    }

    if (tp == 1 && st[0] == '$' && st[1] == 'E') {
        printf("ACCEPT\n");
    } else {
        printf("REJECT\n");
    }

    return 0;
}