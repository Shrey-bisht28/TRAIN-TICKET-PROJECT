#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TRAINS 5
#define MAX_SEATS 100
#define MAX_BOOKINGS 1000
#define NAME_LEN 50
#define PNR_LEN 20
#define FILENAME "bookings.dat"

typedef struct {
    int train_id;
    char name[50];
    char source[30];
    char dest[30];
    int total_seats;
    int seats_booked;
} Train;

typedef struct {
    char pnr[PNR_LEN];
    int train_id;
    char passenger_name[NAME_LEN];
    int age;
    char gender[10];
    int seat_no;
    int active;
} Booking;

Train trains[MAX_TRAINS];
Booking bookings[MAX_BOOKINGS];
int booking_count = 0;

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void init_trains() {
   
trains[0].train_id = 101; strcpy(trains[0].name, "Coastal Express"); strcpy(trains[0].source, "Mumbai"); 
strcpy(trains[0].dest, "Goa"); trains[0].total_seats = 50; trains[0].seats_booked = 0;
trains[1].train_id = 102; strcpy(trains[1].name, "Mountain Flyer");  
 strcpy(trains[1].source, "Delhi"); strcpy(trains[1].dest, "Shimla"); trains[1].total_seats = 40; trains[1].seats_booked = 0;
trains[2].train_id = 103; strcpy(trains[2].name, "Desert Rider");   
 strcpy(trains[2].source, "Jaipur"); strcpy(trains[2].dest, "Jodhpur"); trains[2].total_seats = 60; trains[2].seats_booked = 0;
trains[3].train_id = 104; strcpy(trains[3].name, "Riverine Line");  
 strcpy(trains[3].source, "Kolkata"); strcpy(trains[3].dest, "Puri"); trains[3].total_seats = 55; trains[3].seats_booked = 0;
trains[4].train_id = 105; strcpy(trains[4].name, "Southern Star"); 
  strcpy(trains[4].source, "Bangalore"); strcpy(trains[4].dest, "Chennai"); trains[4].total_seats = 45; trains[4].seats_booked = 0;
}

Train* find_train_by_id(int id) {
    for (int i = 0; i < MAX_TRAINS; ++i) {
        if (trains[i].train_id == id) return &trains[i];
    }
    return NULL;
}

void generate_pnr(char *buf, int bufsize) {
    static int counter = 1000;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    
    snprintf(buf, bufsize, "%04d%02d%02d-%04d",
             tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, counter++);
}


void save_bookings_to_file() {
    FILE *fp = fopen(FILENAME, "wb");
    if (!fp) {
        perror("Unable to save bookings");
        return;
    }
    fwrite(&booking_count, sizeof(int), 1, fp);
    fwrite(bookings, sizeof(Booking), booking_count, fp);
    fclose(fp);
}

void load_bookings_from_file() {
    FILE *fp = fopen(FILENAME, "rb");
    if (!fp) {
      
        return;
    }
    int cnt = 0;
    if (fread(&cnt, sizeof(int), 1, fp) != 1) {
        fclose(fp); return;
    }
    if (cnt > MAX_BOOKINGS) cnt = MAX_BOOKINGS;
    if (fread(bookings, sizeof(Booking), cnt, fp) != (size_t)cnt) {
        
    }
    booking_count = cnt;

    
    for (int i = 0; i < MAX_TRAINS; ++i) trains[i].seats_booked = 0;
    for (int i = 0; i < booking_count; ++i) {
        if (bookings[i].active) {
            Train *t = find_train_by_id(bookings[i].train_id);
            if (t) t->seats_booked++;
        }
    }
    fclose(fp);
}


void list_trains() {
    printf("\nAvailable Trains:\n");
    printf("ID\tName\t\t\tFrom -> To\tSeats(available/total)\n");
    printf("----------------------------------------------------------------\n");
    for (int i = 0; i < MAX_TRAINS; ++i) {
        printf("%d\t%-18s\t%s -> %s\t%d/%d\n",
               trains[i].train_id,
               trains[i].name,
               trains[i].source,
               trains[i].dest,
               trains[i].total_seats - trains[i].seats_booked,
               trains[i].total_seats);
    }
    printf("\n");
}

void view_bookings() {
    if (booking_count == 0) {
        printf("\nNo bookings yet.\n");
        return;
    }
    printf("\nAll Bookings:\n");
    printf("PNR\t\tTrainID\tSeat\tName\t\tAge\tGender\tStatus\n");
    printf("--------------------------------------------------------------------\n");
    for (int i = 0; i < booking_count; ++i) {
        Booking *b = &bookings[i];
        printf("%s\t%d\t%d\t%-12s\t%d\t%-6s\t%s\n",
               b->pnr,
               b->train_id,
               b->seat_no,
               b->passenger_name,
               b->age,
               b->gender,
               b->active ? "Booked" : "Cancelled");
    }
    printf("\n");
}

