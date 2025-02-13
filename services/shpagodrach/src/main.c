#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/inotify.h>
#include <openssl/sha.h>
#include "vtable_lib.h"

#define GLADIATORS_DIR "/tmp/gladiators/"
#define BANANA_FILE "banana.jpg"
#define BANANA_EXPECTED_HASH "2baac52a877358ae99b8dbcdbd362ef6db982e21365e83cafa52255a672fe5e8"

void* monitor_banana(void* arg);

int verify_banana() {
    FILE *fp = fopen(BANANA_FILE, "rb");
    if (!fp) {
        return -1;
    }
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    if (SHA256_Init(&sha256) == 0) {
         fclose(fp);
         return -1;
    }
    const int bufSize = 32768;
    unsigned char *buffer = malloc(bufSize);
    if (!buffer) {
         fclose(fp);
         return -1;
    }
    int bytesRead = 0;
    while ((bytesRead = fread(buffer, 1, bufSize, fp)) > 0) {
         SHA256_Update(&sha256, buffer, bytesRead);
    }
    SHA256_Final(hash, &sha256);
    fclose(fp);
    free(buffer);
    char outputBuffer[65];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
         sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = '\0';
    if (strcmp(outputBuffer, BANANA_EXPECTED_HASH) != 0) {
         fprintf(stderr, "Error: %s failed integrity check.\n", BANANA_FILE);
         return -1;
    }
    return 0;
}

void* monitor_banana(void* arg) {
    int fd = inotify_init();
    if (fd < 0) {
         perror("inotify_init");
         exit(1);
    }
    int wd = inotify_add_watch(fd, ".", IN_DELETE | IN_MOVED_FROM);
    if (wd < 0) {
         perror("inotify_add_watch");
         exit(1);
    }
    char buf[4096] __attribute__((aligned(8)));
    while (1) {
         int length = read(fd, buf, sizeof(buf));
         if (length < 0) {
             perror("read");
             continue;
         }
         int i = 0;
         while (i < length) {
             struct inotify_event *event = (struct inotify_event *) &buf[i];
             if (event->len > 0) {
                 if (strcmp(event->name, BANANA_FILE) == 0) {
                     if (event->mask & (IN_DELETE | IN_MOVED_FROM)) {
                         exit(1);
                     }
                 }
             }
             i += sizeof(struct inotify_event) + event->len;
         }
    }
    return NULL;
}

char current_filename[256] = {0};

void createGladiator() {
    char name[40];
    char pwd[40];
    char comment[40];

    printf("Enter name (login): ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Enter password: ");
    fgets(pwd, sizeof(pwd), stdin);
    pwd[strcspn(pwd, "\n")] = '\0';

    printf("Enter comment: ");
    fgets(comment, sizeof(comment), stdin);
    comment[strcspn(comment, "\n")] = '\0';

    srand(time(NULL));
    int id = rand();
    char filename[256];
    snprintf(filename, sizeof(filename), "%sgladiator_%d.dat", GLADIATORS_DIR, id);

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("Error creating file\n");
        return;
    }

    Gladiator g;
    memset(&g, 0, sizeof(g));
    strncpy(g.name, name, sizeof(g.name) - 1);
    strncpy(g.pwd, pwd, sizeof(g.pwd) - 1);
    strncpy(g.comment, comment, sizeof(g.comment) - 1);

    fwrite(&g, sizeof(g), 1, fp);
    fclose(fp);

    printf("Gladiator created! ID = %d\n", id);
}

int loginGladiator() {
    char name[40];
    char pwd[40];

    printf("Enter name (login): ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Enter password: ");
    fgets(pwd, sizeof(pwd), stdin);
    pwd[strcspn(pwd, "\n")] = '\0';

    DIR *d = opendir(GLADIATORS_DIR);
    if (!d) {
        printf("Error: failed to open directory %s\n", GLADIATORS_DIR);
        return 0;
    }

    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        if (strstr(de->d_name, "gladiator_") == de->d_name) {
            char path[512];
            snprintf(path, sizeof(path), "%s%s", GLADIATORS_DIR, de->d_name);

            FILE *fp = fopen(path, "rb");
            if (!fp) continue;

            Gladiator g;
            if (fread(&g, sizeof(g), 1, fp) != 1) {
                fclose(fp);
                continue;
            }
            fclose(fp);

            if(strcmp(g.name, name) == 0 && strcmp(g.pwd, pwd) == 0) {
                memcpy(&current, &g, sizeof(Gladiator));
                current.vtable = &default_vtable;
                strncpy(current_filename, path, sizeof(current_filename) - 1);
                current_filename[sizeof(current_filename) - 1] = '\0';
            
                closedir(d);
                printf("Successfully hired gladiator [%s]!\n", current.name);
                return 1;
            }
        }
    }
    closedir(d);

    printf("No such gladiator or incorrect password.\n");
    return 0;
}

