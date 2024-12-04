#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// Constants
#define WORD_LENGTH 5
#define MAX_GUESSES 6
#define CELL_SIZE 50
#define MAX_WORDS 1000
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TIME_TRIAL_LIMIT 60

// Game States
typedef enum GameState {
    MENU,
    CLASSIC_MODE,
    ENDLESS_MODE,
    TIME_TRIAL_MODE
} GameState;

// Function Prototypes
void loadWords(const char *filename, char words[MAX_WORDS][WORD_LENGTH + 1], int *wordCount);
char* chooseRandomWord(const char words[MAX_WORDS][WORD_LENGTH + 1], int wordCount);
void displayWordleBoard(const char board[MAX_GUESSES][WORD_LENGTH + 1], const char feedback[MAX_GUESSES][WORD_LENGTH + 1], int currentRow);
int checkGuess(const char *guess, const char *targetWord, char feedback[WORD_LENGTH + 1]);
void playClassicMode(const char words[MAX_WORDS][WORD_LENGTH + 1], int wordCount, GameState *state);
void playEndlessMode(const char words[MAX_WORDS][WORD_LENGTH + 1], int wordCount, GameState *state);
void playTimeTrialMode(const char words[MAX_WORDS][WORD_LENGTH + 1], int wordCount, GameState *state);
void showMenu(GameState *state);
int validateWord(const char *guess, const char words[][WORD_LENGTH + 1], int wordCount);

// Main Function
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Wordle Game");
    SetTargetFPS(60);
    
    // Disable default ESC key exit behavior
    SetExitKey(KEY_NULL);

    char words[MAX_WORDS][WORD_LENGTH + 1];
    int wordCount = 0;
    loadWords("words.txt", words, &wordCount);

    GameState state = MENU;

    while (!WindowShouldClose()) {
        switch (state) {
            case MENU:
                showMenu(&state);
                break;
            case CLASSIC_MODE:
                playClassicMode(words, wordCount, &state);
                break;
            case ENDLESS_MODE:
                playEndlessMode(words, wordCount, &state);
                break;
            case TIME_TRIAL_MODE:
                playTimeTrialMode(words, wordCount, &state);
                break;
        }
    }

    CloseWindow();
    return 0;
}

// Load words from a file
void loadWords(const char *filename, char words[MAX_WORDS][WORD_LENGTH + 1], int *wordCount) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open %s\n", filename);
        exit(EXIT_FAILURE);
    }

    *wordCount = 0;
    while (fscanf(file, "%5s", words[*wordCount]) == 1 && *wordCount < MAX_WORDS) {
        (*wordCount)++;
    }
    fclose(file);
}

// Choose a random word from the list
char* chooseRandomWord(const char words[MAX_WORDS][WORD_LENGTH + 1], int wordCount) {
    srand(time(NULL));
    return words[rand() % wordCount];
}

