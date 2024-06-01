#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <time.h>

#define MAX_SONG_POOL 120
#define MAX_STRING_SIZE 75

// Data Structure for an individual song

typedef struct Song *songptr;
typedef struct Song
{
    char title[MAX_STRING_SIZE];
    char album[MAX_STRING_SIZE];
    char uri[MAX_STRING_SIZE];
    char id[MAX_STRING_SIZE];
    short int year;
    double duration;
} Song;

songptr song_pool[MAX_SONG_POOL];
int pool_insert_index = 0;

typedef struct PlaylistNode *node_ptr;
typedef struct PlaylistNode
{
    songptr song;
    node_ptr prev_song;
    node_ptr next_song;
} PlaylistNode;

node_ptr header_node = NULL, now_playing = NULL ;


void readFromCSV();
void free_all_memory();
bool delete_playlist();
void create_song_pool();
bool is_pool_empty();
bool does_playlist_exist();
void play_next_song();
void play_prev_song();
void show_song_details();
void main_menu();
void show_all_songs_raw();
void show_playlist();
void insert_into_playlist(int position);
void reverse_playlist();
void display_playlist();
void show_reversed_playlist();
void swap_songs(int position1, int position2);
void shuffle_playlist();
void pagewise_song_display(int step);
int song_selector();
Song *createSong(const char *title, const char *album, const short int year, const double duration, const char *uri, const char *id);
void create_playlist();
void add_to_playlist();
void user_song_input();
// Function prototype for is_song_in_playlist
int is_song_in_playlist(Song* song);

node_ptr getNodeAtIndex(int index);

//Function to check if the song pool is empty
bool is_pool_empty()// song pool is pointer to struct song
{
    return song_pool[0] == NULL;
}

//Function to check if playlist exists
bool does_playlist_exist()
{
    return !(header_node == NULL );
}


void play_next_song()
{
    if (does_playlist_exist() && now_playing != NULL)
    {
        if (now_playing->next_song != NULL)
            now_playing = now_playing->next_song;
        else
            printf("REACHED END OF PLAYLIST");
    }
    else
    {
        printf("NO SONG ADDED TO PLAYLIST");
    }
}

void play_prev_song()
{
    if (does_playlist_exist() && now_playing != NULL)
    {
        if (now_playing->prev_song != NULL)
            now_playing = now_playing->prev_song;
        else
            printf("REACHED START OF PLAYLIST");
    }
    else
    {
        printf("NO SONG ADDED TO PLAYLIST");
    }
}

void show_song_details()
{
    if (now_playing == NULL || now_playing->song == NULL)
    {
        printf("                       CREATE A PLAYLIST FIRST\n");
    }
    else
    {
        printf("\n----------------------------------------------------------------------\n");
        printf("                          DETAILED OVERVIEW\n");
        printf("----------------------------------------------------------------------\n");
        printf("\t   TITLE    |   %s\n", now_playing->song->title);
        printf("\t   ALBUM    |   %s\n", now_playing->song->album);
        printf("\t   YEAR     |   %d\n", now_playing->song->year);
        printf("\t  DURATION  |   %0.2lf minutes\n", now_playing->song->duration);
        printf("\t SPOTIFY URI |   %s\n", now_playing->song->uri);
        printf("    Paste the above URI in your browser to play the song on Spotify\n");
        printf("----------------------------------------------------------------------\n");
    }
    char leave[MAX_STRING_SIZE];
    printf("<<<<< Enter any input to return to MAIN MENU\n");
    if (scanf("%s", leave))
    {
        system("clear");
        return;
    } 
}

