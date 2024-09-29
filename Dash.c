#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

#define WIDTH 20
#define HEIGHT 10
#define PLAYER_X 5
#define MAX_PILLARS 3
#define MAX_FLOATING_PILLARS 2 // Number of floating pillars
#define MAX_DIAGONAL_PILLARS 2 // Number of diagonal pillars
#define MIN_PILLAR_LENGTH 1    // Minimum length of pillars
#define MAX_PILLAR_LENGTH 5    // Maximum length of pillars

typedef struct {
    int y; // Player's vertical position
} Player;

typedef struct {
    int x;        // Pillar's horizontal position
    int y;        // Pillar's vertical position
    int direction; // 0 for top, 1 for bottom
    int length;   // Length of the pillar
} Pillar;

typedef struct {
    int x;        // Floating pillar's horizontal position
    int y;        // Floating pillar's vertical position
    int length;   // Length of the pillar
    int velocity; // Vertical movement speed (up or down)
} FloatingPillar;

typedef struct {
    int x;        // Diagonal pillar's horizontal position
    int y;        // Diagonal pillar's vertical position
    int length;   // Length of the pillar
    int dx;       // Horizontal movement speed
    int dy;       // Vertical movement speed
} DiagonalPillar;

void drawGame(Player *player, Pillar pillars[], FloatingPillar floatingPillars[], DiagonalPillar diagonalPillars[], int score) {
    system("CLS");

    // Top border
    printf("+");
    for (int i = 0; i < WIDTH; i++) printf("--");
    printf("+\n");

    // Draw the game space
    for (int i = 0; i < HEIGHT; i++) {
        printf("|");
        for (int j = 0; j < WIDTH; j++) {
            if (j == PLAYER_X && i == player->y) {
                printf("{}"); // Player
            } else {
                int pillarDrawn = 0;
                for (int p = 0; p < MAX_PILLARS; p++) {
                    if (j == pillars[p].x && i >= pillars[p].y && i < pillars[p].y + pillars[p].length) {
                        printf("[]"); // Fixed Pillar
                        pillarDrawn = 1;
                        break;
                    }
                }

                // Draw floating pillars after score reaches 30
                if (score >= 30) {
                    for (int p = 0; p < MAX_FLOATING_PILLARS; p++) {
                        if (j == floatingPillars[p].x && i >= floatingPillars[p].y && i < floatingPillars[p].y + floatingPillars[p].length) {
                            printf("##"); // Floating Pillar
                            pillarDrawn = 1;
                            break;
                        }
                    }
                }

                // Draw diagonal pillars after score reaches 50
                if (score >= 50) {
                    for (int p = 0; p < MAX_DIAGONAL_PILLARS; p++) {
                        if (j == diagonalPillars[p].x && i >= diagonalPillars[p].y && i < diagonalPillars[p].y + diagonalPillars[p].length) {
                            printf(diagonalPillars[p].dx > 0 ? "//" : "\\\\"); // Diagonal Pillar
                            pillarDrawn = 1;
                            break;
                        }
                    }
                }
                
                if (!pillarDrawn) {
                    printf("  "); // Empty space
                }
            }
        }
        printf("|\n");
    }

    // Bottom border
    printf("+");
    for (int i = 0; i < WIDTH; i++) printf("--");
    printf("+\n");

    printf("Score: %d\n", score);
}

int checkCollision(Player *player, Pillar pillars[], FloatingPillar floatingPillars[], DiagonalPillar diagonalPillars[], int score) {
    // Check collision with fixed pillars
    for (int i = 0; i < MAX_PILLARS; i++) {
        if (pillars[i].x == PLAYER_X && pillars[i].y <= player->y && player->y < pillars[i].y + pillars[i].length) {
            return 1; // Hits any fixed pillar
        }
    }

    // Check collision with floating pillars only after score reaches 30
    if (score >= 30) {
        for (int i = 0; i < MAX_FLOATING_PILLARS; i++) {
            if (floatingPillars[i].x == PLAYER_X && floatingPillars[i].y <= player->y && player->y < floatingPillars[i].y + floatingPillars[i].length) {
                return 1; // Hits any floating pillar
            }
        }
    }

    // Check collision with diagonal pillars only after score reaches 50
    if (score >= 50) {
        for (int i = 0; i < MAX_DIAGONAL_PILLARS; i++) {
            if (diagonalPillars[i].x == PLAYER_X && diagonalPillars[i].y <= player->y && player->y < diagonalPillars[i].y + diagonalPillars[i].length) {
                return 1; // Hits any diagonal pillar
            }
        }
    }
    
    return 0;
}