// Validate the guessed word exists in the dictionary
int validateWord(const char *guess, const char words[][WORD_LENGTH + 1], int wordCount) {
    for (int i = 0; i < wordCount; i++) {
        if (strcmp(guess, words[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Compare guess and generate feedback
int checkGuess(const char *guess, const char *targetWord, char feedback[WORD_LENGTH + 1]) {
    int correct = 1;
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == targetWord[i]) {
            feedback[i] = 'G';
        } else if (strchr(targetWord, guess[i])) {
            feedback[i] = 'Y';
            correct = 0;
        } else {
            feedback[i] = 'X';
            correct = 0;
        }
    }
    feedback[WORD_LENGTH] = '\0';
    return correct;
}

// Display the Wordle board
void displayWordleBoard(const char board[MAX_GUESSES][WORD_LENGTH + 1], const char feedback[MAX_GUESSES][WORD_LENGTH + 1], int currentRow) {
    int gridX = (SCREEN_WIDTH - WORD_LENGTH * CELL_SIZE) / 2;
    int gridY = (SCREEN_HEIGHT - MAX_GUESSES * CELL_SIZE) / 2;

    for (int row = 0; row < MAX_GUESSES; row++) {
        for (int col = 0; col < WORD_LENGTH; col++) {
            int x = gridX + col * CELL_SIZE;
            int y = gridY + row * CELL_SIZE;

            Color rectColor = LIGHTGRAY;

            if (row < currentRow) {
                if (feedback[row][col] == 'G') rectColor = GREEN;
                else if (feedback[row][col] == 'Y') rectColor = YELLOW;
                else if (feedback[row][col] == 'X') rectColor = GRAY;
            }

            DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, rectColor);
            DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, BLACK);

            if (board[row][col] != '\0') {
                DrawText((char[]){board[row][col], '\0'}, x + 15, y + 10, 20, BLACK);
            }
        }
    }
}

// Play Classic Mode
void playClassicMode(const char words[MAX_WORDS][WORD_LENGTH + 1], int wordCount, GameState *state) {
    const char *targetWord = chooseRandomWord(words, wordCount);

    char board[MAX_GUESSES][WORD_LENGTH + 1] = {0};
    char feedback[MAX_GUESSES][WORD_LENGTH + 1] = {0};

    int currentRow = 0, currentCol = 0;
    bool gameWon = false;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            *state = MENU; // Return to menu
            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Wordle - Classic Mode (Press ESC to return to Menu)", SCREEN_WIDTH / 2 - MeasureText("Wordle - Classic Mode (Press ESC to return to Menu)", 20) / 2, 20, 20, BLACK);
        displayWordleBoard(board, feedback, currentRow);

        if (gameWon) {
            DrawText("You Win!", SCREEN_WIDTH / 2 - MeasureText("You Win!", 40) / 2, SCREEN_HEIGHT - 80, 40, GREEN);
        } else if (currentRow == MAX_GUESSES) {
            DrawText("Game Over!", SCREEN_WIDTH / 2 - MeasureText("Game Over!", 40) / 2, SCREEN_HEIGHT - 80, 40, RED);
            DrawText(TextFormat("Word: %s", targetWord), SCREEN_WIDTH / 2 - MeasureText(targetWord, 20) / 2, SCREEN_HEIGHT - 40, 20, DARKGRAY);
        }

        EndDrawing();

        if (gameWon || currentRow == MAX_GUESSES) {
            if (IsKeyPressed(KEY_ENTER)) break;
            continue;
        }

        if (IsKeyPressed(KEY_BACKSPACE) && currentCol > 0) {
            currentCol--;
            board[currentRow][currentCol] = '\0';
        }

        if (IsKeyPressed(KEY_ENTER) && currentCol == WORD_LENGTH) {
            if (!checkGuess(board[currentRow], targetWord, feedback[currentRow])) {
                currentRow++;
                currentCol = 0;
            } else {
                gameWon = true;
            }
        }

        for (int key = KEY_A; key <= KEY_Z; key++) {
            if (IsKeyPressed(key) && currentCol < WORD_LENGTH) {
                board[currentRow][currentCol] = (char)(key + ('a' - KEY_A));
                currentCol++;
            }
        }
    }
}

