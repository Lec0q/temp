#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> 
#include <time.h>   

#define MAX_STAT   50

typedef struct node {
    char trip_id[15];
    char train_id[15];
    char start_station[MAX_STAT];
    char end_station[MAX_STAT];
    int  num_of_passengers;
    int  start_time;     // HHMM
    int  arrival_time;   // HHMM
    char start_day[11];   // "DD/MM/YYYY"
    char arrival_day[11]; // "DD/MM/YYYY"
    struct node *next, *prev;
} NODE;

typedef struct {
    NODE *head, *tail;
} DLIST;

void create_list(DLIST *list) {
    list->head = list->tail = NULL;
}

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

int is_valid_date(const char *date) {
    int d, m, y;
    if (sscanf(date, "%2d/%2d/%4d", &d, &m, &y) != 3) return 0;
    if (y < 1900 || y > 2100) return 0;
    if (m < 1 || m > 12) return 0;
    if (d < 1 || d > 31) return 0;
    return 1;
}

int is_valid_time(int time) {
    int hh = time / 100;
    int mm = time % 100;
    return (hh >= 0 && hh <= 23 && mm >= 0 && mm <= 59);
}

void to_upper_str(char *s) {
    for (; *s; ++s) *s = toupper((unsigned char)*s);
}

void add_new_data_manually(DLIST *list) {
    NODE *p = malloc(sizeof *p);
    if (!p) { perror("malloc"); return; }

    do {
        printf("Enter trip ID (e.g. TR1234): ");
        scanf("%14s", p->trip_id);
        to_upper_str(p->trip_id);
        if (strlen(p->trip_id) < 3)
            printf("Trip ID must be at least 3 characters long!\n");
    } while (strlen(p->trip_id) < 3);

    do {
        printf("Enter train ID (e.g. TN456): ");
        scanf("%14s", p->train_id);
        to_upper_str(p->train_id);
        if (strlen(p->train_id) < 3)
            printf("Train ID must be at least 3 characters long!\n");
    } while (strlen(p->train_id) < 3);

    do {
        printf("Enter start station: ");
        scanf("%49s", p->start_station);
        if (strlen(p->start_station) == 0)
            printf("Start station cannot be empty!\n");
    } while (strlen(p->start_station) == 0);

    do {
        printf("Enter end station: ");
        scanf("%49s", p->end_station);
        if (strlen(p->end_station) == 0)
            printf("End station cannot be empty!\n");
    } while (strlen(p->end_station) == 0);

    do {
        printf("Enter number of passengers (1-1000): ");
        scanf("%d", &p->num_of_passengers);
        if (p->num_of_passengers < 1 || p->num_of_passengers > 1000)
            printf("Invalid number! Please enter 1 - 1000.\n");
    } while (p->num_of_passengers < 1 || p->num_of_passengers > 1000);

    do {
        printf("Enter start time (HHMM): ");
        scanf("%d", &p->start_time);
        if (!is_valid_time(p->start_time))
            printf("Invalid time format! Please enter HHMM.\n");
    } while (!is_valid_time(p->start_time));

    do {
        printf("Enter arrival time (HHMM): ");
        scanf("%d", &p->arrival_time);
        if (!is_valid_time(p->arrival_time))
            printf("Invalid time format! Please enter HHMM.\n");
    } while (!is_valid_time(p->arrival_time));

    do {
        printf("Enter start day (DD/MM/YYYY): ");
        scanf("%10s", p->start_day);
        if (!is_valid_date(p->start_day))
            printf("Invalid date format! Use DD/MM/YYYY.\n");
    } while (!is_valid_date(p->start_day));

    do {
        printf("Enter arrival day (DD/MM/YYYY): ");
        scanf("%10s", p->arrival_day);
        if (!is_valid_date(p->arrival_day))
            printf("Invalid date format! Use DD/MM/YYYY.\n");
    } while (!is_valid_date(p->arrival_day));

    add_node_tail(list, p);
}

