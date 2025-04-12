#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>

// Booking structure as a linked list node
typedef struct Booking {
    int roomNumber;
    char name[50];
    char phone[15];
    int pricePerNight;
    char checkIn[15];
    char checkOut[15];
    int totalBill;
    struct Booking* next;
} Booking;

Booking* head = NULL;

int roomNumbers[] = {101, 102, 103, 104, 105, 201, 202, 203, 204, 205, 301, 302, 303, 304, 305};
int totalRooms = sizeof(roomNumbers) / sizeof(roomNumbers[0]);
char roomTypes[] = {'S', 'S', 'S', 'S', 'S', 'C', 'C', 'C', 'C', 'C', 'F', 'F', 'F', 'F', 'F'};

// Function to check if the room is valid
int isValidRoom(int room) {
    for (int i = 0; i < totalRooms; i++)
        if (room == roomNumbers[i]) return 1;
    return 0;
}

// Function to validate date
int isValidDate(int day, int month, int year) {
    struct tm date = {0};
    date.tm_mday = day;
    date.tm_mon = month - 1;
    date.tm_year = year - 1900;
    time_t t = mktime(&date);
    return (date.tm_mday == day && date.tm_mon == month - 1 && date.tm_year == year - 1900);
}

// Check room availability
int isRoomAvailable(int room, const char* checkIn, const char* checkOut) {
    Booking* temp = head;
    while (temp) {
        if (temp->roomNumber == room) {
            struct tm bookedIn = {0}, bookedOut = {0};
            sscanf(temp->checkIn, "%d-%d-%d", &bookedIn.tm_mday, &bookedIn.tm_mon, &bookedIn.tm_year);
            sscanf(temp->checkOut, "%d-%d-%d", &bookedOut.tm_mday, &bookedOut.tm_mon, &bookedOut.tm_year);
            bookedIn.tm_year -= 1900; bookedIn.tm_mon -= 1;
            bookedOut.tm_year -= 1900; bookedOut.tm_mon -= 1;

            struct tm inputIn = {0}, inputOut = {0};
            sscanf(checkIn, "%d-%d-%d", &inputIn.tm_mday, &inputIn.tm_mon, &inputIn.tm_year);
            sscanf(checkOut, "%d-%d-%d", &inputOut.tm_mday, &inputOut.tm_mon, &inputOut.tm_year);
            inputIn.tm_year -= 1900; inputIn.tm_mon -= 1;
            inputOut.tm_year -= 1900; inputOut.tm_mon -= 1;

            time_t bookedInTime = mktime(&bookedIn);
            time_t bookedOutTime = mktime(&bookedOut);
            time_t inputInTime = mktime(&inputIn);
            time_t inputOutTime = mktime(&inputOut);

            if ((inputInTime < bookedOutTime) && (inputOutTime > bookedInTime)) {
                return 0;
            }
        }
        temp = temp->next;
    }
    return 1;
}

// Get room price
int getRoomPrice(int room) {
    for (int i = 0; i < totalRooms; i++) {
        if (room == roomNumbers[i]) {
            char type = roomTypes[i];
            if (type == 'S') return 2000;
            if (type == 'C') return 3000;
            if (type == 'F') return 5000;
        }
    }
    return 0;
}