// Play Endless Mode
void playEndlessMode(const char words[MAX_WORDS][WORD_LENGTH + 1], int wordCount, GameState *state) {
    const char *targetWord = chooseRandomWord(words, wordCount);

    char board[MAX_GUESSES][WORD_LENGTH + 1] = {0};
    char feedback[MAX_GUESSES][WORD_LENGTH + 1] = {0};

    int currentRow = 0, currentCol = 0;
    bool paused = false;  // Flag for pausing after game end added
    int pauseFrames = 60; // Number of frames to wait before restarting (e.g., 120 frames = ~2 seconds) added
    bool gameWon = false;
    bool gameLost = false;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            *state = MENU; // Return to menu
            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Wordle - Endless Mode (Press ESC to return to Menu)", SCREEN_WIDTH / 2 - MeasureText("Wordle - Endless Mode (Press ESC to return to Menu)", 20) / 2, 20, 20, BLACK);
        displayWordleBoard(board, feedback, currentRow);

        if (gameWon) {
            DrawText("You Win!", SCREEN_WIDTH / 2 - MeasureText("You Win!", 40) / 2, SCREEN_HEIGHT - 80, 40, GREEN);
        } else if (gameLost) {
            DrawText(TextFormat("Game Over! The word was: %s", targetWord), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Game Over! The word was: %s", targetWord), 20) / 2, SCREEN_HEIGHT - 80, 20, RED);
        }

        EndDrawing();

        if (gameWon || gameLost) {
            if (!paused) {
                paused = true;  // Start the pause
            }

            if (pauseFrames > 0) {
                pauseFrames--;  // Decrease the pause duration
                continue;  // Skip the rest of the game loop for this frame
            }

            // After the pause is over, reset the game
            paused = false;  // Reset pause flag added
            pauseFrames = 60;  // Reset pauseFrames for the next round
            targetWord = chooseRandomWord(words, wordCount);
            memset(board, 0, sizeof(board));
            memset(feedback, 0, sizeof(feedback));
            currentRow = 0;
            currentCol = 0;
            gameWon = false;
            gameLost = false;
        }
        if(IsKeyPressed(KEY_ESCAPE)) {
                *state = MENU;
                break;
            }

        if (currentRow < MAX_GUESSES && !gameLost) {
            if (IsKeyPressed(KEY_BACKSPACE) && currentCol > 0) {
                currentCol--;
                board[currentRow][currentCol] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER) && currentCol == WORD_LENGTH) {
                if (!checkGuess(board[currentRow], targetWord, feedback[currentRow])) {
                    currentRow++;
                    currentCol = 0;
                } else {
                    gameWon = true;
                }
            }

            for (int key = KEY_A; key <= KEY_Z; key++) {
                if (IsKeyPressed(key) && currentCol < WORD_LENGTH) {
                    board[currentRow][currentCol] = (char)(key + ('a' - KEY_A));
                    currentCol++;
                }
            }
        }

        if (currentRow == MAX_GUESSES && !gameWon) {
            gameLost = true;
        }
    }
}

// Function to draw the time remaining on the screen
void drawTimeRemaining(int timeRemaining) {
    char timeText[20];
    sprintf(timeText, "Time Left: %d", timeRemaining);

    DrawText(timeText, SCREEN_WIDTH / 2 - MeasureText(timeText, 20) / 2, 60, 20, BLACK);
}

