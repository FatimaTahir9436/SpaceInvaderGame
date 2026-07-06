#include <iostream>
#include <windows.h>
#include<algorithm>  
#include <string>
#include <fstream>
#include "help.h"  
using namespace std;

// Game  Boundary
const int gameLeft = 50, gameTop = 50, gameRight = 450, gameBottom = 400;

// Game settings
const int rows = 5, colum = 10;
float alienX[rows][colum], alienY[rows][colum];
bool alienAlive[rows][colum];
float bulletX[5], bulletY[5];
bool bulletActive[5];
float alienBulletX[5], alienBulletY[5];
bool alienBulletActive[5];

float playerX = 250, playerY = 350;
int score = 0, lives = 3;
float alienDx = 1.0f, alienDy = 10.0f;
bool gameRunning = true, gamePause = false;
int highScore[6] = { 0 };
         // Function prototypes
// Allien Functions
void initializeAliens();
void drawAliens();
void eraseAliens();
void moveAliens();
//Player Fuctions
void drawJet();
void eraseJet();
//Game logic
void handlePlayerBullets();
void handleAlienBullets();
void checkCollisions();
void handleInput();
void pauseGame();
void saveGameState();
void loadGameState();
void resetGameState();
void StartNewGame();
void endGame();
int  gameLoop();
//scoring and messages
void updateHighScores(int newScore);
void displayHighScores();
void displayWinMessage();
void displayGameOverMessage();
// to intialize aliens
void initializeAliens() {
    float startX = 60, startY = 60;
    float spacingX = 30, spacingY = 30;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < colum; ++j) {
            alienX[i][j] = startX + j * spacingX;
            alienY[i][j] = startY + i * spacingY;
            alienAlive[i][j] = true;
        }
    }

    for (int i = 0; i < 5; ++i) {
        bulletActive[i] = false;
        alienBulletActive[i] = false;
    }
}
// to draw aliens
void drawAliens() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < colum; ++j) {
            if (alienAlive[i][j]) {
                myEllipse(alienX[i][j], alienY[i][j], alienX[i][j] + 20, alienY[i][j] + 20, 0, 255, 0, 255, 255, 0);
            }
        }
    }
}
// to erase aliens
void eraseAliens() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < colum; ++j) {
            if (alienAlive[i][j]) {
                myEllipse(alienX[i][j], alienY[i][j], alienX[i][j] + 20, alienY[i][j] + 20, 0, 0, 0, 0, 0, 0);
            }
        }
    }
}

// to move aliens
void moveAliens() {
    bool reverse = false;
    eraseAliens();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < colum; ++j) {
            if (alienAlive[i][j]) {
                alienX[i][j] += alienDx;

                if (alienX[i][j] + 20 > gameRight || alienX[i][j] < gameLeft) {
                    reverse = true;
                }
            }
        }
    }
    // to reverse the direction
    if (reverse) {
        alienDx = -alienDx;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < colum; ++j) {
                if (alienAlive[i][j]) {
                    alienY[i][j] += alienDy;

                    // If an alien reaches the player area
                    if (alienY[i][j] > gameBottom - 50) {
                        lives = 0;
                    }
                }
            }
        }
    }

    drawAliens();
}
// to draw jet
void drawJet() {
    myRect(playerX, playerY, playerX + 20, playerY + 10, 255, 0, 0);
}
// to erase jet
void eraseJet() {
    myRect(playerX, playerY, playerX + 20, playerY + 10, 0, 0, 0);
}
// to handle player bullets
void handlePlayerBullets() {
    for (int i = 0; i < 5; ++i) {
        if (bulletActive[i]) {
            myLine(bulletX[i], bulletY[i], bulletX[i], bulletY[i] - 10, 0, 0, 0);
            bulletY[i] -= 2;

            if (bulletY[i] < gameTop) {
                bulletActive[i] = false;
            }
            else {
                myLine(bulletX[i], bulletY[i], bulletX[i], bulletY[i] - 10, 255, 255, 255);
            }
        }
    }
}
// to handle aliens bullets
void handleAlienBullets() {
    for (int i = 0; i < 5; ++i) {
        if (alienBulletActive[i]) {
            myLine(alienBulletX[i], alienBulletY[i], alienBulletX[i], alienBulletY[i] + 10, 0, 0, 0);
            alienBulletY[i] += 2;

            if (alienBulletY[i] > gameBottom) {
                alienBulletActive[i] = false;
            }
            else {
                myLine(alienBulletX[i], alienBulletY[i], alienBulletX[i], alienBulletY[i] + 10, 255, 0, 0);
            }
        }
    }
}

