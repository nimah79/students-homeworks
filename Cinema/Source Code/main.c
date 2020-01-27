#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>
#include <langinfo.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>
#include "json.c"
#include "json-builder.c"

// 2000-03-01 (mod 400 year, immediately after feb29
#define LEAPOCH (946684800LL + 86400 * (31 + 29))

#define DAYS_PER_400Y (365 * 400 + 97)
#define DAYS_PER_100Y (365 * 100 + 24)
#define DAYS_PER_4Y (365 * 4 + 1)

const char *databaseFilepath = "data.json";

typedef struct {
    int startTimestamp, finishTimestamp, movieId, capacity;
    char *hallTitle;
} Session;

typedef struct {
    int id, duration, castCount;
    char *title, *genre, *plot, **cast;
} Movie;

typedef struct {
    char *title;
    int capacity, sessionsCount;
    Session *sessions;
} Hall;

Hall *halls;
Movie *movies;
int hallsCount;
int moviesCount;

void swap(void *i, void *j, size_t size)
{
    void* tmp = malloc(size);
    memcpy(tmp, j, size);
    memcpy(j, i, size);
    memcpy(i, tmp, size);
    free(tmp);
}

char *strptime(const char *s, char *f, struct tm *tm) {
    int i, w, neg, adj, min, range, *dest;
    const char *ex;
    size_t len;
    while (*f) {
        if (*f != '%') {
            if (isspace(*f)) for (; *s && isspace(*s); s++);
            else if (*s != *f) return 0;
            else s++;
            f++;
            continue;
        }
        f++;
        if (*f == '+') f++;
        if (isdigit(*f)) w = strtoul(f, &f, 10);
        else w = -1;
        adj = 0;
        switch (*f++) {
            case 'a':
            case 'A':
                dest = &tm->tm_wday;
                min = ABDAY_1;
                range = 7;
                goto symbolic_range;
            case 'b':
            case 'B':
            case 'h':
                dest = &tm->tm_mon;
                min = ABMON_1;
                range = 12;
                goto symbolic_range;
            case 'c':
                s = strptime(s, nl_langinfo(D_T_FMT), tm);
                if (!s) return 0;
                break;
            case 'C':
            case 'd':
            case 'e':
                dest = &tm->tm_mday;
                min = 1;
                range = 31;
                goto numeric_range;
            case 'D':
                s = strptime(s, (char*) "%m/%d/%y", tm);
                if (!s) return 0;
                break;
            case 'H':
                dest = &tm->tm_hour;
                min = 0;
                range = 24;
                goto numeric_range;
            case 'I':
                dest = &tm->tm_hour;
                min = 1;
                range = 12;
                goto numeric_range;
            case 'j':
                dest = &tm->tm_yday;
                min = 1;
                range = 366;
                goto numeric_range;
            case 'm':
                dest = &tm->tm_mon;
                min = 1;
                range = 12;
                adj = 1;
                goto numeric_range;
            case 'M':
                dest = &tm->tm_min;
                min = 0;
                range = 60;
                goto numeric_range;
            case 'n':
            case 't':
                for (; *s && isspace(*s); s++);
                break;
            case 'p':
                ex = nl_langinfo(AM_STR);
                len = strlen(ex);
                if (!strncasecmp(s, ex, len)) {
                    tm->tm_hour %= 12;
                    break;
                }
                ex = nl_langinfo(PM_STR);
                len = strlen(ex);
                if (!strncasecmp(s, ex, len)) {
                    tm->tm_hour %= 12;
                    tm->tm_hour += 12;
                    break;
                }
                return 0;
            case 'r':
                s = strptime(s, nl_langinfo(T_FMT_AMPM), tm);
                if (!s) return 0;
                break;
            case 'R':
                s = strptime(s, (char*) "%H:%M", tm);
                if (!s) return 0;
                break;
            case 'S':
                dest = &tm->tm_sec;
                min = 0;
                range = 61;
                goto numeric_range;
            case 'T':
                s = strptime(s, (char*) "%H:%M:%S", tm);
                if (!s) return 0;
                break;
            case 'U':
            case 'W':
                //FIXME
                return 0;
            case 'w':
                dest = &tm->tm_wday;
                min = 0;
                range = 7;
                goto numeric_range;
            case 'x':
                s = strptime(s, nl_langinfo(D_FMT), tm);
                if (!s) return 0;
                break;
            case 'X':
                s = strptime(s, nl_langinfo(T_FMT), tm);
                if (!s) return 0;
                break;
            case 'y':
                //FIXME
                return 0;
            case 'Y':
                dest = &tm->tm_year;
                if (w < 0) w = 4;
                adj = 1900;
                goto numeric_digits;
            case '%':
                if (*s++ != '%') return 0;
                break;
            numeric_range:
                if (!isdigit(*s)) return 0;
                *dest = 0;
                for (i = 1; i <= min + range && isdigit(*s); i *= 10)
                    *dest = *dest * 10 + *s++ - '0';
                if (*dest - min >= (unsigned) range) return 0;
                *dest -= adj;
                switch ((char *) dest - (char *) tm) {
                    case offsetof(struct tm, tm_yday):;
                }
                goto update;
            numeric_digits:
                neg = 0;
                if (*s == '+') s++;
                else if (*s == '-') neg = 1, s++;
                if (!isdigit(*s)) return 0;
                for (*dest = i = 0; i < w && isdigit(*s); i++)
                    *dest = *dest * 10 + *s++ - '0';
                if (neg) *dest = -*dest;
                *dest -= adj;
                goto update;
            symbolic_range:
                for (i = 2 * range - 1; i >= 0; i--) {
                    ex = nl_langinfo(min + i);
                    len = strlen(ex);
                    if (strncasecmp(s, ex, len)) continue;
                    s += len;
                    *dest = i % range;
                    break;
                }
                if (i < 0) return 0;
                goto update;
            update:
                //FIXME
                ;
        }
    }
    return (char *) s;
}