void updateCurrentGladiator() {
    if (current.name[0] == '\0') {
        printf("Please hire a gladiator (login) first!\n");
        return;
    }

    printf("Which field would you like to edit?\n");
    printf("1) name\n");
    printf("2) password\n");
    printf("3) comment\n");
    printf("Enter index: ");

    int idx;
    if (scanf("%d", &idx) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    printf("Enter new value: ");    
    char *base = (char *)&current;
    char *dest = base + 40 * (idx - 1);
    
    int bytes_read = read(0, dest, 40);
    if (bytes_read > 0) {
        // Удаляем символ перевода строки, если он присутствует
        for (int i = 0; i < bytes_read; i++) {
            if (dest[i] == '\n') {
                dest[i] = '\0';
                break;
            }
        }
    }

    printf("Parameter #%d updated (offset %ld).\n", idx, (long)(dest - base));

    if (current_filename[0] == '\0') {
        printf("Error: no file information available for saving changes.\n");
        return;
    }
    FILE *fp = fopen(current_filename, "wb");
    if (!fp) {
        printf("Error opening file for update.\n");
        return;
    }
    fwrite(&current, sizeof(current), 1, fp);
    fclose(fp);
    printf("Changes saved to file: %s\n", current_filename);
}

void showCurrentGladiator() {
    if (current.name[0] == '\0') {
        printf("Please hire a gladiator (login) first!\n");
        return;
    }
    printf("Name: %s\n", current.name);
    printf("Comment: %s\n", current.comment);
}

void fight() {
    if (current.name[0] == '\0') {
        printf("Please hire a gladiator (login) first!\n");
        return;
    }

    DIR *d = opendir(GLADIATORS_DIR);
    if (!d) {
        printf("Error opening directory %s\n", GLADIATORS_DIR);
        return;
    }

    struct dirent *fileList[1024];
    int count = 0;
    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        if (strstr(de->d_name, "gladiator_") == de->d_name) {
            fileList[count++] = de;
        }
    }
    closedir(d);

    if (count == 0) {
        printf("No gladiators in folder, no one to fight.\n");
        return;
    }

    srand(time(NULL));
    int idx = rand() % count;

    char path[512];
    snprintf(path, sizeof(path), "%s%s", GLADIATORS_DIR, fileList[idx]->d_name);

    FILE *fp = fopen(path, "rb");
    if (!fp) {
        printf("Failed to open enemy file.\n");
        return;
    }

    Gladiator enemy;
    fread(&enemy, sizeof(enemy), 1, fp);
    fclose(fp);

    printf("Fight: %s vs %s\n", current.name, enemy.name);
    for (int i = 0; i < 3; i++) {
        sleep(1);
        printf(".");
    }
    printf("\n");

    if (current.vtable && current.vtable->attack)
        current.vtable->attack(&current);
    else
        printf("[Error] vtable not initialized!\n");

    int res = rand() % 2;
    if (res == 0) {
        printf("Winner -> %s\n", enemy.name);
        printf("You lost!\n");
    } else {
        printf("Winner -> %s\n", current.name);
        printf("You won!\n");
        printf(current.comment);
    }
}

void banner() {
    puts("==================================================================");
    puts(" _____ _   _______  ___  _____ _________________  ___  _____ _   _");
    puts("/  ___| | | | ___ \\/ _ \\|  __ |  _  |  _  | ___ \\/ _ \\/  __ | | | |");
    puts("\\ `--.| |_| | |_/ / /_\\ | |  \\| | | | | | | |_/ / /_\\ | /  \\| |_| |");
    puts(" `--. |  _  |  __/|  _  | | __| | | | | | |    /|  _  | |   |  _  |");
    puts("/\\__/ | | | | |   | | | | |_\\ \\ \\_/ | |/ /| |\\ \\| | | | \\__/| | | |");
    puts("\\____/\\_| |_\\_|   \\_| |_/\\____/\\___/|___/ \\_| \\_\\_| |_/\\____\\_| |_|");
    puts("===================================================================");
}

int main() {
    if (verify_banana() != 0) {
        exit(1);
    }

    pthread_t tid;
    if (pthread_create(&tid, NULL, monitor_banana, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    mkdir(GLADIATORS_DIR, 0755);

    banner();

    setbuf(stdout, NULL);
    setbuf(stdin,  NULL);
    setbuf(stderr, NULL);

    memset(&current, 0, sizeof(current));

    while (1) {
        printf("\n--- Gladiator Arena ---\n");
        printf("1) Create a gladiator\n");
        printf("2) Hire an existing gladiator\n");
        printf("3) Edit current gladiator parameters\n");
        printf("4) View current gladiator parameters\n");
        printf("5) Go to battle\n");
        printf("6) Exit\n");
        printf("> ");

        int choice;
        if (scanf("%d", &choice) != 1)
            break;
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                createGladiator();
                break;
            case 2:
                loginGladiator();
                break;
            case 3:
                updateCurrentGladiator();
                break;
            case 4:
                showCurrentGladiator();
                break;
            case 5:
                fight();
                break;
            case 6:
                printf("Goodbye!\n");
                exit(0);
            default:
                printf("Invalid menu option.\n");
                break;
        }
    }

    return 0;
}
