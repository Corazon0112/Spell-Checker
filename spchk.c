#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h> // speed, I am speed
#include <ctype.h>
#include <dirent.h>

#define BUFSIZE 1
#define MAX_PATH_LEN 257

#define INITIAL_CAPACITY 10
#define MAX_WORD_LENGTH 100  // Maximum length of a word

int num_words = 0;
char** words = NULL;

// linked list node struct

struct Node {
    char word[46];
    int wordNum;
    int lineNum;
    char filename[255];
    int hyphen; // If hyphen == 1 then the word has a hypen
    struct Node *next;
};

int my_strcmp(const char *s1, const char *s2) {
    while (*s1 || *s2) { // Continue looping until both strings end
        // Skip over apostrophes and non-ASCII characters in s1
        while (*s1 && (*s1 == '\'' || !isascii((unsigned char)*s1))) 
            s1++;
        // Skip over apostrophes and non-ASCII characters in s2
        while (*s2 && (*s2 == '\'' || !isascii((unsigned char)*s2))) 
            s2++;

        // If we reach the end of both strings simultaneously, they are equal
        if (!*s1 && !*s2) 
            return 0; // Strings are equal

        // Compare the current characters of s1 and s2
        if (*s1 != *s2)
            return ((unsigned char)*s1 - (unsigned char)*s2);

        // Only increment pointers if not at the end of either string
        if (*s1) s1++;
        if (*s2) s2++;
    }

    // Handle cases where one string is a prefix of the other
    return ((unsigned char)*s1 - (unsigned char)*s2);
}

int my_NCSstrcmp(const char *s1, const char *s2) {
    while (*s1 || *s2) { // Continue until at least one string ends
        // Skip apostrophes and non-ASCII characters in s1
        while (*s1 && (*s1 == '\'' || !isascii(*s1)))
            s1++;
        // Skip apostrophes and non-ASCII characters in s2
        while (*s2 && (*s2 == '\'' || !isascii(*s2))) 
            s2++;

        // If both strings end simultaneously, they are equal up to this point
        if (!*s1 && !*s2)
            return 0;

        // Convert current characters to lowercase for case-insensitive comparison
        char c1 = tolower((unsigned char)*s1);
        char c2 = tolower((unsigned char)*s2);

        // If characters are different, return the difference
        if (c1 != c2)
            return c1 - c2;

        // Move to the next character if current characters are equal
        if (*s1) s1++;
        if (*s2) s2++;
    }

    // Handles the case where one string is a prefix of the other
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

int CSbinarySearch(int rows, char *word) {
    int low = 0;
    int high = rows - 1;

    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = my_strcmp(words[mid], word);
        if (cmp == 0) {
            return mid; // Found
        } else if (cmp < 0) {
            low = mid + 1; // Search in right half
        } else {
            high = mid - 1; // Search in left half
        }
    }

    return -1; // Not found
}

int NCSbinarySearch(int rows, char *word) {
    int low = 0;
    int high = rows - 1;

    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = my_NCSstrcmp(words[mid], word);
        printf ("strcmp: %d mid word: %s \n", cmp, words[mid]);
        printf("debug ASCII mid: %s, and word: %s, and wordword: %s \n", words[mid], word, word);
        printf("debug the word is between low: %d, and high: %d. \n", low, high);
        if (cmp == 0) {
            return mid; // Found
        } else if (cmp < 0) {
            low = mid + 1; // Search in right half
        } else {
            high = mid - 1; // Search in left half
        }
    }

    return -1; // Not found
}

int NCSseqSearch (int rows, char *word) {
    for (int i = 0; i< rows; i++){
        if(my_NCSstrcmp(words[i], word)==0){
            return i;
        }
    }
    return -1;
}

int ttl_upper(char a[46]){
    int i = 0; 
    int counter;
    char ch;
    ch = a[0];
 
    // counting of upper case
    while (ch != '\0') {
        ch = a[i];
        if (isupper(ch))
            counter++;
 
        i++;
    }
 
    // returning total number of upper case present in sentence
    return (counter);
}

// stores word in 2d global dictionary array and increments the number of words in it
void process_word(char *word) {
    
    strcpy(words[num_words], word);
    num_words++;
    
}

int ttl_lower(char a[46]){
    int i = 0; 
    int counter;
    char ch;
    ch = a[0];
 
    // counting of upper case
    while (ch != '\0') {
        ch = a[i];
        if (islower(ch))
            counter++;
        i++;
    }
 
    // returning total number of upper case present in sentence
    return (counter);
}