// to check collisions
void checkCollisions() {
    // player bullets with aliens
    for (int i = 0; i < 5; i++) {
        if (bulletActive[i]) {
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < colum; ++c) {
                    if (alienAlive[r][c]) {
                        if (bulletX[i] >= alienX[r][c] && bulletX[i] <= alienX[r][c] + 20 &&
                            bulletY[i] >= alienY[r][c] && bulletY[i] <= alienY[r][c] + 20) {
                            alienAlive[r][c] = false;  // Alien is destroyed when hit by bullet
                            bulletActive[i] = false;   // Bullet is destroyed
                            score += 10;
                            break;

                        }
                    }
                }
                if (!bulletActive[i])
                    break;
            }
        }
    }

    // alien bullets with player
    for (int i = 0; i < 5; ++i) {
        if (alienBulletActive[i]) {
            if (alienBulletX[i] >= playerX && alienBulletX[i] <= playerX + 20 &&
                alienBulletY[i] >= playerY && alienBulletY[i] <= playerY + 10) {
                alienBulletActive[i] = false;  // Alien bullet disappears when it hits the jet
                lives--;
            }
        }
    }

    // jet-to-alien collision (touching the alien grid)
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < colum; ++c) {
            if (alienAlive[r][c]) {
                // if the player jet is colliding with the alien grid
                if (playerX + 20 >= alienX[r][c] && playerX <= alienX[r][c] + 20 &&
                    playerY + 10 >= alienY[r][c] && playerY <= alienY[r][c] + 20) {

                    lives--;

                    if (lives > 0) {
                        myText(200, 200, "You lost a life", 255, 0, 0);
                        Sleep(1000);
                    }

                    return;
                }
            }
        }
    }

}
// to handle input
void handleInput() {
    if (GetAsyncKeyState(VK_LEFT) && playerX > gameLeft) {
        eraseJet();
        playerX -= 5;
        drawJet();
    }
    if (GetAsyncKeyState(VK_RIGHT) && playerX < gameRight - 20) {
        eraseJet();
        playerX += 5;
        drawJet();
    }
    if (GetAsyncKeyState(VK_UP) && playerY > gameTop) {
        eraseJet();
        playerY -= 5;
        drawJet();
    }
    if (GetAsyncKeyState(VK_DOWN) && playerY < gameBottom - 10) {
        eraseJet();
        playerY += 5;
        drawJet();
    }
    if (GetAsyncKeyState(VK_SPACE)) {
        for (int i = 0; i < 5; ++i) {
            if (!bulletActive[i]) {
                bulletX[i] = playerX + 10;
                bulletY[i] = playerY;
                bulletActive[i] = true;
                break;
            }
        }
    }
    if (GetAsyncKeyState(0x50)) { // 'P' key for pause
        pauseGame();
    }
}
// to pause game
void pauseGame() {
    cout << "Game Paused! Press 'P' to resume.\n";
    while (true) {
        if (GetAsyncKeyState(0x50)) { // 'P' key to resume the game
            break;
        }
    }
}

// to save game state 
void saveGameState() {
    ofstream saveFile("game_save.txt");
    if (saveFile.is_open()) {
        // save player data
        saveFile << playerX << " " << playerY << " " << score << " " << lives << endl;

        // save aliens state
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < colum; ++j) {
                saveFile << alienAlive[i][j] << " ";
            }
            saveFile << endl;
        }
        for (int i = 0; i < 5; i++) {
            saveFile << highScore[i] << endl;//save high score
        }

        saveFile.close();
        myText(200, 200, "Game Saved!", 255, 255, 0);
        Sleep(2000);
    }
    else {
        myText(200, 200, "Error Saving Game!", 255, 0, 0);
        Sleep(2000);
    }
}

// to load game state
void loadGameState() {
    ifstream saveFile("game_save.txt");
    if (saveFile.is_open()) {
        // to load player data
        saveFile >> playerX >> playerY >> score >> lives;

        // to load alien states
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < colum; ++j) {
                saveFile >> alienAlive[i][j];
            }
        }
        saveFile.close();

        myText(200, 200, "Game Loaded!", 0, 255, 0);
        Sleep(2000);
    }
    else {
        myText(200, 200, "No Save File Found!", 255, 0, 0);
        Sleep(2000);
    }

    // load high scores separately
    ifstream highScoresFile("highScore.txt");
    if (highScoresFile.is_open()) {
        for (int i = 0; i < 5; i++) {
            highScoresFile >> highScore[i];
        }
        highScoresFile.close();
    }
    else {
        for (int i = 0; i < 5; i++) {
            highScore[i] = 0;
        }
    }
}
// to reset game
void resetGameState() {
    score = 0;
    lives = 3;

}
// to start new game
void StartNewGame() {
    resetGameState();
}