void sort(Session* mass, int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = 0; j < n-i-1; ++j)
        {
            if(mass[j].startTimestamp > mass[j+1].startTimestamp)
            {
                swap(&mass[j],&mass[j+1], sizeof(Session));
            }
        }
    }
}

int partition(Session arr[], int low, int high)
{
   int pivot = high, index = low;
   for (int i = low; i < high; ++i) {
      if (arr[i].startTimestamp < arr[pivot].startTimestamp)
         swap(&arr[i++], &arr[index], sizeof(Session));
   }
   swap(&arr[pivot], &arr[index], sizeof(Session));
   return index;
}

int partitionPtr(Session *arr[], int low, int high)
{
   int pivot = high, index = low;
   for (int i = low; i < high; ++i) {
      if (arr[i]->startTimestamp < arr[pivot]->startTimestamp)
         swap(&arr[i++], &arr[index], sizeof(Session *));
   }
   swap(&arr[pivot], &arr[index], sizeof(Session *));
   return index;
}

int RandomPivotPartition(Session arr[], int low, int high)
{
   swap(&arr[high], &arr[low + rand() % (high - low + 1)], sizeof(Session));
   return partition(arr, low, high);
}

int RandomPivotPartitionPtr(Session *arr[], int low, int high)
{
   swap(&arr[high], &arr[low + rand() % (high - low + 1)], sizeof(Session *));
   return partitionPtr(arr, low, high);
}

void quickSort(Session arr[], int p, int q)
{
    if (p < q) {
        int pindex = RandomPivotPartition(arr, p, q);
        quickSort(arr, p, pindex - 1);
        quickSort(arr, pindex + 1, q);
    }
}

void quickSortPtr(Session *arr[], int p, int q)
{
    if (p < q) {
        int pindex = RandomPivotPartitionPtr(arr, p, q);
        quickSortPtr(arr, p, pindex - 1);
        quickSortPtr(arr, pindex + 1, q);
    }
}