void capFL(char *str) {
    if (str == NULL || *str == '\0')
        return; // Handle empty string or NULL pointer
    *str = toupper(*str); // Capitalize the first character
}

// Dynamically allocating words from dict file into global dict array. 
void DYprepare(const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening file.\n");
        exit(1);
    }

    words = (char**)malloc(INITIAL_CAPACITY * sizeof(char*));
    if (!words) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    int capacity = INITIAL_CAPACITY;

    char line[MAX_WORD_LENGTH + 1]; // +1 for null terminator
    while (fgets(line, sizeof(line), fp) != NULL) {
        char *token = strtok(line, " \n");
        while (token != NULL) {
            if (num_words == capacity) {
                capacity *= 2;
                char** temp = (char**)realloc(words, capacity * sizeof(char*));
                if (!temp) {
                    printf("Memory reallocation failed.\n");
                    exit(1);
                }
                words = temp;
            }

            words[num_words++] = strdup(token);
            if (!words[num_words-1]) {
                printf("Memory allocation for word failed.\n");
                exit(1);
            }

            token = strtok(NULL, " \n");
        }
    }

    if (ferror(fp)) {
        printf("Error reading file.\n");
        exit(1);
    }

    fclose(fp);
}
//DEFUNCT used when we had a fixed array. now our global dict array is dynamically allocated
void prepare(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    char buf[BUFSIZE];
    ssize_t bytesRead;
    char *word = NULL;
    size_t wordCapacity = 0;
    size_t wordLength = 0;

    while ((bytesRead = read(fd, buf, BUFSIZE)) > 0) 
        for (int i = 0; i < bytesRead; i++) {
            if (buf[i] == '\n' || buf[i] == ' ' || buf[i] == '\t')
                if (wordLength > 0) {
                    word[wordLength] = '\0'; // Null-terminate the current word
                    process_word(word);
                    wordLength = 0; // Reset for the next word
                }
            else {
                if (wordLength + 1 >= wordCapacity) {
                    // Increase capacity: choose a strategy (e.g., doubling) to reduce the number of reallocations
                    size_t newCapacity = wordCapacity == 0 ? 2 : wordCapacity * 2;
                    char* temp = realloc(word, newCapacity * sizeof(char));
                    if (temp == NULL) {
                        perror("Error allocating memory");
                        close(fd);
                        free(word); // Free allocated memory before returning
                        return;
                    }
                    word = temp;
                    wordCapacity = newCapacity;
                }
                word[wordLength++] = buf[i];
            }
        }

    if (bytesRead == -1) {
        perror("Error reading file");
        close(fd);
        free(word); // Ensure to free word in case of read error
        return;
    }

    // Process the last word if it exists
    if (wordLength > 0) {
        word[wordLength] = '\0'; // Ensure null-termination
        process_word(word);
    }

    close(fd);
    free(word);
}
// Adds node to linked list
void append(struct Node **headRef, const char *word, int wordNum, int lineNum, const char *filename) {
    struct Node *newNode = malloc(sizeof(struct Node));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    strcpy(newNode->word, word);
    newNode->wordNum = wordNum;
    newNode->lineNum = lineNum;
    strcpy(newNode->filename, filename);
    newNode->next = NULL;

    if (*headRef == NULL)
        *headRef = newNode;
    else {
        struct Node *current = *headRef;
        while (current->next != NULL)
            current = current->next;
        current->next = newNode;
    }
}



int DOcheckL(char *word_to_search, struct Node *head) {
    int index = CSbinarySearch(num_words, word_to_search);
    if (index != -1)
        // Word found in case-sensitive search
        return 0;

    // Proceed with case-insensitive search if case-sensitive search fails
    int NCSindex = NCSseqSearch(num_words, word_to_search);
    if (NCSindex == -1)
        // Word not found in either search
        return -1;

    // At this point, word is found in NCS search. Now check for case variations.
    char *dictWord = words[NCSindex]; // Assuming words is accessible here

    // Compare directly without making extra copies
    if (strcasecmp(dictWord, word_to_search) == 0)
        // Word matches in case-insensitive comparison
        return 0;

    // No need to transform and compare as strcasecmp already covers case variations
    return -1;
}