void playTimeTrialMode(const char words[MAX_WORDS][WORD_LENGTH + 1], int wordCount, GameState *state) {
    time_t startTime = time(NULL);
    const char *targetWord = chooseRandomWord(words, wordCount);

    char board[MAX_GUESSES][WORD_LENGTH + 1] = {0};
    char feedback[MAX_GUESSES][WORD_LENGTH + 1] = {0};

    int currentRow = 0, currentCol = 0;
    bool gameWon = false;
    bool gameLost = false;
    bool paused = false;
    int pauseFrames = 60;  // Duration to show the "Game Over" screen (1 second)

    static int totalWins = 0;
    static int totalLosses = 0;

    while (!WindowShouldClose()) {
        int timeRemaining = TIME_TRIAL_LIMIT - (int)difftime(time(NULL), startTime);
        if (timeRemaining <= 0) {
            timeRemaining = 0;
            paused = true;
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            *state = MENU;
            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the time remaining
        drawTimeRemaining(timeRemaining);
        DrawText("Wordle - Time Trial Mode (Press ESC to return to Menu)", SCREEN_WIDTH / 2 - MeasureText("Wordle - Time Trial Mode (Press ESC to return to Menu)", 20) / 2, 20, 20, BLACK);
        displayWordleBoard(board, feedback, currentRow);

        if (gameWon) {
            DrawText("You Win!", SCREEN_WIDTH / 2 - MeasureText("You Win!", 40) / 2, SCREEN_HEIGHT - 80, 40, GREEN);
        } else if (gameLost) {
            DrawText(TextFormat("Incorrect! The word was: %s", targetWord), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Game Over! The word was: %s", targetWord), 20) / 2, SCREEN_HEIGHT - 80, 20, RED);
        }

        EndDrawing();

        if (gameWon || gameLost) {
            if (!paused) {
                paused = true;
            }

            if (pauseFrames > 0) {
                pauseFrames--;
                continue;
            }

            paused = false;
            pauseFrames = 60;
            targetWord = chooseRandomWord(words, wordCount);
            memset(board, 0, sizeof(board));
            memset(feedback, 0, sizeof(feedback));
            currentRow = 0;
            currentCol = 0;
            gameWon = false;
            gameLost = false;
        }

        if (currentRow < MAX_GUESSES && !gameLost) {
            if (IsKeyPressed(KEY_BACKSPACE) && currentCol > 0) {
                currentCol--;
                board[currentRow][currentCol] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER) && currentCol == WORD_LENGTH) {
                if (!checkGuess(board[currentRow], targetWord, feedback[currentRow])) {
                    currentRow++;
                    currentCol = 0;
                } else {
                    gameWon = true;
                    totalWins++;
                }
            }

            for (int key = KEY_A; key <= KEY_Z; key++) {
                if (IsKeyPressed(key) && currentCol < WORD_LENGTH) {
                    board[currentRow][currentCol] = (char)(key + ('a' - KEY_A));
                    currentCol++;
                }
            }
        }

        if (currentRow == MAX_GUESSES && !gameWon) {
            gameLost = true;
            totalLosses++;
        }

        // Check if time's up and show the "Game Over" screen
        if (timeRemaining == 0) {
            while (pauseFrames > 0) {
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("Time's Up! Game Over!", SCREEN_WIDTH / 2 - MeasureText("Time's Up! Game Over!", 40) / 2, SCREEN_HEIGHT / 2 - 20, 40, RED);
                DrawText(TextFormat("Total Wins: %d", totalWins), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Total Wins: %d", totalWins), 20) / 2, SCREEN_HEIGHT / 2 + 30, 20, GREEN);
                DrawText(TextFormat("Total Losses: %d", totalLosses), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Total Losses: %d", totalLosses), 20) / 2, SCREEN_HEIGHT / 2 + 60, 20, RED);
                EndDrawing();
                pauseFrames--;
            }
            paused = false;
            pauseFrames = 60;
            *state = MENU;  // Transition back to menu after 1 second pause
            break;
        }
    }
}



// Show Menu
void showMenu(GameState *state) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText("Wordle Game", SCREEN_WIDTH / 2 - MeasureText("Wordle Game", 40) / 2, SCREEN_HEIGHT / 2 - 100, 40, BLACK);
    DrawText("Press C for Classic Mode", SCREEN_WIDTH / 2 - MeasureText("Press C for Classic Mode", 20) / 2, SCREEN_HEIGHT / 2 - 40, 20, BLACK);
    DrawText("Press E for Endless Mode", SCREEN_WIDTH / 2 - MeasureText("Press E for Endless Mode", 20) / 2, SCREEN_HEIGHT / 2, 20, BLACK);
    DrawText("Press T for Time Trial Mode", SCREEN_WIDTH / 2 - MeasureText("Press T for Time Trial Mode", 20) / 2, SCREEN_HEIGHT / 2 + 40, 20, BLACK);

    EndDrawing();

    if (IsKeyPressed(KEY_C)) *state = CLASSIC_MODE;
    if (IsKeyPressed(KEY_E)) *state = ENDLESS_MODE;
    if (IsKeyPressed(KEY_T)) *state = TIME_TRIAL_MODE;
}