// Check-in Function (Updated)
void checkIn() {
    Booking* newBooking = (Booking*)malloc(sizeof(Booking));

    printf("\n\t\t\tEnter Guest Name: ");
    scanf(" %[^\n]", newBooking->name);

    while (1) {
        printf("\t\t\tEnter Phone (11 digits): ");
        scanf("%s", newBooking->phone);
        if (strlen(newBooking->phone) == 11) {
            int valid = 1;
            for (int i = 0; i < 11; i++)
                if (newBooking->phone[i] < '0' || newBooking->phone[i] > '9') valid = 0;
            if (valid) break;
        }
        printf("\t\t\tInvalid phone number!\n");
    }

    int nights;
    char checkInDate[15], checkOutDate[15];

    while (1) {
        int d, m, y;
        while (1) {
            printf("\n\t\t\tEnter check-in date (dd-mm-yyyy): ");
            scanf("%s", checkInDate);
            if (sscanf(checkInDate, "%d-%d-%d", &d, &m, &y) == 3 && isValidDate(d, m, y))
                break;
            printf("\t\t\tInvalid date! Please enter a valid date.\n");
        }

        printf("\n\t\t\tEnter number of nights: ");
        scanf("%d", &nights);

        struct tm checkInStruct = {0};
        sscanf(checkInDate, "%d-%d-%d", &checkInStruct.tm_mday, &checkInStruct.tm_mon, &checkInStruct.tm_year);
        checkInStruct.tm_year -= 1900;
        checkInStruct.tm_mon -= 1;

        time_t checkInTime = mktime(&checkInStruct);
        checkInTime += (nights * 24 * 60 * 60);
        struct tm *newCheckOutDate = localtime(&checkInTime);
        strftime(checkOutDate, sizeof(checkOutDate), "%d-%m-%Y", newCheckOutDate);

        printf("\n\t\t\tChecking room availability...\n");
        int availableRoom = 0;
        printf("\n\t\t\tAvailable Rooms:\n");
        for (int i = 0; i < totalRooms; i++) {
            int room = roomNumbers[i];
            if (isRoomAvailable(room, checkInDate, checkOutDate)) {
                availableRoom = 1;
                printf("\t\t\tRoom %d - Price: %d tk\n", room, getRoomPrice(room));
            }
        }

        if (!availableRoom) {
            printf("\n\t\t\tNo rooms available for the given dates.\n");
            continue;
        }

        while (1) {
            printf("\n\t\t\tEnter Room Number: ");
            scanf("%d", &newBooking->roomNumber);
            if (!isValidRoom(newBooking->roomNumber)) {
                printf("\n\t\t\tInvalid Room Number!\n");
                continue;
            }
            if (!isRoomAvailable(newBooking->roomNumber, checkInDate, checkOutDate)) {
                printf("\n\t\t\tRoom already booked for the given dates.\n");
                continue;
            }
            break;
        }

        newBooking->pricePerNight = getRoomPrice(newBooking->roomNumber);
        newBooking->totalBill = newBooking->pricePerNight * nights;
        strcpy(newBooking->checkIn, checkInDate);
        strcpy(newBooking->checkOut, checkOutDate);
        newBooking->next = head;
        head = newBooking;

        printf("\n\t\t\tBooking added! Total bill: %d\n", newBooking->totalBill);
        break;
    }
}

// View available rooms
void viewAvailableRooms() {
    printf("\n\t\t\tAvailable Rooms and Rates:\n");
    for (int i = 0; i < totalRooms; i++) {
        int price = getRoomPrice(roomNumbers[i]);
        printf("\t\t\tRoom %d - Price: %d tk\n", roomNumbers[i], price);
    }
    printf("\n\t\t\tEnd of Room List\n");
}

// View check-in details
void viewCheckInDetails() {
    Booking* temp = head;
    if (!temp) {
        printf("\n\t\t\tNo bookings found!\n");
        return;
    }
    printf("\n\t\t\t--- Check-in Details ---\n");
    while (temp) {
        printf("\n\t\t\tRoom Number: %d\n", temp->roomNumber);
        printf("\t\t\tName: %s\n", temp->name);
        printf("\t\t\tPhone: %s\n", temp->phone);
        printf("\t\t\tCheck-in: %s\n", temp->checkIn);
        printf("\t\t\tCheck-out: %s\n", temp->checkOut);
        printf("\t\t\tTotal Bill: %d\n", temp->totalBill);
        temp = temp->next;
    }
}