char *timestampToDate(int timestamp) {
    long long days, secs;
    int remdays, remsecs, remyears;
    int qc_cycles, c_cycles, q_cycles;
    int years, months;
    int wday, yday, leap;
    const char days_in_month[] = {31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29};
    if (timestamp < INT_MIN * 31622400LL || timestamp > INT_MAX * 31622400LL)
        return NULL;
    secs = timestamp - LEAPOCH;
    days = secs / 86400;
    remsecs = secs % 86400;
    if (remsecs < 0) {
        remsecs += 86400;
        days--;
    }
    wday = (3 + days) % 7;
    if (wday < 0) wday += 7;
    qc_cycles = days / DAYS_PER_400Y;
    remdays = days % DAYS_PER_400Y;
    if (remdays < 0) {
        remdays += DAYS_PER_400Y;
        qc_cycles--;
    }
    c_cycles = remdays / DAYS_PER_100Y;
    if (c_cycles == 4) c_cycles--;
    remdays -= c_cycles * DAYS_PER_100Y;
    q_cycles = remdays / DAYS_PER_4Y;
    if (q_cycles == 25) q_cycles--;
    remdays -= q_cycles * DAYS_PER_4Y;
    remyears = remdays / 365;
    if (remyears == 4) remyears--;
    remdays -= remyears * 365;
    leap = !remyears && (q_cycles || !c_cycles);
    yday = remdays + 31 + 28 + leap;
    if (yday >= 365 + leap) yday -= 365 + leap;

    years = remyears + 4 * q_cycles + 100 * c_cycles + 400 * qc_cycles;

    for (months = 0; days_in_month[months] <= remdays; months++)
        remdays -= days_in_month[months];

    if (years + 100 > INT_MAX || years + 100 < INT_MIN)
        return NULL;

    years += 2000;
    months += 2;
    if (months >= 12) {
        months -= 12;
        ++years;
    }
    char *result = (char *) calloc(80, sizeof(char));
    snprintf(result, 80 * sizeof(char), "%d-%02d-%02d %02d:%02d:%02d", years, ++months, ++remdays, remsecs / 3600,
             remsecs / 60 % 60, remsecs % 60);
    return result;
}

bool addSession(Hall *hall, int movieId, int startTimestamp, int finishTimestamp) {
    for (int i = 0; i < hall->sessionsCount; ++i) {
        if (finishTimestamp > hall->sessions[i].startTimestamp && hall->sessions[i].finishTimestamp > startTimestamp) {
            return false;
        }
    }
    ++hall->sessionsCount;
    hall->sessions = (Session *) realloc(hall->sessions, hall->sessionsCount * sizeof(Session));
    Session *session = &hall->sessions[hall->sessionsCount - 1];
    session->startTimestamp = startTimestamp;
    session->finishTimestamp = finishTimestamp;
    session->movieId = movieId;
    session->capacity = hall->capacity;
    session->hallTitle = hall->title;
    return true;
}

Movie getMovieById(int id) {
    for (int i = 0; i < moviesCount; ++i) {
        if (movies[i].id == id) {
            return movies[i];
        }
    }
    Movie notFound = (Movie) {-1};
    return notFound;
}

void showMovie(int i, Movie movie) {
    printf("%d. %s\n", i + 1, movie.title);
    printf("Genre: %s\n", movie.genre);
    printf("Duration: %d\n", movie.duration);
    printf("Plot: %s\n", movie.plot);
    printf("Cast:\n");
    for (int j = 0; j < movie.castCount; ++j) {
        printf("- %s\n", movie.cast[j]);
    }
}

void showMovies() {
    printf("List of movies:\n\n");
    for (int i = 0; i < moviesCount; ++i) {
        showMovie(i, movies[i]);
        printf("\n");
    }
}