void main_menu()
{
    printf("\n----------------------------------------------------------------------\n");
    printf("                              MAIN MENU\n");
    printf("----------------------------------------------------------------------\n");

    if (now_playing == NULL || now_playing->song == NULL)
    {
        printf("                          NO TRACK SELECTED\n");
    }
    else
    {
        printf("    NOW    |   %s\n", now_playing->song->title);
        printf("  PLAYING  |   %0.2lf minutes\n", now_playing->song->duration);
    }
    printf("----------------------------------------------------------------------\n");
    printf("   #   |  Action   \n");
    printf("----------------------------------------------------------------------\n");
    printf("   1   |  Create a new song\n");
    printf("   2   |  Display all available songs\n");
    if (does_playlist_exist())
        printf("   3   |  Delete playlist\n");
    else
        printf("   3   |  Create a new playlist\n");
    printf("   4   |  Add a song to the playlist\n");
    printf("   5   |  Show playlist\n");
    printf("   6   |  Play previous track\n");
    printf("   7   |  Play next track\n");
    printf("   8   |  Insert song into the queue\n");
    printf("   9   |  Reverse playlist\n");
    printf("   10  |  Swap songs in playlist\n");
    printf("   11  |  Shuffle songs in playlist\n");
    printf("   12  |  Show song details\n");
    printf("   13  |  Play From Queue\n");
    printf("  -1   |  Exit music player\n");
    printf("----------------------------------------------------------------------\n");
    printf("                        Enter your choice below\n");
}

void show_all_songs_raw()
{
    for (int i = 0; i < MAX_SONG_POOL && song_pool[i] != NULL; i++)
        printf("%-1d %-3s %-3s %-3d %0.2lfmin\n", (i + 1), song_pool[i]->title, song_pool[i]->album, song_pool[i]->year, song_pool[i]->duration);
}

void show_playlist()
{
    if (does_playlist_exist() && now_playing != NULL && now_playing->song != NULL)
    {
        printf("\n----------------------------------------------------------------------\n");
        printf("                          YOUR PLAYLIST\n");
        printf("----------------------------------------------------------------------\n");
        printf(" Title                                            | Duration\n");
        printf("----------------------------------------------------------------------\n");

        node_ptr current;

   
            current = header_node;
   
        while (current != NULL && current->song != NULL)
        {
            printf(" %-48.48s | %2.2lf min\n", current->song->title, current->song->duration);

           
                current = current->next_song;
           
           
        }
    }
    else
    {
        printf("\nYou haven't created a playlist yet or no song is selected.\n");
    }

    // Additional information about now_playing
    if (now_playing != NULL && now_playing->song != NULL)
    {
        printf("\nNow Playing: %s | Duration: %2.2lf min\n", now_playing->song->title, now_playing->song->duration);
    }
    else
    {
        printf("\nNo track selected.\n");
    }
}


// Function to insert a song into the playlist at a specific position
void insert_into_playlist(int position)
{
    if (position < 1 || position > pool_insert_index + 1)
    {
        printf("Invalid position. Please enter a position between 1 and %d.\n", pool_insert_index + 1);
        return;
    }

    node_ptr new_node = (node_ptr)malloc(sizeof(PlaylistNode));
    printf("\nPick the song you want to add\n");
    int song_number = song_selector();

    if (song_number > 0 && song_number <= pool_insert_index)
    {
        new_node->song = song_pool[song_number - 1];

        if (position == 1)
        {
            // Insert at the beginning
            new_node->next_song = header_node;
            new_node->prev_song = NULL;
            if (header_node != NULL)
            {
                header_node->prev_song = new_node;
            }
            header_node = new_node;
        }
        else
        {
            // Insert at the specified position
            node_ptr current = header_node;
            for (int i = 1; i < position - 1; i++)
            {
                current = current->next_song;
            }

            new_node->next_song = current->next_song;
            new_node->prev_song = current;

            if (current->next_song != NULL)
            {
                current->next_song->prev_song = new_node;
            }

            current->next_song = new_node;
        }

        system("clear");
        printf("%s has been added to your playlist at position %d.\n", song_pool[song_number - 1]->title, position);
    }
    else if (song_number == 0)
    {
        system("clear");
    }
    else
    {
        printf("\nThere was a problem while handling your request.\n");
    }
}