void checkList(struct Node* head) {
    while (head != NULL) {
        char* hyphen_position = strchr(head->word, '-');
        head->hyphen = (hyphen_position != NULL);

        if (head->hyphen) {
            *hyphen_position = '\0'; // Temporarily terminate the first part
            int fCheck = DOcheckL(head->word, head); // Check the first part
            *hyphen_position = '-'; // Restore the hyphen to check the whole word if needed

            // Check the second part, offset by 1 to skip the hyphen
            int lCheck = DOcheckL(hyphen_position + 1, head);

            if (fCheck == -1 || lCheck == -1)
                printf("%s (%d,%d): %s\n", head->filename, head->lineNum, head->wordNum, head->word);
        } else {
            int Check = DOcheckL(head->word, head);
            if (Check == -1)
                printf("%s (%d,%d): %s\n", head->filename, head->lineNum, head->wordNum, head->word);
        }

        head = head->next;
    }
    printf("\n");
}

void clearSurround(char *word) {
    int len = strlen(word);
    if (len == 0) return;

    int start = 0, end = len - 1;
    int temp = len-1;

    // Find the first non-punct/non-digit character from the start
    while (start <= end && (ispunct(word[start]) || isdigit(word[start]))) 
        ++start;

    // Find the first non-punct/non-digit character from the end
    while (end >= start && (ispunct(word[end]) || isdigit(word[end]))) 
        --end;

    if (start > end)
        word[0] = '\0'; // Entire string is to be cleared
    else if (start > 0 || end < len - 1) {
        memmove(word, &word[start], end - start + 1);
        word[end - start + 1] = '\0';
    }
    
    // No action needed if start == 0 and end == len - 1 (string is already clear)
}

int comparePrepare(const char *filename) {
    addSpaceToEndOfFile(filename);
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return -1;
    }

    char word[46];
    int lineNum = 1;
    int wordNumber = 0;
    struct Node *head = NULL;

    char buf[BUFSIZE];
    ssize_t r;
    size_t wordLength = 0;

    
    while ((r = read(fd, buf, BUFSIZE)) > 0) {
        for (int i = 0; i < r; i++) {
            if (buf[i] != '\n' && buf[i] != ' ')
                word[wordLength++] = buf[i];
            else {
                if (wordLength > 0) {
                    word[wordLength] = '\0';
                    clearSurround(word);
                    if (strlen(word)!=0)
                        append(&head, word, wordNumber, lineNum, filename);
                    
                    wordLength = 0;
                }
            }

            if(buf[i] == '\n'){
                lineNum++;
                wordNumber = 0;
            }
            wordNumber++;
        }
    }

    if (wordLength > 0) {
        word[wordLength] = '\0';
        append(&head, word, wordNumber, lineNum, filename);
    }

    if (r == -1) {
        perror("Error reading file");
        return 1;
    }
    checkList(head);
    // Free the memory allocated for the linked list
    struct Node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }

    close(fd);     

    return 0;
}


void traverseDirectory(const char *dirPath, int dict) {
    DIR *dir = opendir(dirPath);
    if (!dir) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip the current and parent directory entries.
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        // Construct the full path of the found entry.
        char filePath[MAX_PATH_LEN];
        snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);

        // Decide on action based on the type of entry.
        if (entry->d_type == DT_DIR)
            // Recursive call if it's a directory.
            traverseDirectory(filePath, dict);
        else if (entry->d_type == DT_REG) {
            // Process regular files based on conditions.
            if (dict == 1) 
                DYprepare(filePath);
            else if (strstr(entry->d_name, ".txt") != NULL)
                comparePrepare(filePath);
        }
    }

    closedir(dir);
}

void processIndividualFile(const char *filePath, int dict) {
    if(dict == 1)
        DYprepare(filePath);
    else
        comparePrepare(filePath);
    
}
void addSpaceToEndOfFile(const char *filename) {
    FILE *fp = fopen(filename, "a"); // Open in append mode
    if (!fp) {
        perror("Error opening file to append space");
        exit(EXIT_FAILURE);
    }
    fputc(' ', fp); // Append a space to the file
    fclose(fp); // Close the file
}

// silly main function to test searcher
int main(int argc, char *argv[]) {
    clock_t start_time = clock();

    if (argc < 2) {
        printf("Not enough arguments!!!");
        return EXIT_FAILURE;
    }
   
    // ignore first argument because that's the file

    // first string will be the dictionary 
    
    for(int i = 1; i < argc; i++){
        const char *path = argv[i];

        DIR *dir = opendir(path);
        if (dir != NULL) {
            traverseDirectory(path, i);
            closedir(dir);
        } else {
            processIndividualFile(path, i);
        }
    }
        

    return 0;
}