bool showSessions() {
    int count = 0;
    int currentTime = time(NULL);
    Session *sessions = (Session *) calloc(0, sizeof(Session));
    for (int i = 0; i < hallsCount; ++i) {
        Hall hall = halls[i];
        for (int j = 0; j < hall.sessionsCount; ++j) {
            Session session = hall.sessions[j];
            if (session.startTimestamp > currentTime && session.capacity > 0) {
                sessions = (Session *) realloc(sessions, ++count * sizeof(Session));
                sessions[count - 1] = session;
            }
        }
    }
    if (count == 0) {
        printf("There are currently no sessions.\n");
        return false;
    }
    //quickSort(sessions, 0, count - 1);
    for (int i = 0; i < count; ++i) {
        Session session = sessions[i];
        showMovie(i, getMovieById(session.movieId));
        char *startDate = timestampToDate(session.startTimestamp);
        char *finishDate = timestampToDate(session.finishTimestamp);
        printf("Hall: %s\n", session.hallTitle);
        printf("Start: %s\n", startDate);
        printf("Finish: %s\n", finishDate);
        printf("Tickets left: %d\n", session.capacity);
        printf("\n");
        free(startDate);
        free(finishDate);
    }
    free(sessions);
    return true;
}

bool buyTicket(int n) {
    if (n < 0)
        return false;
    int count = 0, currentTime = time(NULL);
    Session **sessions = (Session **) calloc(0, sizeof(Session *));
    for (int i = 0; i < hallsCount; ++i) {
        for (int j = 0; j < halls[i].sessionsCount; ++j) {
            if (halls[i].sessions[j].startTimestamp > currentTime && halls[i].sessions[j].capacity > 0) {
                sessions = (Session **) realloc(sessions, ++count * sizeof(Session *));
                sessions[count - 1] = &halls[i].sessions[j];
            }
        }
    }
    if (n >= count)
        return false;
    quickSortPtr(sessions, 0, count - 1);
    --(sessions[n]->capacity);
    return true;
}

void showOptions() {
    printf("1. Add session to a hall\n");
    printf("2. Buy ticket for a session\n");
    printf("0. Exit\n\n");
    printf("Select an option above: ");
}

int getOption() {
    int option;
    scanf("%d", &option);
    return option;
}

int showMenuAndGetOption() {
    showOptions();
    return getOption();
}

int showHallsAndGetOption() {
    printf("List of halls:\n\n");
    for (int i = 0; i < hallsCount; ++i) {
        printf("%d. %s\n", i + 1, halls[i].title);
        printf("Capacity: %d\n", halls[i].capacity);
        printf("Current sessions count: %d\n", halls[i].sessionsCount);
        printf("\n");
    }
    printf("Select a hall above (enter 0 to go to main menu): ");
    return getOption();
}

void printLine() {
    printf("_________________________________________\n");
}

void showHallSessions(int index) {
    if (halls[index].sessionsCount == 0) {
        printf("There are currently no sessions in %s.\n\n", halls[index].title);
        return;
    }
    printf("Current sessions of %s:\n\n", halls[index].title);
    for (int i = 0; i < halls[index].sessionsCount; ++i) {
        Session session = halls[index].sessions[i];
        char *startDate = timestampToDate(session.startTimestamp);
        char *finishDate = timestampToDate(session.finishTimestamp);
        printf("%d. %s\n", i + 1, getMovieById(session.movieId).title);
        printf("Start: %s\n", startDate);
        printf("Finish: %s\n", finishDate);
        printf("\n");
        free(startDate);
        free(finishDate);
    }
}