void add_new_data_from_file(DLIST *list, FILE *f) {
    NODE *p = malloc(sizeof *p);
    if (!p) {
        perror("malloc");
        return;
    }
    if (fscanf(f,
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
    ) != 9) {
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
            NODE *peek = malloc(sizeof *peek);
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
            free(peek);
            if (r != 9) break;
            fseek(f, pos, SEEK_SET);
            add_new_data_from_file(list, f);
        }
        fclose(f);
    } else {
        printf("Invalid choice!\n");
    }
}

int get_day(const char *s) {
    int d, m, y;
    if (sscanf(s, "%2d/%2d/%4d", &d, &m, &y) == 3) return d;
    return -1;
}

int get_month(const char *s) {
    int d, m, y;
    if (sscanf(s, "%2d/%2d/%4d", &d, &m, &y) == 3) return m;
    return -1;
}

int get_year(const char *s) {
    int d, m, y;
    if (sscanf(s, "%2d/%2d/%4d", &d, &m, &y) == 3) return y;
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
    *a = *b; a->next = a_next; a->prev = a_prev;
    *b = tmp; b->next = b_next; b->prev = b_prev;
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

void add_lifo(DLIST *list, NODE *p) {
    p->prev = NULL;
    p->next = list->head;
    if (list->head) list->head->prev = p;
    else            list->tail = p;
    list->head = p;
}

void delete_lifo_by_id(DLIST *list, const char *id) {
    if (!list->head) {
        printf("List empty\n");
        return;
    }
    NODE *h = list->head;
    if (strcmp(h->trip_id, id) == 0 || strcmp(h->train_id, id) == 0) {
        list->head = h->next;
        if (list->head) list->head->prev = NULL;
        else            list->tail = NULL;
        printf("Deleted (LIFO) trip %s / train %s\n", h->trip_id, h->train_id);
        free(h);
    } else {
        printf("Cannot delete %s: it's not the last inserted (LIFO)\n", id);
    }
}

int search_by_id_total(const DLIST *list, const char *id) {
    int count = 0;
    for (NODE *cur = list->head; cur; cur = cur->next) {
        if (strcmp(cur->trip_id, id) == 0 || strcmp(cur->train_id, id) == 0) {
            count++;
        }
    }
    return count;
}

void printBusiestStation(DLIST *list) {
    char stations[100][MAX_STAT];
    int counts[100] = {0};
    int stationCount = 0;

    for (NODE *p = list->head; p; p = p->next) {
        const char *names[2] = { p->start_station, p->end_station };
        for (int k = 0; k < 2; ++k) {
            int i;
            for (i = 0; i < stationCount; ++i) {
                if (strcmp(stations[i], names[k]) == 0) {
                    counts[i]++;
                    break;
                }
            }
            if (i == stationCount && stationCount < 100) {
                strncpy(stations[stationCount], names[k], MAX_STAT);
                stations[stationCount][MAX_STAT-1] = '\0';
                counts[stationCount] = 1;
                stationCount++;
            }
        }
    }

    if (stationCount == 0) {
        printf("No stations in the list.\n");
        return;
    }
    int best = 0;
    for (int i = 1; i < stationCount; ++i) {
        if (counts[i] > counts[best]) {
            best = i;
        }
    }

    printf("Busiest station: %s (%d trips)\n",
           stations[best], counts[best]);
}
static int cmpYMD(int y1, int m1, int d1, int y2, int m2, int d2) {
    if (y1 != y2) return y1 - y2;
    if (m1 != m2) return m1 - m2;
    return d1 - d2;
}
int totalPassengersInRange(DLIST *list,
    int d1, int m1, int y1,
    int d2, int m2, int y2)
{
    int sum = 0;
    for (NODE *p = list->head; p; p = p->next) {
        int sd = get_day(p->start_day);
        int sm = get_month(p->start_day);
        int sy = get_year(p->start_day);
        if (cmpYMD(sy, sm, sd, y1, m1, d1) >= 0 &&
            cmpYMD(sy, sm, sd, y2, m2, d2) <= 0)
        {
            sum += p->num_of_passengers;
        }
    }
    return sum;
}

void printLongestTrip(DLIST *list) {
    if (!list->head) { printf("List empty\n"); return; }
    NODE *best = NULL;
    double bestMin = -1;
    for (NODE *p = list->head; p; p = p->next) {
        struct tm t1={0}, t2={0};
        t1.tm_mday = get_day(p->start_day);
        t1.tm_mon  = get_month(p->start_day)-1;
        t1.tm_year = get_year(p->start_day)-1900;
        t1.tm_hour = p->start_time/100;
        t1.tm_min  = p->start_time%100;
        t2.tm_mday = get_day(p->arrival_day);
        t2.tm_mon  = get_month(p->arrival_day)-1;
        t2.tm_year = get_year(p->arrival_day)-1900;
        t2.tm_hour = p->arrival_time/100;
        t2.tm_min  = p->arrival_time%100;
        double diff = difftime(mktime(&t2), mktime(&t1)) / 60.0;
        if (diff > bestMin) {
            bestMin = diff;
            best = p;
        }
    }
    if (best) {
        printf("Longest trip: %s (train %s), duration %.0f minutes\n",
            best->trip_id, best->train_id, bestMin);
    }
}

void printMaxTripsInOneDay(DLIST *list) {
    struct { int d,m,y,cnt; } A[500];
    int n = 0;
    for (NODE *p = list->head; p; p = p->next) {
        int d = get_day(p->start_day);
        int m = get_month(p->start_day);
        int y = get_year(p->start_day);
        int i;
        for (i = 0; i < n; i++) {
            if (A[i].d==d && A[i].m==m && A[i].y==y) {
                A[i].cnt++;
                break;
            }
        }
        if (i==n && n<500) {
            A[n].d=d; A[n].m=m; A[n].y=y; A[n].cnt=1;
            n++;
        }
    }
    if (!n) { printf("List empty\n"); return; }
    int best=0;
    for (int i = 1; i < n; i++)
        if (A[i].cnt > A[best].cnt) best = i;
    printf("Max trips on one day: %02d/%02d/%04d = %d trips\n",
        A[best].d, A[best].m, A[best].y, A[best].cnt);
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

int main() {
    DLIST list;
    create_list(&list);
    int opt;
    int lang = 0;
    printf("Choose language: 1. English 2. Vietnamese\n");
    printf("Your choice: ");
    scanf("%d", &lang);
    switch (lang) {
    case 1:
    do {
        printf("\n=== TRAIN SCHEDULE MANAGEMENT ===\n");
        printf("1. Enter data (manual/auto)\n");
        printf("2. Display all\n");
        printf("3. Add/Delete data (LIFO)\n");
        printf("4. Sort data by time/date\n");
        printf("5. Add data after sorted\n");
        printf("6. Total trip by ID (TRIP/TRAIN)\n");
        printf("7. Find the busiest station\n");
        printf("8. Total passengers in range\n");
        printf("9. The longest trip\n");
        printf("10. The max trips in one day\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &opt);
        getchar();
        switch (opt) {
            case 1:
                enter_data(&list);
                break;
            case 2:
                display_data(&list);
                break;
            case 3: {
                int C1;
                printf("Enter 1 to add data (LIFO)\n");
                printf("Enter 2 to delete data (LIFO)\n");
                printf("Your choice: ");
                scanf("%d", &C1);
                getchar();
                if (C1 == 1) {
                    add_new_data_manually(&list);
                    add_lifo(&list, list.tail);  
                } else if (C1 == 2) {
                    char id[15];
                    printf("Enter trip/train ID to delete: ");
                    scanf("%14s", id);
                    to_upper_str(id);
                    delete_lifo_by_id(&list, id);
                } else {
                    printf("Invalid choice!\n");
                }
                sleep(2);
                break; // Added break to prevent fall-through
            }
            case 4: {
                printf("Sorting data by time/date");
                fflush(stdout);
                for (int i = 0; i < 5; i++) {
                    printf(".");

                    sleep(1);
                }
                printf(" done.\n");
                sorting_list_by_time_date(&list);
                display_data(&list);
                break;

            }
            case 5: {
                printf("Adding data after sorted\n");
                add_new_data_manually(&list);
                printf("Applying changes");
                fflush(stdout);
                for (int i = 0; i < 5; i++) {
                    printf(".");
                    sleep(1);
                }
                printf("\n");
                sorting_list_by_time_date(&list);
                display_data(&list);
                break;
            }
            case 6: {
                char id[15];
                printf("Enter trip/train ID to search: ");
                scanf("%14s", id);
                to_upper_str(id);
                printf("Searching for ID %s", id);
                fflush(stdout);
                for (int i = 0; i < 5; i++) {
                    printf(".");
                    sleep(0.5);
                }
                int count = search_by_id_total(&list, id);
                printf("\nFound %d entries for ID %s\n", count, id);
                sleep(2);
                break; // Ensure no fall-through
            }
            case 7:{
                printf("Finding the busiest station ");
                for (int i = 0; i < 5; i++)
                {
                    printf(".");
                    sleep(1);
                }
                printf("\n");
                printBusiestStation(&list);
                break;
            }
            case 8: {
                int d1, m1, y1, d2, m2, y2;
                printf("Enter start date (DD/MM/YYYY): ");
                scanf("%2d/%2d/%4d", &d1, &m1, &y1);
                printf("Enter end date (DD/MM/YYYY): ");
                scanf("%2d/%2d/%4d", &d2, &m2, &y2);
                printf("Searching for passengers from %02d/%02d/%04d to %02d/%02d/%04d ",
                    d1, m1, y1, d2, m2, y2);
                for (int i = 0; i < 4; i++)
                {
                    printf(".");
                    sleep(1);
                }
                printf("\n");
                int total = totalPassengersInRange(&list, d1, m1, y1, d2, m2, y2);
                printf("Total passengers from %02d/%02d/%04d to %02d/%02d/%04d: %d\n",
                    d1, m1, y1, d2, m2, y2, total);
                break;
            }
            case 9: {
                printf("Finding the longest trip:");
                for (int i = 0; i < 5; i++)
                {
                    printf(".");
                    sleep(1);
                }
                printf("\nThe longest trip is:");
                printLongestTrip(&list);
                sleep(2);
                break;

            }
            case 10: {
                printf("Finding the max trips in one day: ");
                for (int i = 0; i < 5; i++)
                {
                    printf(".");
                    sleep(1);
                }
                printf("\nThe max trips in one day is:");
                printMaxTripsInOneDay(&list);
                break;
            }
            case 0:
                printf("Exiting...\n");
                break;
        }
    } while (opt != 0);    
    break;
    case 2:{
        do {
    printf("\n=== QUAN LY LICH TRINH TAU HOA ===\n");
    printf("1. Nhap du lieu (thu cong/tu dong)\n");
    printf("2. Hien thi tat ca\n");
    printf("3. Them/Xoa du lieu (LIFO)\n");
    printf("4. Sap xep du lieu theo thoi gian/ngay\n");
    printf("5. Them du lieu sau khi sap xep\n");
    printf("6. Tong so chuyen theo ID (CHUYEN/TAU)\n");
    printf("7. Tim ga ban ron nhat\n");
    printf("8. Tong so hanh khach trong khoang thoi gian\n");
    printf("9. Chuyen di dai nhat\n");
    printf("10. So chuyen nhieu nhat trong mot ngay\n");
    printf("0. Thoat\n");
    printf("Lua chon: ");
    scanf("%d", &opt);
    getchar();

    switch (opt) {
        case 1:
            enter_data(&list);
            break;
        case 2:
            display_data(&list);
            break;
        case 3: {
            int C1;
            printf("Nhap 1 de them du lieu (LIFO)\n");
            printf("Nhap 2 de xoa du lieu (LIFO)\n");
            printf("Lua chon cua ban: ");
            scanf("%d", &C1);
            getchar();
            if (C1 == 1) {
                add_new_data_manually(&list);
                add_lifo(&list, list.tail);  
            } else if (C1 == 2) {
                char id[15];
                printf("Nhap ID chuyen/tau can xoa: ");
                scanf("%14s", id);
                to_upper_str(id);
                delete_lifo_by_id(&list, id);
            } else {
                printf("Lua chon khong hop le!\n");
            }
            sleep(2);
            break;
        }
        case 4: {
            printf("Dang sap xep du lieu theo thoi gian/ngay");
            fflush(stdout);
            for (int i = 0; i < 5; i++) {
                printf(".");
                sleep(1);
            }
            printf(" xong.\n");
            sorting_list_by_time_date(&list);
            display_data(&list);
            break;
        }
        case 5: {
            printf("Them du lieu sau khi sap xep\n");
            add_new_data_manually(&list);
            printf("Dang ap dung thay doi");
            fflush(stdout);
            for (int i = 0; i < 5; i++) {
                printf(".");
                sleep(1);
            }
            printf("\n");
            sorting_list_by_time_date(&list);
            display_data(&list);
            break;
        }
        case 6: {
            char id[15];
            printf("Nhap ID chuyen/tau de tim kiem: ");
            scanf("%14s", id);
            to_upper_str(id);
            printf("Dang tim kiem ID %s", id);
            fflush(stdout);
            for (int i = 0; i < 5; i++) {
                printf(".");
                usleep(500000);
            }
            int count = search_by_id_total(&list, id);
            printf("\nTim thay %d muc voi ID %s\n", count, id);
            sleep(2);
            break;
        }
        case 7: {
            printf("Dang tim ga ban ron nhat ");
            for (int i = 0; i < 5; i++) {
                printf(".");
                sleep(1);
            }
            printf("\n");
            printBusiestStation(&list);
            break;
        }
        case 8: {
            char b1[11], b2[11];
            printf("Nhap ngay bat dau (DD/MM/YYYY): ");
            scanf("%10s", b1);
            printf("Nhap ngay ket thuc (DD/MM/YYYY): ");
            scanf("%10s", b2);

            int d1 = get_day(b1), m1 = get_month(b1), y1 = get_year(b1);
            int d2 = get_day(b2), m2 = get_month(b2), y2 = get_year(b2);

            printf("Dang tinh so hanh khach tu %s den %s", b1, b2);
            fflush(stdout);
            for (int i = 0; i < 4; i++) {
                printf(".");
                sleep(1);
            }
            printf("\n");
            int total = totalPassengersInRange(&list, d1, m1, y1, d2, m2, y2);
            printf("Tong so hanh khach tu %s den %s: %d\n", b1, b2, total);
            break;
        }
        case 9: {
            printf("Dang tim chuyen di dai nhat:");
            for (int i = 0; i < 5; i++) {
                printf(".");
                sleep(1);
            }
            printf("\n");
            printLongestTrip(&list);
            sleep(2);
            break;
        }
        case 10: {
            printf("Dang tim ngay co nhieu chuyen nhat: ");
            for (int i = 0; i < 5; i++) {
                printf(".");
                sleep(1);
            }
            printf("\n");
            printMaxTripsInOneDay(&list);
            break;
        }
        case 0:
            printf("Dang thoat...\n");
            break;
    }
} while (opt != 0);
        
    default:
        break;
    
    free_list(&list);
    return 0;
}
}
}