// Function to reverse the playlist
void reverse_playlist() {
    if (does_playlist_exist()) {
        node_ptr current = header_node;
        node_ptr temp = NULL;

        // Traverse to the end of the playlist
        while (current->next_song != NULL) {
            current = current->next_song;
        }

        // Save the reference to the initial header node
        node_ptr twinheader_node = header_node;

        // Update the header_node to point to the last node in the original playlist
        header_node = current;

        // Reverse the playlist
        while (current != NULL) {
            temp = current->prev_song;
            current->prev_song = current->next_song;
            current->next_song = temp;
            current = temp; // Fix: Move to the next node in the original order
        }

        // Update the now_playing pointer to the new position (new head of the reversed playlist)
        now_playing = header_node;
    } else {
        printf("\nYou haven't created a playlist yet.\n");
        return;
    }
}


 
 
void display_playlist() {
    if (!does_playlist_exist()) {
        printf("\nYou haven't created a playlist yet.\n");
        return;
    }

    node_ptr current = now_playing;  // Start from the new head

    if (current == NULL) {
        printf("\nPlaylist is empty.\n");
        return;
    }

    printf("\n------------------------------------------------------------------\n");
    printf("                          YOUR PLAYLIST\n");
    printf("------------------------------------------------------------------\n");
    printf(" Title                                            | Duration\n");
    printf("------------------------------------------------------------------\n");

    // Traverse the reversed playlist and print
    while (current != NULL) {
        if (current->song != NULL) {
            printf(" %-48.48s | %2.2lf min\n", current->song->title, current->song->duration);
        } else {
            printf(" [Unknown Title] | [Unknown Duration]\n");
        }
        current = current->prev_song;  // Move to the previous node in the reversed list
    }
}

   


//Function to play from any position in queue

void playFromQueue(struct PlaylistNode* header_node, int position) {
    if (position < 1) {
        printf("Invalid position. Queue positions start at 1.\n");
        return;
    }

    node_ptr current = header_node;
    int currentPosition = 0;
    while (current != NULL) {
        currentPosition++;
        if (currentPosition == position) {
            now_playing = current;  // Update now_playing pointer
            printf("Now playing: %s\n", current->song->title);
            return;
        }
        current = current->next_song;
    }

    if (position > currentPosition) {
        printf("Position out of range. The playlist has only %d songs.\n", currentPosition);
    }
}



// Function to swap two songs in the playlist
void swap_songs(int position1, int position2)
{
    if (!does_playlist_exist()) {
        printf("Cannot swap songs. Playlist does not exist.\n");
        return;
    }

    if (position1 < 1 || position1 > pool_insert_index || position2 < 1 || position2 > pool_insert_index) {
        printf("Invalid positions. Please enter valid positions between 1 and %d.\n", pool_insert_index);
        return;
    }

    node_ptr current1 = header_node;
    node_ptr current2 = header_node;

    // Move to the node at position1
    for (int i = 1; i < position1 && current1 != NULL; i++) {
        current1 = current1->next_song;
    }

    // Move to the node at position2
    for (int i = 1; i < position2 && current2 != NULL; i++) {
        current2 = current2->next_song;
    }

    // Check if current1 or current2 is NULL before accessing their prev_song and next_song pointers
    if (current1 == NULL || current2 == NULL) {
        printf("Error: One of the nodes is NULL.\n");
        return;
    }

    printf("Before swap logic\n");

    // Swap the positions of the nodes
    printf("Before swap: %s, %s\n", current1->song->title, current2->song->title);

    if (current1->prev_song != NULL) {
        current1->prev_song->next_song = current2;
    } else {
        header_node = current2;
    }

    if (current2->prev_song != NULL) {
        current2->prev_song->next_song = current1;
    } else {
        header_node = current1;
    }

    if (current1->next_song != NULL) {
        current1->next_song->prev_song = current2;
    }

    if (current2->next_song != NULL) {
        current2->next_song->prev_song = current1;
    }

    node_ptr temp = current1->prev_song;
    current1->prev_song = current2->prev_song;
    current2->prev_song = temp;

    temp = current1->next_song;
    current1->next_song = current2->next_song;
    current2->next_song = temp;

    printf("After swap: %s, %s\n", current1->song->title, current2->song->title);

    printf("After swap logic\n");

    system("clear");
    printf("Songs at positions %d and %d have been swapped.\n", position1, position2);
}