void loadData() {
    FILE *fp = fopen(databaseFilepath, "rb");
    if (!fp) {
        perror(databaseFilepath);
        exit(1);
    }
    long int lSize;
    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);
    char *buffer = (char*) calloc(1, lSize + 1);
    if (!buffer) {
        fclose(fp), fputs("memory allocation failed.", stderr);
        exit(1);
    }
    if (1 != fread(buffer, lSize, 1, fp)) {
        fclose(fp);
        free(buffer);
        fputs("file read failed.", stderr);
        exit(1);
    }
    json_settings settings = {};
    settings.value_extra = json_builder_extra;
    char error[128];
    json_value *json = json_parse_ex(&settings, buffer, strlen(buffer), error);
    json_object_entry hallsJson = json->u.object.values[0];
    hallsCount = (hallsJson.value)->u.array.length;
    halls = (Hall *) calloc(hallsCount, sizeof(Hall));
    for (int i = 0; i < hallsCount; ++i) {
        halls[i] = (Hall) {
                (((hallsJson.value)->u.array.values[i])->u.object.values[0].value)->u.string.ptr,
                (int) (((hallsJson.value)->u.array.values[i])->u.object.values[1].value)->u.integer,
                (int) (((hallsJson.value)->u.array.values[i])->u.object.values[2].value)->u.array.length,
                (Session *) calloc((((hallsJson.value)->u.array.values[i])->u.object.values[2].value)->u.array.length,
                                   sizeof(Session))
        };
        for (int j = 0; j < halls[i].sessionsCount; ++j) {
            halls[i].sessions[j] = (Session) {
                    (int) (((((hallsJson.value)->u.array.values[i])->u.object.values[2].value)->u.array.values[j])->u.object.values[0].value)->u.integer,
                    (int) (((((hallsJson.value)->u.array.values[i])->u.object.values[2].value)->u.array.values[j])->u.object.values[1].value)->u.integer,
                    (int) (((((hallsJson.value)->u.array.values[i])->u.object.values[2].value)->u.array.values[j])->u.object.values[2].value)->u.integer,
                    (int) (((((hallsJson.value)->u.array.values[i])->u.object.values[2].value)->u.array.values[j])->u.object.values[3].value)->u.integer,
                    (((((hallsJson.value)->u.array.values[i])->u.object.values[2].value)->u.array.values[j])->u.object.values[4].value)->u.string.ptr
            };
        }
    }
    json_object_entry moviesJson = json->u.object.values[1];
    moviesCount = (moviesJson.value)->u.array.length;
    movies = (Movie *) calloc(moviesCount, sizeof(Movie));
    for (int i = 0; i < moviesCount; ++i) {
        movies[i] = (Movie) {
                (int) (((moviesJson.value)->u.array.values[i])->u.object.values[0].value)->u.integer,
                (int) (((moviesJson.value)->u.array.values[i])->u.object.values[1].value)->u.integer,
                (int) (((moviesJson.value)->u.array.values[i])->u.object.values[5].value)->u.array.length,
                (((moviesJson.value)->u.array.values[i])->u.object.values[2].value)->u.string.ptr,
                (((moviesJson.value)->u.array.values[i])->u.object.values[3].value)->u.string.ptr,
                (((moviesJson.value)->u.array.values[i])->u.object.values[4].value)->u.string.ptr,
                (char **) calloc((((moviesJson.value)->u.array.values[i])->u.object.values[5].value)->u.array.length,
                                 sizeof(char *))
        };
        for (int j = 0; j < movies[i].castCount; ++j) {
            movies[i].cast[j] = ((((moviesJson.value)->u.array.values[i])->u.object.values[5].value)->u.array.values[j])->u.string.ptr;
        }
    }
    char *buf = (char*) malloc(json_measure(json));
    json_serialize(buf, json);
    fclose(fp);
    free(buffer);
    free(buf);
}

