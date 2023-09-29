#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int total_guesses, letters, used_guesses, sorted_words;
char guess, *user_guess, *letters_used, **words;
bool *usedLetters, *excluded;
int letters, mindex, success = 0, max = 0;

void play();
void startGame();
int initFile();
void doCounts(char letter);
bool check_duplicates(char *word);

int main(int argc, char *argv[]) {
    letters = atoi(argv[1]);
    total_guesses = atoi(argv[2]);
    used_guesses = 0;

    usedLetters = (bool *) calloc(26, sizeof(bool));
    excluded = (bool *) calloc(sorted_words, sizeof(bool));
    user_guess = (char *) malloc((letters + 1) * sizeof(char ));
    letters_used = (char *) malloc(27 * sizeof(char));
    letters_used[0] = '\0';
    startGame();
    for (int i = 0; i < sorted_words; i++) {
        free(words[i]);
    }
    return 0;
}

void startGame() {
    for (int i = 0; i < letters; i++) {
        user_guess[i] = '_';}
    initFile();
    while (used_guesses < total_guesses) {
        play();
    }

}

void play()
{
    char buffer[128], text[128];
    while (true) {
        if (used_guesses == total_guesses) {
            printf("Answer is = Something lol\n");
            break;
        }
        printf("%s", user_guess);
        printf(" Enter letter: ");
        scanf("%c", &guess);
        guess = tolower(guess);
        int val = guess - 'a'; // a = 0
        if (usedLetters[val] == true) {
            printf("%c already used \n", guess);
            getchar();
        }
        else if (usedLetters[val] == false)
        {
            makeGuess(guess);
            if (success == letters)
            {
                printf("%s is correct!\n", user_guess);
                break;
            }
        }
        usedLetters[val] = true;
    }
}

int initFile() {
    FILE *fptr = fopen("dictionary.txt", "r");
    if (!fptr) {
        printf("Could not open file");
        return 1;
    }
    char buffer[128];
    int count = 0;
    fscanf(fptr, "%127s", buffer);
    char new_filename[128];
    sprintf(new_filename, "game_%d_%d.txt", letters, total_guesses);
    FILE *gameptr = fopen(new_filename, "w+");
    if (!gameptr) {
        printf("Could not create game file\n");
        fclose(fptr);
        return 1;
    }
    rewind(fptr);
    while (fscanf(fptr, "%127s", buffer) == 1) {
        int len = strlen(buffer);
        if (len == letters)
            if (check_duplicates(buffer)) {
                fprintf(gameptr, "%s\n", buffer);
                count++;
            }
    }
    sorted_words = count; // Sorted words = count of words in dictionary that contain n letters (there are 6887 words in dictionary of length 6)
    words = (char **) malloc(sorted_words * sizeof(char *)); // Words is all 6 letter words indexed from 0 - 6887

    for (int i = 0; i < sorted_words && fscanf(gameptr, "%127s", buffer) == 1; i++) {
        int len = strlen(buffer);
        words[i] = (char *) malloc((len + 1) * sizeof(char));
        if (words[i] != NULL) {
            strcpy(words[i], buffer); // words[0] = a 6 letter word starting with a
        } else {
            printf("Memory allocation error");
            exit(1);
        }
        // This is where I add words to array
    }
    fclose(gameptr);
    fclose(fptr);
    return true;
}

bool check_duplicates(char *word) {
    int len = strlen(word);
    for (int i = 0; i < len; i++) {
        for (int j = i + 1; j < len; j++) {
            if (word[i] == word[j])
                return false;
        }
    }
    return true; // No duplicate letters found
}

void doCounts(char letter) {
    int *counts = (int *) calloc(letters + 1, sizeof(int)); // Counts of each word in the word categories below
    // categories are e***** *e**** **e*** ***e** ****e* ******e ******
    // Find largest category
    for (int i = 0; i < sorted_words; i++) { // Start from 0 - 6887
        if (!excluded[i]) { // if not excluded
            bool found = false;
            for (int j = 0; j < letters; j++) { // Increment through word by letter index
                if (words[i][j] ==
                    letter) { // e.g word is yanked       i = 6803 (index of word in words array)        j = 1 (index of letter 'a' in word)
                    found = true;
                    counts[j]++; // e.g count of *a**** incremented
                    break;
                }
            }
            if (!found)
                counts[letters]++; // Increment count where 'a' found in word
        }
    }
    max = counts[0], mindex = 0;//start off assuming that the 1st element is the max
    for (int i = 0; i <= letters; i++)//now compare it with the rest of the array, updataing the max all along
    {
        if (counts[i] > max) {
            max = counts[i];
            mindex = i; // this tells me that the count with the most words is when 'a' isn't in the word
        }
    }
    if (mindex < letters){ // largest count is less than 6 (e.g 'a' was in the word)
        user_guess[mindex] = letter; // Change the guess letter
        success++;
    }
    for (int i = 0; i < sorted_words; i++) {
        if (!excluded[i]) {
            bool found = false;
            for (int j = 0; j < letters; j++) {
                if (words[i][j] == letter) {
                    found = true;
                    if (j != mindex) { // Word was found but the index of 'a' wasn't where the most were
                        excluded[i] = true; // Exclude it
                        break;
                    }
                }
            } // yanked
            if (!found && mindex != letters) // Word with a wasnt found and the max index isnt 6
                excluded[i] = true;
        }
    }


}