// Declare the getNodeAtIndex function
node_ptr getNodeAtIndex(int index);

// Declare the getNodeAtIndex function
node_ptr getNodeAtIndex(int index);

void shuffle_playlist()
{
    if (does_playlist_exist())
    {
        int num_songs = 0;
        node_ptr current = header_node;

        // Count the number of songs in the playlist
        while (current != NULL)
        {
            num_songs++;
            current = current->next_song;
        }

        // Use Fisher-Yates shuffle algorithm
        srand((unsigned int)time(NULL));

        for (int i = num_songs - 1; i > 0; i--)
        {
            int j = rand() % (i + 1);

            // Swap data (assuming a song struct within the node)
            node_ptr node_i = getNodeAtIndex(i);
            node_ptr node_j = getNodeAtIndex(j);

            if (node_i != NULL && node_j != NULL)
            {
                // Swap data, not pointers
                Song temp_song = *(node_i->song);
                *(node_i->song) = *(node_j->song);
                *(node_j->song) = temp_song;
            }
        }

        // Reset now_playing to the beginning
        now_playing = header_node;
    }
    else
    {
        printf("\nYou haven't created a playlist yet.\n");
    }
}

// Helper function to get the node at a specific index
node_ptr getNodeAtIndex(int index)
{
    node_ptr current = header_node;
    for (int k = 0; k < index && current != NULL; k++)
    {
        current = current->next_song;
    }
    return current;
}


void pagewise_song_display(int step)
{
    printf("\n----------------------------------------------------------------------\n");
    printf("                             SONGS LIST\n");
    printf("----------------------------------------------------------------------\n");
    printf("   #   |  Title                                            | Duration\n");
    printf("----------------------------------------------------------------------\n");
    for (int i = step - 10; i < step && song_pool[i] != NULL; i++)
    {
        printf("  %2d   |  %-48.48s | %2.2lf min\n", (i + 1), song_pool[i]->title, song_pool[i]->duration);
    }
    printf("----------------------------------------------------------------------\n");
    printf("[Enter -2 to go to prev page] | Page %d | [Enter -1 to go to next page]\n", ((int)step / 10));
    printf("----------------------------------------------------------------------\n");
    printf(" <<<< Enter 0 to go back to main menu.\n");

    printf("----------------------------------------------------------------------\n");
    printf("                        Enter your choice below\n");
}

int song_selector()
{
    int song_number = -1;
    int step = 10;

    while (song_number < 0 || song_number > pool_insert_index)
    {
        pagewise_song_display(step);
        char input[MAX_STRING_SIZE];
        scanf("%s", input);

        if (sscanf(input, "%d", &song_number) != 1)
        {
            system("clear");
            printf("You seem to have entered an invalid input.\nReturning to the main menu.\n");
            return 0;
        }

        if (song_number == -2)
        {
            if (step > 10)
            {
                step -= 10;
                system("clear");
            }
            else
            {
                system("clear");
                printf("\nThis is the first page.\n");
            }
        }
        else if (song_number == -1)
        {
            if (step < ceil(pool_insert_index / 10.0) * 10)
            {
                step += 10;
                system("clear");
            }
            else
            {
                system("clear");
                printf("\nThis is the last page.\n");
            }
        }
        else if (song_number < -2 || song_number > pool_insert_index)
        {
            system("clear");
            printf("\nEnter a valid choice.\n");
        }
    }

    return song_number;
}