int find_free_seat_number(Train *t) {
    
    int used[MAX_SEATS + 1] = {0};
    for (int i = 0; i < booking_count; ++i) {
        if (bookings[i].active && bookings[i].train_id == t->train_id) {
            if (bookings[i].seat_no >= 1 && bookings[i].seat_no <= t->total_seats)
                used[bookings[i].seat_no] = 1;
        }
    }
    for (int s = 1; s <= t->total_seats; ++s) {
        if (!used[s]) return s;
    }
    return -1;
}

void book_ticket() {
    int train_id;
    char name[NAME_LEN];
    int age;
    char gender[10];

    list_trains();
    printf("Enter train ID to book: ");
    if (scanf("%d", &train_id) != 1) { clear_stdin(); printf("Invalid input.\n"); return; }
    clear_stdin();

    Train *t = find_train_by_id(train_id);
    if (!t) { printf("Train with ID %d not found.\n", train_id); return; }

    if (t->seats_booked >= t->total_seats) {
        printf("No seats available on this train.\n");
        return;
    }

    printf("Enter passenger name: ");
    if (!fgets(name, sizeof(name), stdin)) { printf("Input error\n"); return; }
    name[strcspn(name, "\n")] = 0;

    if (strlen(name) == 0) {
        printf("Name cannot be empty.\n"); return;
    }

    printf("Enter age: ");
    if (scanf("%d", &age) != 1) { clear_stdin(); printf("Invalid age.\n"); return; }
    clear_stdin();

    printf("Enter gender (M/F/Other): ");
    if (!fgets(gender, sizeof(gender), stdin)) { printf("Input error\n"); return; }
    gender[strcspn(gender, "\n")] = 0;

    int seat_no = find_free_seat_number(t);
    if (seat_no == -1) { printf("No free seat found (unexpected).\n"); return; }

    if (booking_count >= MAX_BOOKINGS) {
        printf("Booking storage full.\n"); return;
    }

    Booking newb;
    generate_pnr(newb.pnr, sizeof(newb.pnr));
    newb.train_id = train_id;
    strncpy(newb.passenger_name, name, NAME_LEN);
    newb.age = age;
    strncpy(newb.gender, gender, sizeof(newb.gender));
    newb.seat_no = seat_no;
    newb.active = 1;

    bookings[booking_count++] = newb;
    t->seats_booked++;

    printf("\nTicket booked successfully!\n");
    printf("PNR: %s\nTrain: %d (%s)\nPassenger: %s\nSeat no: %d\n\n",
           newb.pnr, t->train_id, t->name, newb.passenger_name, newb.seat_no);

    save_bookings_to_file();
}

void cancel_ticket() {
    char pnr[PNR_LEN];
    printf("Enter PNR to cancel: ");
    if (!fgets(pnr, sizeof(pnr), stdin)) { printf("Input error\n"); return; }
    pnr[strcspn(pnr, "\n")] = 0;

    for (int i = 0; i < booking_count; ++i) {
        if (strcmp(bookings[i].pnr, pnr) == 0) {
            if (!bookings[i].active) {
                printf("Booking already cancelled.\n");
                return;
            }
            bookings[i].active = 0;
            Train *t = find_train_by_id(bookings[i].train_id);
            if (t && t->seats_booked > 0) t->seats_booked--;
            printf("Booking with PNR %s cancelled successfully.\n", pnr);
            save_bookings_to_file();
            return;
        }
    }
    printf("PNR not found.\n");
}

void show_menu() {
    printf("\n===== Train Ticket Reservation System =====\n");
    printf("1. List Trains\n");
    printf("2. Book Ticket\n");
    printf("3. Cancel Ticket\n");
    printf("4. View All Bookings\n");
    printf("5. Exit\n");
    printf("Select option: ");
}

int main() {
    init_trains();
    load_bookings_from_file();

    int choice = 0;
    while (1) {
        show_menu();
        if (scanf("%d", &choice) != 1) {
            clear_stdin();
            printf("Invalid choice.\n");
            continue;
        }
        clear_stdin();

        switch (choice) {
            case 1:
                list_trains();
                break;
            case 2:
                book_ticket();
                break;
            case 3:
                cancel_ticket();
                break;
            case 4:
                view_bookings();
                break;
            case 5:
                printf("Goodbye — saving bookings and exiting.\n");
                save_bookings_to_file();
                exit(0);
                break;
            default:
                printf("Invalid option. Try again.\n");
        }
    }
    return 0;
}