void movePillars(Pillar pillars[], FloatingPillar floatingPillars[], DiagonalPillar diagonalPillars[], int *score) {
    // Move fixed pillars
    for (int i = 0; i < MAX_PILLARS; i++) {
        pillars[i].x--;

        // Check if the pillar is off screen and reset it
        if (pillars[i].x < 0) {
            pillars[i].x = WIDTH - 1;

            // Set the length of the pillar randomly within the defined limits
            pillars[i].length = rand() % (MAX_PILLAR_LENGTH - MIN_PILLAR_LENGTH + 1) + MIN_PILLAR_LENGTH;

            // Randomly set the direction while ensuring no more than 2 consecutive pillars on the same side
            if (i > 0 && pillars[i - 1].direction == 0 && pillars[i - 2].direction == 0) {
                pillars[i].direction = 1;
            } else if (i > 0 && pillars[i - 1].direction == 1 && pillars[i - 2].direction == 1) {
                pillars[i].direction = 0;
            } else {
                pillars[i].direction = rand() % 2;
            }

            if (pillars[i].direction == 0) {
                pillars[i].y = 0;
            } else {
                pillars[i].y = HEIGHT - pillars[i].length;
            }

            (*score)++;
        }
    }

    // Move floating pillars after score reaches 30
    if (*score >= 30) {
        for (int i = 0; i < MAX_FLOATING_PILLARS; i++) {
            floatingPillars[i].y += floatingPillars[i].velocity;

            // Change direction when hitting top or bottom of the screen
            if (floatingPillars[i].y <= 0 || floatingPillars[i].y + floatingPillars[i].length >= HEIGHT) {
                floatingPillars[i].velocity *= -1;
            }

            floatingPillars[i].x--;

            if (floatingPillars[i].x < 0) {
                floatingPillars[i].x = WIDTH - 1;
                floatingPillars[i].y = rand() % (HEIGHT - floatingPillars[i].length);
                floatingPillars[i].velocity = (rand() % 2 == 0) ? 1 : -1;
                (*score)++;
            }
        }
    }

    // Move diagonal pillars after score reaches 50
    if (*score >= 50) {
        for (int i = 0; i < MAX_DIAGONAL_PILLARS; i++) {
            diagonalPillars[i].x += diagonalPillars[i].dx;
            diagonalPillars[i].y += diagonalPillars[i].dy;

            // Change direction when hitting top, bottom, or sides of the screen
            if (diagonalPillars[i].y <= 0 || diagonalPillars[i].y + diagonalPillars[i].length >= HEIGHT) {
                diagonalPillars[i].dy *= -1;
            }
            if (diagonalPillars[i].x <= 0 || diagonalPillars[i].x >= WIDTH - 1) {
                diagonalPillars[i].dx *= -1;
            }

            // Reset diagonal pillar when off-screen
            if (diagonalPillars[i].x < 0 || diagonalPillars[i].x >= WIDTH) {
                diagonalPillars[i].x = WIDTH - 1;
                diagonalPillars[i].y = rand() % (HEIGHT - diagonalPillars[i].length);
                diagonalPillars[i].dx = (rand() % 2 == 0) ? 1 : -1;
                diagonalPillars[i].dy = (rand() % 2 == 0) ? 1 : -1;
                (*score)++;
            }
        }
    }
}

void updatePlayer(Player *player) {
    if (_kbhit()) {
        char ch = _getch();
        if (ch == 's') {
            player->y = (player->y == HEIGHT - 1) ? 0 : HEIGHT - 1;
        }
    }
}

void showWelcomeScreen() {
    system("CLS");
    printf("Welcome to Geometry Dash!\n");
    printf("Instructions:\n");
    printf("1. Press 's' to switch between top and bottom.\n");
    printf("2. Avoid the pillars!\n");
    printf("3. Score points by passing through the gaps.\n");
    printf("Press any key to start...\n");
    _getch();
}

int main() {
    srand(time(0));
    Player player = {HEIGHT - 1};

    showWelcomeScreen();

    // Initialize fixed pillars
    Pillar pillars[MAX_PILLARS];
    for (int i = 0; i < MAX_PILLARS; i++) {
        pillars[i].x = WIDTH - 1 + (i * 6);
        pillars[i].length = rand() % (MAX_PILLAR_LENGTH - MIN_PILLAR_LENGTH + 1) + MIN_PILLAR_LENGTH;

        if (i < 2) {
            pillars[i].direction = rand() % 2;
        } else {
            pillars[i].direction = (pillars[i - 1].direction == 0 && pillars[i - 2].direction == 0) ? 1 : (pillars[i - 1].direction == 1 && pillars[i - 2].direction == 1) ? 0 : rand() % 2;
        }

        if (pillars[i].direction == 0) {
            pillars[i].y = 0;
        } else {
            pillars[i].y = HEIGHT - pillars[i].length;
        }
    }

    // Initialize floating pillars
    FloatingPillar floatingPillars[MAX_FLOATING_PILLARS];
    for (int i = 0; i < MAX_FLOATING_PILLARS; i++) {
        floatingPillars[i].x = WIDTH - 1 + (i * 10);
        floatingPillars[i].length = rand() % (MAX_PILLAR_LENGTH - MIN_PILLAR_LENGTH + 1) + MIN_PILLAR_LENGTH;
        floatingPillars[i].y = rand() % (HEIGHT - floatingPillars[i].length);
        floatingPillars[i].velocity = (rand() % 2 == 0) ? 1 : -1;
    }

    // Initialize diagonal pillars
    DiagonalPillar diagonalPillars[MAX_DIAGONAL_PILLARS];
    for (int i = 0; i < MAX_DIAGONAL_PILLARS; i++) {
        diagonalPillars[i].x = WIDTH - 1 + (i * 10);
        diagonalPillars[i].length = rand() % (MAX_PILLAR_LENGTH - MIN_PILLAR_LENGTH + 1) + MIN_PILLAR_LENGTH;
        diagonalPillars[i].y = rand() % (HEIGHT - diagonalPillars[i].length);
        diagonalPillars[i].dx = (rand() % 2 == 0) ? 1 : -1;
        diagonalPillars[i].dy = (rand() % 2 == 0) ? 1 : -1;
    }

    int score = 0;

    while (1) {
        updatePlayer(&player);
        movePillars(pillars, floatingPillars, diagonalPillars, &score);

        if (checkCollision(&player, pillars, floatingPillars, diagonalPillars, score)) {
            printf("Game Over! Final Score: %d\n", score);
            break;
        }

        drawGame(&player, pillars, floatingPillars, diagonalPillars, score);
        Sleep(25);
    }

    return 0;
}