Song *createSong(const char *title, const char *album, const short int year, const double duration, const char *uri, const char *id)
{
    if (pool_insert_index >= MAX_SONG_POOL)
    {
        printf("Cannot add more songs.\n");
        return NULL;
    }

    Song *temp = malloc(sizeof(Song));
    if (temp == NULL)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    strcpy(temp->title, title);
    strcpy(temp->album, album);
    strcpy(temp->uri, uri);
    strcpy(temp->id, id);
    temp->year = year;
    temp->duration = duration;
    song_pool[pool_insert_index] = temp;  // Update song_pool with the new song
    pool_insert_index++;  // Increment after adding the song
    return temp;
}

void create_playlist() {
    int song_number;
    char choice;

    do {
        node_ptr temp = (node_ptr)malloc(sizeof(PlaylistNode));
        printf("\nPick the song you want to add to your new playlist (0 to stop adding)\n");
        song_number = song_selector();

        if (song_number > 0 && song_number <= 100) {
            // Check if the song is already in the playlist
            if (is_song_in_playlist(song_pool[song_number - 1])) {
                system("clear");
                printf("---\n%s is already in your playlist. Cannot add duplicate songs.\n---", song_pool[song_number - 1]->title);
                continue; // Skip adding the duplicate song
            }

            temp->song = song_pool[song_number - 1];
            temp->prev_song = NULL;
            temp->next_song = NULL;

            if (header_node == NULL) {
                header_node = temp;
                now_playing = temp;  // Update now_playing after adding the first song
            } else {
                // Add the new node to the end of the playlist
                node_ptr current = header_node;
                while (current->next_song != NULL) {
                    current = current->next_song;
                }
                current->next_song = temp;
                temp->prev_song = current;
            }

            system("clear");
            printf("---\n%s has been added to your new playlist.\n---", song_pool[song_number - 1]->title);
        } else if (song_number == 0) {
            system("clear");
            break; // Exit the loop if the user chooses to stop adding songs
        } else {
            printf("\nThere was a problem while handling your request, try again.\n");
        }

        // Clear the newline character from the input buffer
        while ((getchar()) != '\n');

        printf("\nDo you want to add another song? (y/n): ");
        scanf("%c", &choice);

    } while (choice == 'y' || choice == 'Y'); // Removed the extra }

}

// Function to check if a song is already in the playlist
int is_song_in_playlist(Song* song) {
    node_ptr current = header_node;

    while (current != NULL) {
        if (current->song == song) {
            return 1; // Song is in the playlist
        }
        current = current->next_song;
    }

    return 0; // Song is not in the playlist
}

//Prompts user to pick a song from the pool and add it to the existing playlist
void add_to_playlist()
{
    int song_number = 0;
    int step = 10;
    printf("\nPick the song you want to add\n");
    song_number = song_selector();
    if (song_number > 0 && song_number <= pool_insert_index)
    {
        node_ptr new_node = (node_ptr)malloc(sizeof(PlaylistNode));
        node_ptr last = header_node;
        new_node->song = song_pool[song_number - 1];
        new_node->next_song = NULL;

        /* 4. If the Linked List is empty, then make the new
          node as head */
        if (header_node == NULL)
        {
            new_node->prev_song = NULL;
            header_node = new_node;
            now_playing = new_node;  // Update now_playing after adding the first song
        }
        else
        {
            //Traverse till the last node
            while (last->next_song != NULL)
                last = last->next_song;

            last->next_song = new_node;
            new_node->prev_song = last;
        }
        system("clear");
        printf("%s has been added to your playlist.\n", song_pool[song_number - 1]->title);
    }
    else if (song_number == 0)
    {
        system("clear");
    }
    else
    {
        printf("\nThere was a problem while handling your request.\n");
    }
}

