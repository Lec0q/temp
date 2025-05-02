#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STAT   50

typedef struct node {
    char trip_id[15];
    char train_id[15];
    char start_station[MAX_STAT];
    char end_station[MAX_STAT];
    int  num_of_passengers;
    int  start_time;     // HHMM, ví dụ 0930
    int  arrival_time;   // HHMM
    char start_day[11];  // "DD/MM/YYYY"
    char arrival_day[11]; // "DD/MM/YYYY"
    struct node *next, *prev;
} NODE;

typedef struct {
    NODE *head, *tail;
} DLIST;

// Khởi tạo danh sách
void create_list(DLIST *list) {
    list->head = list->tail = NULL;
}

// Thêm node ở cuối
void add_node_tail(DLIST *list, NODE *p) {
    p->next = NULL;
    if (list->tail == NULL) {
        p->prev = NULL;
        list->head = list->tail = p;
    } else {
        p->prev = list->tail;
        list->tail->next = p;
        list->tail = p;
    }
}


void add_new_data_manually(DLIST *list) {
    NODE *p = (NODE *)malloc(sizeof *p);
    if (!p) {
        perror("malloc");
        return;
    }
    printf("Enter trip ID:      "); scanf("%14s", p->trip_id);
    printf("Enter train ID:     "); scanf("%14s", p->train_id);
    printf("Enter start station:"); scanf("%49s", p->start_station);
    printf("Enter end station:  "); scanf("%49s", p->end_station);
    printf("Enter number of passengers: "); scanf("%d", &p->num_of_passengers);
    printf("Enter start time (HHMM):     "); scanf("%d", &p->start_time);
    printf("Enter arrival time (HHMM):   "); scanf("%d", &p->arrival_time);
    printf("Enter start day (DD/MM/YYYY):   "); scanf("%10s", p->start_day);
    printf("Enter arrival day (DD/MM/YYYY): "); scanf("%10s", p->arrival_day);
    add_node_tail(list, p);
}


void add_new_data_from_file(DLIST *list, FILE *f) {
    NODE *p = (NODE *)malloc(sizeof *p);
    if (!p) {
        perror("malloc");
        return;
    }
    int ret = fscanf(f,
        "%14s %14s %49s %49s %d %d %d %10s %10s",
        p->trip_id,
        p->train_id,
        p->start_station,
        p->end_station,
        &p->num_of_passengers,
        &p->start_time,
        &p->arrival_time,
        p->start_day,
        p->arrival_day
    );
    if (ret != 9) {
        free(p);
        return;
    }
    add_node_tail(list, p);
}

void enter_data(DLIST *list) {
    int choice;
    printf("Enter 1 to add data manually\n");
    printf("Enter 2 to add data from file\n");
    printf("Your choice: ");
    scanf("%d", &choice);
    getchar(); 
    if (choice == 1) {
        add_new_data_manually(list);
    } else if (choice == 2) {
        char filename[128];
        printf("Enter filename: ");
        fgets(filename, sizeof filename, stdin);
        filename[strcspn(filename, "\n")] = '\0';
        FILE *f = fopen(filename, "r");
        if (!f) {
            perror("fopen");
            return;
        }
        while (1) {
            long pos = ftell(f);
            NODE *peek = (NODE *)malloc(sizeof *peek);
            if (!peek) { perror("malloc"); break; }
            int r = fscanf(f,
                "%14s %14s %49s %49s %d %d %d %10s %10s",
                peek->trip_id,
                peek->train_id,
                peek->start_station,
                peek->end_station,
                &peek->num_of_passengers,
                &peek->start_time,
                &peek->arrival_time,
                peek->start_day,
                peek->arrival_day
            );
            if (r != 9) {
                free(peek);
                break;
            }
            fseek(f, pos, SEEK_SET);
            add_new_data_from_file(list, f);
        }
        fclose(f);
    } else {
        printf("Invalid choice!\n");
    }
}

int get_day(const char *date_str) {
    int d, m, y;
    if (sscanf(date_str, "%2d/%2d/%4d", &d, &m, &y) == 3)
        return d;
    return -1;
}

int get_month(const char *date_str) {
    int d, m, y;
    if (sscanf(date_str, "%2d/%2d/%4d", &d, &m, &y) == 3)
        return m;
    return -1;
}

int get_year(const char *date_str) {
    int d, m, y;
    if (sscanf(date_str, "%2d/%2d/%4d", &d, &m, &y) == 3)
        return y;
    return -1;
}


int compare_start(const NODE *a, const NODE *b) {
    int da = get_day(a->start_day), db = get_day(b->start_day);
    int ma = get_month(a->start_day), mb = get_month(b->start_day);
    int ya = get_year(a->start_day), yb = get_year(b->start_day);
    if (ya != yb) return ya - yb;
    if (ma != mb) return ma - mb;
    if (da != db) return da - db;
    return a->start_time - b->start_time;
}

void swap_data(NODE *a, NODE *b) {
    NODE tmp = *a;
    struct node *a_next = a->next, *a_prev = a->prev;
    struct node *b_next = b->next, *b_prev = b->prev;

    *a = *b;
    a->next = a_next; a->prev = a_prev;

    *b = tmp;
    b->next = b_next; b->prev = b_prev;
}

void sorting_list_by_time_date(DLIST *list) {
    if (!list->head) return;
    for (NODE *i = list->head; i->next; i = i->next) {
        for (NODE *j = i->next; j; j = j->next) {
            if (compare_start(i, j) > 0) {
                swap_data(i, j);
            }
        }
    }
}

void display_data(const DLIST *list) {
    printf("\n%-15s %-15s %-12s %-12s %-5s %-10s %-5s %-10s %-5s\n",
        "Trip ID","Train ID","From","To","Pax",
        "StartDay","ST","ArrDay","AT");
    for (NODE *cur = list->head; cur; cur = cur->next) {
        printf("%-15s %-15s %-12s %-12s %-5d %-10s %-4d %-10s %-5d\n",
            cur->trip_id,
            cur->train_id,
            cur->start_station,
            cur->end_station,
            cur->num_of_passengers,
            cur->start_day,
            cur->start_time,
            cur->arrival_day,
            cur->arrival_time
        );
    }
}





void free_list(DLIST *list) {
    NODE *cur = list->head, *next;
    while (cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    list->head = list->tail = NULL;
}

int main(void) {
    DLIST list;
    create_list(&list);

    int opt;
    do {
        printf("\n=== TRAIN SCHEDULE MANAGEMENT ===\n");
        printf("1. Enter data\n");
        printf("2. Display all\n");
        printf("3. Sorting_LIFO\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &opt);
        getchar(); // bỏ newline

        switch (opt) {
            case 1:
                enter_data(&list);
                break;
            case 2:
                display_data(&list);
                break;
            case 3:
                sorting_list_by_time_date(&list);
                break;
            case 0:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid option!\n");
        }
    } while (opt != 0);

    free_list(&list);
    return 0;
}