void saveData() {
    json_value *result = json_object_new(2);
    json_value *hallsJson = json_array_new(hallsCount);
    for (int i = 0; i < hallsCount; ++i) {
        json_value *hall = json_object_new(3);
        json_object_push(hall, "title", json_string_new(halls[i].title));
        json_object_push(hall, "capacity", json_integer_new(halls[i].capacity));
        json_value *sessionsJson = json_array_new(halls[i].sessionsCount);
        for (int j = 0; j < halls[i].sessionsCount; ++j) {
            json_value *sessionJson = json_object_new(5);
            json_object_push(sessionJson, "startTimestamp", json_integer_new(halls[i].sessions[j].startTimestamp));
            json_object_push(sessionJson, "finishTimestamp", json_integer_new(halls[i].sessions[j].finishTimestamp));
            json_object_push(sessionJson, "movieId", json_integer_new(halls[i].sessions[j].movieId));
            json_object_push(sessionJson, "capacity", json_integer_new(halls[i].sessions[j].capacity));
            json_object_push(sessionJson, "hallTitle", json_string_new(halls[i].sessions[j].hallTitle));
            json_array_push(sessionsJson, sessionJson);
        }
        json_object_push(hall, "sessions", sessionsJson);
        json_array_push(hallsJson, hall);
    }
    json_object_push(result, "halls", hallsJson);
    json_value *moviesJson = json_array_new(moviesCount);
    for (int i = 0; i < moviesCount; ++i) {
        json_value *movie = json_object_new(6);
        json_object_push(movie, "id", json_integer_new(movies[i].id));
        json_object_push(movie, "duration", json_integer_new(movies[i].duration));
        json_object_push(movie, "title", json_string_new(movies[i].title));
        json_object_push(movie, "genre", json_string_new(movies[i].genre));
        json_object_push(movie, "plot", json_string_new(movies[i].plot));
        json_value *castJson = json_array_new(movies[i].castCount);
        for (int j = 0; j < movies[i].castCount; ++j) {
            json_array_push(castJson, json_string_new(movies[i].cast[j]));
        }
        json_object_push(movie, "cast", castJson);
        json_array_push(moviesJson, movie);
    }
    json_object_push(result, "movies", moviesJson);
    char *json = (char*) malloc(json_measure(result));
    json_serialize(json, result);
    FILE *fp = fopen(databaseFilepath, "w+");
    fputs(json, fp);
    fclose(fp);
}

int main() {
    srand(time(NULL));
    loadData();
    int option;
    printf("Welcome to cinema manager!\n\n");
    while (true) {
        saveData();
        option = showMenuAndGetOption();
        switch (option) {
            case 1: {
                bool added = false;
                while (true) {
                    if (option == 0)
                        break;
                    printLine();
                    option = showHallsAndGetOption();
                    printLine();
                    if (option == 0)
                        break;
                    if (option > hallsCount || option < 0) {
                        printf("Invalid hall selected.\n\n");
                        continue;
                    }
                    showHallSessions(option - 1);
                    showMovies();
                    char startDate[80], finishDate[80];
                    int movieId, startTimestamp, finishTimestamp;
                    Movie movie;
                    printf("Enter movie ID: ");
                    while (true) {
                        scanf("%d", &movieId);
                        if (movieId <= 0 || movieId > moviesCount) {
                            printf("Invalid movie ID.\nEnter movie ID: ");
                            continue;
                        }
                        movie = movies[movieId - 1];
                        movieId = movie.id;
                        break;
                    }
                    struct tm tm;
                    time_t epoch;
                    gets(startDate);
                    while (true) {
                        printf("\nEnter start date (in format 2020-01-10 21:30:00): ");
                        while (true) {
                            gets(startDate);
                            if (strptime(startDate, (char*) "%Y-%m-%d %H:%M:%S", &tm) != 0) {
                                epoch = mktime(&tm);
                                startTimestamp = epoch + 16200;
                                break;
                            }
                            printf("Invalid date.\nEnter start date (in format 2020-01-10 21:30:00): ");
                        }
                        finishTimestamp = startTimestamp + (movie.duration * 60);
                        if (finishTimestamp <= startTimestamp) {
                            printf("%s\n", "Invalid dates are entered.");
                        } else if (!addSession(&halls[option - 1], movieId, startTimestamp, finishTimestamp)) {
                            printf("%s\n", "Entered date has conflict with another session.");
                        } else {
                            printf("Session successfully added!\n");
                            printLine();
                            added = true;
                            break;
                        }
                    }
                    if (added)
                        break;
                }
                if (option == 0 || added)
                    break;
                break;
            }
            case 2:
                printLine();
                if (!showSessions())
                    break;
                printf("Select a session above (enter 0 to go to main menu): ");
                while (true) {
                    scanf("%d", &option);
                    if (option == 0)
                        break;
                    if (!buyTicket(option - 1)) {
                        printf("Invalid session selected.\nSelect a session above (enter 0 to go to main menu): ");
                        continue;
                    }
                    printf("Ticket has been successfully bought!\n");
                    printLine();
                    break;
                }
                break;
            case 0:
                printf("Good bye!");
                return 0;
            default:
                printf("Invalid option.\n\n");
        }
    }
    return 0;
}