/** CSV Parser that maps a data set of upto songs onto the song pool
    Songs must be present in the root directory at rawdata.csv
*/
void readFromCSV()
{
    FILE *file = fopen("rawdata.csv", "r");
    char line[1000];
    int i = 0;

    if (file == NULL)
    {
        perror("******\nUnable to load songs from data source");
        printf("Please check if rawdata.csv exists at the root directory of the application.\n");
        printf("You can still enter songs manually.\n******\n");
        printf("Error Message");
    }
    else
    {
        char songtitle[MAX_STRING_SIZE];
        char releaseDate[MAX_STRING_SIZE];
        int durationms;
        double duration = 0.0;
        char id[MAX_STRING_SIZE];
        char album[MAX_STRING_SIZE];
        char uri[MAX_STRING_SIZE];
        int year;
        while (fgets(line, sizeof(line), file) && i < MAX_SONG_POOL)
        {
            char *token;
            token = strtok(line, ","); //tokenizes current line with delimiter ',' and returns the first substring
            if (token == NULL)
                continue;
            strcpy(songtitle, token);
            token = strtok(NULL, ","); // get the next token
            strncpy(releaseDate, token + 1, 4);
            sscanf(releaseDate, "%d", &year);
            token = strtok(NULL, ",");
            char temp[MAX_STRING_SIZE];
            strncpy(temp, token + 1, 6);
            sscanf(temp, "%d", &durationms);
            duration = durationms / 60000.0;
            token = strtok(NULL, ","); // get the next token
            strcpy(id, token);
            token = strtok(NULL, ","); // get the next token
            strcpy(album, token);
            token = strtok(NULL, ","); // get the next token
            strcpy(uri, token);
            token = strtok(NULL, ","); // get the next token
            //printf("Song read : %s  %s\n",songname,durationm);
            if (i == 0)
            {
                i++;
                continue;
            }
            song_pool[i - 1] = createSong(songtitle, album, year, duration, uri, id);
            i++;
        }
    }

    //printf("%d Songs added.\n\n", i);
    fclose(file);
}

//Function that allows the user to create a song of their own
void user_song_input()
{
    int add_another = 1;
    char songname[MAX_STRING_SIZE];
    int year;
    int durationms;
    char id[MAX_STRING_SIZE];
    char album[MAX_STRING_SIZE];
    char uri[MAX_STRING_SIZE] = "spotify:track:";
    double duration = 0.0;

    while (add_another && pool_insert_index < MAX_SONG_POOL)
    {
        system("clear");
        printf("\nEnter the details of the song you want to create.\n[ Don't give spaces between each word, use _ instead\n");

        printf("Title: ");
        scanf("%s", songname);
        printf("Album Name: ");
        scanf("%s", album);
        printf("Year of release: ");
        if (!(scanf("%d", &year)))
        {
            printf("INVALID INPUT ENTERED\n");
            sleep(1);
            break;
        }
        printf("Duration(in s): ");
        if (!(scanf("%d", &durationms)))
        {
            printf("INVALID INPUT ENTERED\n");
            sleep(1);
            break;
        }
        duration = durationms / 60;
        printf("Song ID: ");
        scanf("%s", id);
        strcat(uri, id);
        song_pool[pool_insert_index] = createSong(songname, album, year, duration, uri, id);
        printf("\nThe song %s has been created.\n\n", songname);
        printf("\nDo you want to create another song?\n(Enter 1 for yes and 0 for no) : ");
        scanf("%d", &add_another);
    }
    system("clear");
}

//Helper function to delete the playlist created and free all resources taken up by PlaylistNodes
bool delete_playlist()
{
    if (header_node != NULL)
    {
        node_ptr current = header_node;
        while (current != NULL)
        {
            node_ptr next = current->next_song;
            free(current);
            current = next;
        }
        header_node = NULL;
        now_playing = NULL;
        return true;
    }
    return false;
}

//Frees resources that had been dynamically allocated
void free_all_memory()
{
    delete_playlist();
    for (int i = 0; i < MAX_SONG_POOL && song_pool[i] != NULL; i++)
        free(song_pool[i]);
    printf("\n---END---");

    //Generate a 2 second delay
    sleep(2);
}