// to end game
void endGame() {
    updateHighScores(score);
    displayHighScores();
}

// main game loop
int gameLoop() {
    initializeAliens();

    while (lives > 0 && score < rows * colum * 10) {
        myRect(gameLeft, gameTop, gameRight, gameBottom, 0, 0, 0); // to clear game area
        drawJet();
        handlePlayerBullets();
        handleAlienBullets();
        checkCollisions();
        moveAliens();

        // fire alien bullets randomly
        if (rand() % 100 < 2) {
            for (int i = 0; i < 5; ++i) {
                if (!alienBulletActive[i]) {
                    int r = rand() % rows, c = rand() % colum;
                    if (alienAlive[r][c]) {
                        alienBulletX[i] = alienX[r][c] + 10;
                        alienBulletY[i] = alienY[r][c] + 20;
                        alienBulletActive[i] = true;
                        break;
                    }
                }
            }
        }

        handleInput();
        myText(10, 10, "Score: " + to_string(score), 255, 255, 255);
        myText(10, 30, "Lives: " + to_string(lives), 255, 255, 255);

        Sleep(30);
    }


    if (lives <= 0) {
        displayGameOverMessage();
    }
    else if (score == rows * colum * 10) {
        displayWinMessage();
    }
    return score;
}

void updateHighScores(int newScore) {
    highScore[6] = newScore;

    for (int i = 0; i < 5; ++i) {
        for (int j = i + 1; j < 6; ++j) {
            if (highScore[j] > highScore[i]) {
                swap(highScore[i], highScore[j]);
            }
        }
    }

    // save top 5 scores to file
    ofstream highScoresFile("highScore.txt");
    for (int i = 0; i < 5; ++i) {
        highScoresFile << highScore[i] << endl;
    }
    highScoresFile.close();
}
// diplay high scores on screen
  void displayHighScores() {
    system("cls");
    myText(200, 100, "High Scores", 255, 255, 0);

    for (int i = 0; i < 5; ++i) {
        string scoreText = "Score " + to_string(i + 1) + ": " + to_string(highScore[i]);
        myText(200, 150 + i * 30, scoreText, 255, 255, 255);
    }

    myText(200, 350, "Press Enter key to return to menu", 255, 255, 0);
    while (!GetAsyncKeyState(VK_RETURN)) {
        Sleep(1000);
    }
}

// display win message
void displayWinMessage() {
    myText(200, 200, "You Win! Final Score: " + to_string(score), 0, 255, 0);

    myText(200, 350, "Press Enter key to return to menu", 255, 255, 0);
    while (!GetAsyncKeyState(VK_RETURN)) {
        Sleep(3000);
    }
    updateHighScores(score);
}
// display game over message
void displayGameOverMessage() {
    myText(200, 200, "Game Over! Final Score: " + to_string(score), 255, 0, 0);
    Sleep(3000);
    updateHighScores(score);
}

int main() {
    //to load high scores at startup
    ifstream highScoresFile("highScore.txt");
    if (highScoresFile.is_open()) {
        for (int i = 0; i < 5; ++i) {
            highScoresFile >> highScore[i];
        }
        highScoresFile.close();
    }

    while (true) {
        system("cls");
        myText(200, 150, "Welcome to Space Invaders", 255, 255, 0);
        myText(200, 200, "Press S to Start", 0, 255, 0);
        myText(200, 250, "Press L to Load Game", 0, 255, 255);
        myText(200, 300, "Press H to View High Scores", 255, 255, 0);
        myText(200, 350, "Press Q to Quit", 255, 0, 0);

        if (GetAsyncKeyState(0x53)) { //to start game (S)
            resetGameState();
            int finalScore = gameLoop();
            updateHighScores(finalScore);
        }
        else if (GetAsyncKeyState(0x4C)) { // to load game (L)
            loadGameState();
            int finalScore = gameLoop();
            updateHighScores(finalScore);
        }
        else if (GetAsyncKeyState(0x48)) { // to view high scores (H)
            displayHighScores();
        }
        else if (GetAsyncKeyState(0x51)) { // to quit game (Q)
            return 0;
        }

        Sleep(200);
    }
}