// Check-out
void checkOut() {
    int room;
    printf("\n\t\t\tEnter Room Number for Check-out: ");
    scanf("%d", &room);

    Booking* temp = head;
    Booking* prev = NULL;
    while (temp) {
        if (temp->roomNumber == room) {
            printf("\n\t\t\tChecking out Room %d...\n", room);
            printf("\t\t\tName: %s\n", temp->name);
            printf("\t\t\tPhone: %s\n", temp->phone);
            printf("\t\t\tCheck-in: %s\n", temp->checkIn);
            printf("\t\t\tCheck-out: %s\n", temp->checkOut);
            printf("\t\t\tTotal Bill: %d\n", temp->totalBill);

            if (prev)
                prev->next = temp->next;
            else
                head = temp->next;

            free(temp);
            printf("\n\t\t\tRoom %d has been checked out!\n", room);
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    printf("\n\t\t\tRoom not found!\n");
}

// Extend Reservation
void extendReservation() {
    int room;
    printf("\n\t\t\tEnter Room Number to extend reservation: ");
    scanf("%d", &room);

    Booking* temp = head;
    while (temp) {
        if (temp->roomNumber == room) {
            int extension;
            printf("\n\t\t\tEnter number of additional nights to extend: ");
            scanf("%d", &extension);

            struct tm checkOutDate = {0};
            sscanf(temp->checkOut, "%d-%d-%d", &checkOutDate.tm_mday, &checkOutDate.tm_mon, &checkOutDate.tm_year);
            checkOutDate.tm_year -= 1900;
            checkOutDate.tm_mon -= 1;

            time_t checkOutTime = mktime(&checkOutDate);
            checkOutTime += (extension * 24 * 60 * 60);
            struct tm *newCheckOutDate = localtime(&checkOutTime);
            char extendedCheckOut[15];
            strftime(extendedCheckOut, sizeof(extendedCheckOut), "%d-%m-%Y", newCheckOutDate);

            strcpy(temp->checkOut, extendedCheckOut);
            temp->totalBill += temp->pricePerNight * extension;

            printf("\n\t\t\tReservation extended!\n");
            printf("\t\t\tNew Check-out: %s\n", temp->checkOut);
            printf("\t\t\tNew Total Bill: %d\n", temp->totalBill);

            return;
        }
        temp = temp->next;
    }

    printf("\n\t\t\tRoom not found!\n");
}

// Login
int login() {
    char username[50], password[50];
    printf("\n\n\n\n\t\t\t\t   Welcome to --- ``Hotel Management System`` ---\n");
    printf("\n\t\t\t\t\t(Developed By--  'Team Pentagon')\n");
    printf("\t\t\t-----------------------------------------------------------------\n\n\n\n");
    printf("\n\t\t\t\t\t---Login Info(For Management)---\n");
    printf("\t\t\t\t-------------------------------------------------\n\n");
    printf("\t\t\tEnter Username: ");
    scanf("%s", username);

    printf("\t\t\tEnter Password: ");
    int i = 0;
    char ch;
    while ((ch = getch()) != '\r') {
        password[i++] = ch;
        printf("*");
    }
    password[i] = '\0';

    if (strcmp(username, "admin") == 0 && strcmp(password, "1234") == 0) {
        printf("\n\t\t\tLogin successful!\n");
        return 1;
    } else {
        printf("\n\t\t\tInvalid credentials. Please try again.\n");
        return 0;
    }
}

// Main
int main() {
    if (!login()) return 0;

    int choice;
    while (1) {
        printf("\n\t\t\t--- Hotel Management System ---\n");
        printf("\t\t\t1. View Available Rooms\n");
        printf("\t\t\t2. Check-in\n");
        printf("\t\t\t3. View Check-in Details\n");
        printf("\t\t\t4. Check-out\n");
        printf("\t\t\t5. Extend Reservation\n");
        printf("\t\t\t6. Exit\n");
        printf("\t\t\tEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: viewAvailableRooms(); break;
            case 2: checkIn(); break;
            case 3: viewCheckInDetails(); break;
            case 4: checkOut(); break;
            case 5: extendReservation(); break;
            case 6:
                printf("\n\t\t\tExiting program...\n");
                exit(0);
            default:
                printf("\n\t\t\tInvalid choice! Please try again.\n");
        }
    }

    return 0;
}