int main()
{
    int userChoice = 0;
    char *terminate = "X";
    int wrong_choice_count = 0;
    system("clear");
    readFromCSV();
    while (userChoice != -1)
    {
        switch (userChoice)
        {
        case 0:
        { //Show menu options
            main_menu();
            break;
        }
        case 1:
        {
            system("clear");
            if (pool_insert_index >= MAX_SONG_POOL)
            {
                printf("Cannot add more songs.\n");
            }

            user_song_input();
            break;
        }
        case 2:
        {
            system("clear");
            //show_all_songs();
            int user_song_selection = song_selector();
            system("clear");
            break;
        }
        case 3:
        {
            system("clear");
            if (does_playlist_exist())
            {
                printf("---");
                if (delete_playlist())
                    printf("\nThe playlist was successfully deleted.\n");
                else
                    printf("\nYou haven't created a playlist yet. Nothing to delete\n");

                printf("---");
            }
            else
                create_playlist();
            break;
        }
        case 4:
        {
            system("clear");
            add_to_playlist();
            break;
        }
        case 5:
        {
            system("clear");
           // if (twinheader_node != NULL && twinheader_node->next_song == NULL)
           // {
                printf("---");
                 
             //    show_reversed_playlist();
           // }
          // else
          // {
                show_playlist();
          // }
           break;
       }

        case 6:
        {
            system("clear");
            play_prev_song();
            break;
        }
        case 7:
        {
            system("clear");
            play_next_song();
            break;
        }
       
        case 8:
            {
                system("clear");
                int insert_position;
                printf("Enter the position to insert the song: ");
                if (scanf("%d", &insert_position))
                {
                    insert_into_playlist(insert_position);
                }
                else
                {
                    system("clear");
                    printf("Invalid input for position. Please enter a number.\n");
                }
                break;
            }

            case 9:
            {
                system("clear");
                reverse_playlist();
                printf("Playlist reversed.\n");
                break;
            }

            case 10:
            {
                system("clear");
                int swap_position1, swap_position2;
                printf("Enter the positions of the songs to swap: ");
                if (scanf("%d %d", &swap_position1, &swap_position2))
                {
                    swap_songs(swap_position1, swap_position2);
                }
                else
                {
                    system("clear");
                    printf("Invalid input for positions. Please enter two numbers.\n");
                }
                break;
            }

            case 11:
            {
                system("clear");
                shuffle_playlist();
                printf("Playlist shuffled.\n");
                break;
            }
           
            case 12:
           {
               system("clear");
               show_song_details();
               break;
           }
        
          case 13:
    {
        system("clear");
        int position_to_play;
        printf("Enter the position of the song to play: ");
        scanf("%d", &position_to_play);

        playFromQueue(header_node, position_to_play);
        break;
    }

        
        default:
        {
            if (wrong_choice_count == 3)
            {
                system("clear");
                printf("Please enter a valid option or the program will terminate.\n");
            }
            if (wrong_choice_count > 3)
            {
                system("clear");
                printf("Sorry you have exceeded the maximum number of retries, terminating..");
                free_all_memory();
                exit(1);
            }
            else
            {
                system("clear");
                printf("Please enter a valid option from the main menu below\n");
            }
            wrong_choice_count++;
            break;
        }
        }
        if (userChoice != 0)
        {
            main_menu();
        }

        char input[MAX_STRING_SIZE];
        scanf("%s", input);

        if (!sscanf(input, "%d", &userChoice))
        {
            system("clear");
            printf("----\nINVALID INPUT\n");
            userChoice = 0;
            wrong_choice_count++;
            if (wrong_choice_count > 3)
            {
                printf("Sorry you have exceeded the maximum number of retries, terminating..");
                free_all_memory();
                exit(1);
            }
            continue;
        }
    }
    free_all_memory();

    return 0;
}
