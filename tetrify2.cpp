#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "math.h"
#pragma comment(lib,"Winmm.lib")

using namespace std;
using namespace System;
using namespace std::chrono;

// Game defines
#ifndef NUM_TETROMINOES
#define NUM_TETROMINOES 7
#define NUM_TETROMINO_LINES 4
#define NUM_TETROMINO_COLS 4
#define STARTING_OFFSET 2
#define NUM_SCREEN_COLS 34
#define NUM_SCREEN_ROWS 26
#define NUM_CONTROL_KEYS 20
#define MAX_SCORE 999999
#define FIELD_WIDTH 12
#define FIELD_HEIGHT 22
#endif // !NUM_TETROMINOES

// Print fps info?
#ifndef DEBUG
#define DEBUG false
#endif // !DEBUG

// Orientation enum
enum Rotations
{
    ROT_0 = 0,
    ROT_90,
    ROT_180,
    ROT_270,
    ROT_NUM_ROT
};

// Pieces shapes enum
enum Pieces
{
    PI_LINE = 0,
    PI_T,
    PI_REV_L,
    PI_BLOCK,
    PI_REV_S,
    PI_S,
    PI_L,
    PI_NUM_PIECES
};

// Enum for game states
enum GameState
{
    GS_MAIN_MENU = 0,
    GS_DIFFICULTY_SELECTION,
    GS_PLAYING,
    GS_PAUSE,
    GS_RESTARTING,
    GS_GAME_OVER
};

// Enum for keyboard keys for input
enum keys
{
    KEY_R_ARROW = 0,
    KEY_L_ARROW,
    KEY_D_ARROW,
    KEY_U_ARROW,
    KEY_ESC,
    KEY_SPACE,
    KEY_Q,
    KEY_R,
    KEY_M,
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_SHIFT
};

/// <summary>
/// Function to rotate tetrominos 
/// </summary>
/// <param name="px">X position of the tetromino</param>
/// <param name="py">Y position of the tetromino</param>
/// <param name="r">Number of rotation to perform</param>
/// <returns>Returns rotated position of a block of the tetromino</returns>
int Rotate(int px, int py, int r)
{
    // Process roation
    switch (r % ROT_NUM_ROT)
    {
    case ROT_0: return (py * NUM_TETROMINO_COLS) + px;      // 0   degrees
    case ROT_90: return 12 + py - (NUM_TETROMINO_LINES * px); // 90  degrees
    case ROT_180: return 15 - (py * NUM_TETROMINO_COLS) - px; // 180 degrees
    case ROT_270: return 3 - py + (NUM_TETROMINO_LINES * px);  // 2NUM_TETROMINOES0 degrees
    default: return 0;
    }
}

/// <summary>
/// Function to check if the piece fits on the field 
/// </summary>
/// <param name="tetromino">The tetromino being checked</param>
/// <param name="pField">The field that the tetromino is being checked in</param>
/// <param name="nRotation">The rotation of the tetromino</param>
/// <param name="nPosX">The X position of the tetromino</param>
/// <param name="nPosY">The Y position of the tetromino</param>
/// <returns>Returns if the piece fits into the field</returns>
bool DoesPieceFit(const wstring &tetromino, 
                  const unsigned char* pField,
                  const int &nRotation,
                  const int &nPosX,
                  const int &nPosY)
{
    // Iterate tetromino cols
    for (int px = 0; px < NUM_TETROMINO_COLS; px++)
    {
        // Iterate tetromino rows
        for (int py = 0; py < NUM_TETROMINO_LINES; py++)
        {
            // Get index into piece
            int pi = Rotate(px, py, nRotation);

            // Get index into field
            int fi = (nPosY + py) * FIELD_WIDTH + (nPosX + px);

            // Check that we are in the field width
            if (nPosX + px >= 0 && nPosX + px < FIELD_WIDTH)
            {
                // Check that we are in the field height
                if (nPosY + py >= 0 && nPosY + py < FIELD_HEIGHT)
                {
                    // Check for existing piece
                    if (tetromino[pi] == L'X' && pField[fi] != 0)
                        // We colided
                        return false;
                }
            }

        }
    }

    // All clear
    return true;
}

// Main program loop
int main()
{
    // Initialize rand
    srand(time(0));

    // Tetrominoes
    wstring tetromino[NUM_TETROMINOES];
    wstring nextPieceBorder;
    
    // Play field
    unsigned char* pField = nullptr;
    
    // Screen for diplay
    wstring titleSreen;
    wstring difficultySreen;
    wstring gameOverSreen;
    wstring pausedSreen;
    wstring maxOutScreen;

    // Create assets
    tetromino[PI_LINE].append(L"..X.");
    tetromino[PI_LINE].append(L"..X.");
    tetromino[PI_LINE].append(L"..X.");
    tetromino[PI_LINE].append(L"..X.");

    tetromino[PI_T].append(L"..X.");
    tetromino[PI_T].append(L".XX.");
    tetromino[PI_T].append(L"..X.");
    tetromino[PI_T].append(L"....");

    tetromino[PI_REV_L].append(L"..X.");
    tetromino[PI_REV_L].append(L"..X.");
    tetromino[PI_REV_L].append(L".XX.");
    tetromino[PI_REV_L].append(L"....");

    tetromino[PI_BLOCK].append(L".XX.");
    tetromino[PI_BLOCK].append(L".XX.");
    tetromino[PI_BLOCK].append(L"....");
    tetromino[PI_BLOCK].append(L"....");

    tetromino[PI_REV_S].append(L"..X.");
    tetromino[PI_REV_S].append(L".XX.");
    tetromino[PI_REV_S].append(L".X..");
    tetromino[PI_REV_S].append(L"....");

    tetromino[PI_S].append(L".X..");
    tetromino[PI_S].append(L".XX.");
    tetromino[PI_S].append(L"..X.");
    tetromino[PI_S].append(L"....");

    tetromino[PI_L].append(L".X..");
    tetromino[PI_L].append(L".X..");
    tetromino[PI_L].append(L".XX.");
    tetromino[PI_L].append(L"....");

    // Border around the next piece indication
    nextPieceBorder.append(L"XXXXXXXX");
    nextPieceBorder.append(L"X      X");
    nextPieceBorder.append(L"X      X");
    nextPieceBorder.append(L"X      X");
    nextPieceBorder.append(L"X      X");
    nextPieceBorder.append(L"X      X");
    nextPieceBorder.append(L"X      X");
    nextPieceBorder.append(L"XXXXXXXX");

    // Create screens
    titleSreen.append(L"==================================");
    titleSreen.append(L"#                                #");
    titleSreen.append(L"#            CONSOLE             #");
    titleSreen.append(L"#                                #");
    titleSreen.append(L"#     ### ## ### ### ### ###     #");
    titleSreen.append(L"#      #  #   #  # #  #  #       #");
    titleSreen.append(L"#      #  ##  #  ##   #  ###     #");
    titleSreen.append(L"#      #  #   #  # #  #    #     #");
    titleSreen.append(L"#      #  ##  #  # # ### ###     #");
    titleSreen.append(L"#     ======================     #");
    titleSreen.append(L"#                                #");
    titleSreen.append(L"#      PRESS SPACE TO START      #");
    titleSreen.append(L"#              +                 #");
    titleSreen.append(L"#              o+                #");
    titleSreen.append(L"#              l|         +      #");
    titleSreen.append(L"#     +       / ^\\        |      #");
    titleSreen.append(L"#     |   +  / //\\\\  +    ^      #");
    titleSreen.append(L"#     ^   | / (///)\\ |   //\\     #");
    titleSreen.append(L"#    //\\  ^ |  |U| | ^  (///)    #");
    titleSreen.append(L"#   (///)//\\|  /U\\ |//\\  |U|     #");
    titleSreen.append(L"#    |U|(///) |UUU|(///)|UUU|    #");
    titleSreen.append(L"#   |UUU||U|| | ' |||U| |===|    #");
    titleSreen.append(L"#   |===|UUU| |===||UUU||   |    #");
    titleSreen.append(L"#   |   | ' | |   || ' ||   |    #");
    titleSreen.append(L"#   |MMM|OOO| |MMM||OOO||MMM|    #");
    titleSreen.append(L"==================================");

    difficultySreen.append(L"==================================");
    difficultySreen.append(L"#           ~~~~~~~~~~           #");
    difficultySreen.append(L"# *        ~~CONTROLS~~        * #");
    difficultySreen.append(L"#  ============================  #");
    difficultySreen.append(L"#  \u25c4 \u25ba - Move left/right         #");
    difficultySreen.append(L"#   \u25b2  - Rotate piece            #");
    difficultySreen.append(L"#   \u25bc  - Move piece down         #");
    difficultySreen.append(L"#  ESC - Pause                   #");
    difficultySreen.append(L"#   Q  - Quit(any screen)        #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#       CHOOSE DIFFICULTY        #");
    difficultySreen.append(L"#  ============================  #");
    difficultySreen.append(L"#  PRESS 0-9: Difficulty level   #");
    difficultySreen.append(L"#     **HOLD SHIFT FOR +10**     #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#  Selected                      #");
    difficultySreen.append(L"#  Difficulty:                   #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#  ============================  #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#  ***PRESS [SPACE] TO START***  #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"# *            *               * #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"==================================");

    gameOverSreen.append(L"==================================");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"# *   ####   ##   #   #  ###   * #");
    gameOverSreen.append(L"#     #     #  #  ## ##  #       #");
    gameOverSreen.append(L"#     # ##  ####  # # #  ###     #");
    gameOverSreen.append(L"#     #  #  #  #  #   #  #       #");
    gameOverSreen.append(L"#     ####  #  #  #   #  ###     #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#    ####  #  #  ###  ####  ##   #");
    gameOverSreen.append(L"#    #  #  #  #  #    #  #  ##   #");
    gameOverSreen.append(L"#    #  #  #  #  ###  ###   ##   #");
    gameOverSreen.append(L"#    #  #  #  #  #    #  #       #");
    gameOverSreen.append(L"#    ####   ##   ###  #  #  ##   #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#   ==========================   #");
    gameOverSreen.append(L"#   *                        *   #");
    gameOverSreen.append(L"#       PRESS R TO RETRY         #");
    gameOverSreen.append(L"#       PRESS M FOR MAIN MENU    #");
    gameOverSreen.append(L"#       PRESS Q TO QUIT          #");
    gameOverSreen.append(L"#    *                       *   #");
    gameOverSreen.append(L"# ****************************** #");
    gameOverSreen.append(L"#  *   *    *    *    *   *  *   #");
    gameOverSreen.append(L"#    * * *     * * *    * * *    #");
    gameOverSreen.append(L"#  * * * * * * * * * * * * * *   #");
    gameOverSreen.append(L"# ****************************** #");
    gameOverSreen.append(L"==================================");

    maxOutScreen.append(L"==================================");
    maxOutScreen.append(L"#                                #");
    maxOutScreen.append(L"#         #   #  #  #  #         #");
    maxOutScreen.append(L"#         ## ## # #  ##          #");
    maxOutScreen.append(L"#         # # # ###  ##          #");
    maxOutScreen.append(L"#         #   # # # #  #         #");
    maxOutScreen.append(L"#                                #");
    maxOutScreen.append(L"#    ###  ### ### ### ###  ##    #");
    maxOutScreen.append(L"#    #    #   # # # # #    ##    #");
    maxOutScreen.append(L"#    ###  #   # # ##  ###  ##    #");
    maxOutScreen.append(L"#      #  #   # # # # #          #");
    maxOutScreen.append(L"#    ###  ### ### # # ###  ##    #");
    maxOutScreen.append(L"#   ==========================   #");
    maxOutScreen.append(L"#        CONGRADULATIONS         #");
    maxOutScreen.append(L"#   ==========================   #");
    maxOutScreen.append(L"#                                #");
    maxOutScreen.append(L"#       PRESS R TO RETRY         #");
    maxOutScreen.append(L"#       PRESS M FOR MAIN MENU    #");
    maxOutScreen.append(L"#       PRESS Q TO QUIT          #");
    maxOutScreen.append(L"#                                #");
    maxOutScreen.append(L"#   \u2588        \u2588  \u2588        \u2588  \u2588    #");
    maxOutScreen.append(L"#   \u2588   \u2588\u2588   \u2588  \u2588   \u2588   \u2588\u2588  \u2588\u2588   #");
    maxOutScreen.append(L"#   \u2588\u2588  \u2588\u2588  \u2588\u2588  \u2588  \u2588\u2588\u2588  \u2588    \u2588   #");
    maxOutScreen.append(L"#               \u2588                #");
    maxOutScreen.append(L"#                                #");
    maxOutScreen.append(L"==================================");

    pausedSreen.append(L"==================================");
    pausedSreen.append(L"#      ____                      #");
    pausedSreen.append(L"#     /   /                      #");
    pausedSreen.append(L"#    /___/ ___       __  __  _   #");
    pausedSreen.append(L"#   /     /__/ /  / /_  /_  / \\  #");
    pausedSreen.append(L"#  /     /  / /__/ __/ /_  /__/  #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#     PRESS ESC TO RESUME        #");
    pausedSreen.append(L"#     PRESS R TO RESTART         #");
    pausedSreen.append(L"#     PRESS M FOR MAIN MENU      #");
    pausedSreen.append(L"#     PRESS Q TO QUIT            #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#              +                 #");
    pausedSreen.append(L"#              o+                #");
    pausedSreen.append(L"#              l|         +      #");
    pausedSreen.append(L"#     +       / ^\\        |      #");
    pausedSreen.append(L"#     |   +  / //\\\\  +    ^      #");
    pausedSreen.append(L"#     ^   | / (///)\\ |   //\\     #");
    pausedSreen.append(L"#    //\\  ^ |  |U| | ^  (///)    #");
    pausedSreen.append(L"#   (///)//\\|  /U\\ |//\\  |U|     #");
    pausedSreen.append(L"#    |U|(///) |UUU|(///)|UUU|    #");
    pausedSreen.append(L"#   |UUU||U|| | ' |||U| |===|    #");
    pausedSreen.append(L"#   |===|UUU| |===||UUU||   |    #");
    pausedSreen.append(L"#   |   | ' | |   || ' ||   |    #");
    pausedSreen.append(L"#   |MMM|OOO| |MMM||OOO||MMM|    #");
    pausedSreen.append(L"==================================");

    // Setup field
    pField = new unsigned char[FIELD_WIDTH * FIELD_HEIGHT];

    // Setup the screen for drawing to
    wchar_t* screen = new wchar_t[NUM_SCREEN_COLS * NUM_SCREEN_ROWS];
    
    // Set console size
    Console::SetWindowSize(NUM_SCREEN_COLS, NUM_SCREEN_ROWS);
    
    // Set console title
    SetConsoleTitle(L"Console TETRIS");
    
    // Clear out the screen
    for (int i = 0; i < NUM_SCREEN_COLS * NUM_SCREEN_ROWS; i++) screen[i] = L' ';
    
    // Handle to draw to console
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    
    // Set console to the console handle that was created
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Flag for game over
    bool bGameOver = false;

    // Current piece variables
    int  nCurrentPiece = rand() % NUM_TETROMINOES;
    int  nCurrentRotation = 0;
    int  nCurrentX = (FIELD_WIDTH / 2) - STARTING_OFFSET;
    int  nCurrentY = 0;

    // Next Piece
    int  nNextPiece = rand() % NUM_TETROMINOES;
    
    // Projected piece
    int  nProjectedY = 0;

    // Bool array for pressed keys
    bool bKey[NUM_CONTROL_KEYS];

    // Button holds
    bool bRotateHold = false;
    bool bPauseHold = false;
    bool bStartHold = false;
    bool bDirectionHeld = false;

    // DAS(delayed auto shift)
    int  nDAS = 10;
    int  nDASCounter = 0;

    // Difficulty
    int nSpeed = 20;
    int nDifficulty = 0;
    int nSelectedDifficulty = 0;
    int nSpeedCounter = 0;
    int nLineCount = 0;
    bool bForceDown = false;
    int nInitialLevelTransition;
    
    // Timing
    high_resolution_clock::time_point timer;
    high_resolution_clock::time_point timer_last;
    high_resolution_clock::time_point fps_timer_last;
    int fps_counter = 0;
    float fps = 60.0f;

    // Currently playing
    bool bPlaying = true;

    // Score data
    int nLineCountLast = 0;
    int nDistPushed;
    int nScore = 0;
    fstream score_file;
    int nStoredScore = 0;
    int nHighScore = 0;
    vector<int> vLines;

    // Open Score file
    score_file.open("cTetScore.txt", ios::in);
    
    // Check for existign score file
    if (score_file)
    {
        // Get data from file
        string scoreStr;
        getline(score_file, scoreStr);
        score_file.close();

        // Check for valid data
        if (scoreStr != "\0")
            // Convert to int
            nStoredScore = std::stoi(scoreStr);

        // Set high score to read file
        nHighScore = nStoredScore;
    }
    
    // Set initial game state
    GameState gState = GS_MAIN_MENU;

    // LAMBDAS=================================================================

    // Lambda to reset the play field
    auto setupField = [](unsigned char* pField) {
        for (int x = 0; x < FIELD_WIDTH; x++)
            for (int y = 0; y < FIELD_HEIGHT; y++)
                pField[y * FIELD_WIDTH + x] = (x == 0 || x == FIELD_WIDTH - 1 || y == FIELD_HEIGHT - 1) ? 8 : 0;
    };

    // Lambda to clear the screen
    auto clearScreen = [] (wchar_t* screen){
        for (int x = 0; x < NUM_SCREEN_COLS; x++)
            for (int y = 0; y < NUM_SCREEN_ROWS; y++)
                screen[y * NUM_SCREEN_COLS + x] = L' ';
    };

    // Lambda for finding the right speed based on level
    // This is tuned to NES tetris
    auto FindSpeed = [](int nLevel) {
        int retVal = 48;
        if (nLevel >= 29)
            retVal = 1;
        else if (nLevel >= 19)
            retVal = 2;
        else if (nLevel >= 16)
            retVal = 3;
        else if (nLevel >= 13)
            retVal = 4;
        else if (nLevel >= 10)
            retVal = 5;
        else
        {
            switch (nLevel)
            {
                case 9:
                    retVal = 6;
                    break;
                case 8:
                    retVal = 8;
                    break;
                case 7:
                    retVal = 13;
                    break;
                default:
                    retVal = 48 - 5 * nLevel;
                    break;
            }
        }

        return retVal;
    };
    
    // Lambda to reset the current peice back to base position
    auto ResetCurrentPiece = [](int& nCurrentX,
                                int& nCurrentY, 
                                int& nCurrentRotation, 
                                int& nDistPushed,
                                int& nProjectedY,
                                wstring& CurrentPiece,
                                unsigned char* pField)
    {
        // Reset position
        nCurrentX = (FIELD_WIDTH / 2) - STARTING_OFFSET;
        nCurrentY = 0;
        
        // Reset rotation
        nCurrentRotation = 0;

        // Reset distance pushed
        nDistPushed = 0;

        // Reset projection
        nProjectedY = nCurrentY;

        // Project down the project piece
        while (DoesPieceFit(CurrentPiece, pField, nCurrentRotation, nCurrentX, nProjectedY))
            nProjectedY++;
        nProjectedY--;
    };
    
    // Lambda to transfer the next to current and get a random next
    auto NextPieceToCurrent = [](int& nNextPiece, int& nCurrentPiece) 
    {
        nCurrentPiece = nNextPiece;
        nNextPiece = rand() % NUM_TETROMINOES;
    };

    // Lambda to randomize the next and current piece
    auto RandomizePieces = [](int& nNextPiece, 
                              int& nCurrentPiece)
    {
        nCurrentPiece = rand() % NUM_TETROMINOES;
        nNextPiece = rand() % NUM_TETROMINOES;
    };

    // Lambda to get inputs
    auto GetInputs = [](bool (&bKeys)[NUM_CONTROL_KEYS]) {
        for (int k = 0; k < NUM_CONTROL_KEYS; k++)
            //                                                     ->  <-   V   ^  esc spc  Q   R   M   0   1   2   3   4   5   6   7   8   9  SHIFT
            bKeys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26\x1b\x20\x51\x52\x4d\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x10"[k]))) != 0;
    };

    // Lambda to fill screen with another menu/screen
    auto FillScreen = [](wchar_t* screen,const wstring& screenToDraw) {
    
        for (int x = 0; x < NUM_SCREEN_COLS; x++)
            for (int y = 0; y < NUM_SCREEN_ROWS; y++)
                screen[y * NUM_SCREEN_COLS + x] = screenToDraw[y * NUM_SCREEN_COLS + x];
    };

    // Lambda to draw a piece to a screen
    auto DrawPiece = [](wstring &CurrentPiece,
                        const int &nCurrentRotation,
                        const int &nCurrentX,
                        const int &nCurrentY,
                        wchar_t* screen,
                        const wchar_t &displayChar,
                        const wchar_t &altDisplayChar = L'\0') {
        for (int px = 0; px < NUM_TETROMINO_COLS; px++)
            for (int py = 0; py < NUM_TETROMINO_LINES; py++)
                if (CurrentPiece[Rotate(px, py, nCurrentRotation)] == L'X')
                    screen[(nCurrentY + py + STARTING_OFFSET) * NUM_SCREEN_COLS + (nCurrentX + px + STARTING_OFFSET)] = displayChar;
                else if(altDisplayChar != L'\0')
                    screen[(nCurrentY + py + STARTING_OFFSET) * NUM_SCREEN_COLS + (nCurrentX + px + STARTING_OFFSET)] = altDisplayChar;
    };

    // Lambda to reset timers
    auto ResetTimers = [](high_resolution_clock::time_point& timer,
                          high_resolution_clock::time_point& timer_last,
                          high_resolution_clock::time_point& fps_timer_last) {
        timer = high_resolution_clock::now(); 
        timer_last = high_resolution_clock::now();
        fps_timer_last = high_resolution_clock::now();
    };

    // Lambda to take a screen and animate all '*' on it
    auto AnimateStar = [](wstring& Screen)
    {
        // Timer
        static int animTimer = 0;

        // Animate every 10 calls
        if (animTimer >= 10)
        {
            // Animate
            for (int x = 0; x < NUM_SCREEN_COLS; x++)
                for (int y = 0; y < NUM_SCREEN_ROWS; y++)
                {
                    int index = y * NUM_SCREEN_COLS + x;
                    if (Screen[index] == L'*')
                        Screen[y * NUM_SCREEN_COLS + x] = L'\u2736';
                    else if (Screen[index] == L'\u2736')
                        Screen[y * NUM_SCREEN_COLS + x] = L'*';
                }

            // Reset timer
            animTimer = 0;
        }
        else
            // Incriment timer
            animTimer++;
    };

    // Main Game loop
    while (bPlaying)
    {
        // GAME TIMING ========================================================
        this_thread::sleep_for(50ms);

        // INPUT ==============================================================
        GetInputs(bKey);

        // MAIN MENU ==========================================================
        if (gState == GS_MAIN_MENU)
        {
            // Draw title screen
            FillScreen(screen, titleSreen);

            // Go to difficulty selection
            if (bKey[KEY_SPACE] && !bStartHold)
            {
                // Clear the screen
                clearScreen(screen);

                // Change game state to difficulty selection
                gState = GS_DIFFICULTY_SELECTION;

                // Holding does nothing
                bStartHold = true;
            }
            // Holding space does not proceed to quick
            else if (!bKey[KEY_SPACE])
                bStartHold = false;

            // Can quit with Q key
            bGameOver = bKey[KEY_Q];
            bPlaying = !bGameOver;
        }

        // DIFFICULTY SELECTION ===============================================
        if (gState == GS_DIFFICULTY_SELECTION)
        {
            // Animate stars on screen
            AnimateStar(difficultySreen);

            // Draw the screen
            FillScreen(screen, difficultySreen);

            // Write the selected difficulty
            swprintf_s(&screen[16 * NUM_SCREEN_COLS + 19], 3, L"%02d", nSelectedDifficulty);

            // Get difficulty selection
            if (bKey[KEY_0])
                nSelectedDifficulty = 0 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_1])
                nSelectedDifficulty = 1 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_2])
                nSelectedDifficulty = 2 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_3])
                nSelectedDifficulty = 3 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_4])
                nSelectedDifficulty = 4 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_5])
                nSelectedDifficulty = 5 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_6])
                nSelectedDifficulty = 6 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_7])
                nSelectedDifficulty = 7 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_8])
                nSelectedDifficulty = 8 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_9])
                nSelectedDifficulty = 9 + (bKey[KEY_SHIFT] ? 10 : 0);

            // Space to star game
            if (bKey[KEY_SPACE] && !bStartHold)
            {
                // Clear what is curently on the screen
                clearScreen(screen);

                // Setup the playfield
                setupField(pField);

                // Change gamestate to playing
                gState = GS_PLAYING;

                // Holding space
                bStartHold = true;

                // Reset scores
                nScore = 0;
                nLineCount = 0;
                nLineCountLast = 0;

                // Reset pieces
                RandomizePieces(nNextPiece, nCurrentPiece);
                ResetCurrentPiece(nCurrentX, nCurrentY, nCurrentRotation, nDistPushed, nProjectedY, tetromino[nCurrentPiece], pField);

                // Set difficulty
                nDifficulty = nSelectedDifficulty;

                // Set initial level(line) transition
                nInitialLevelTransition = max(100, nDifficulty * 10 - 50);

                // Get speed
                nSpeed = FindSpeed(nDifficulty);

                // Reset timers
                ResetTimers(timer, timer_last, fps_timer_last);
            }
            // Let go of space
            else if(!bKey[KEY_SPACE])
                // Not holding space
                bStartHold = false;

            // Q to quit
            bGameOver = bKey[KEY_Q];
            bPlaying = !bGameOver;
        }
        
        // PLAYING GAME=========================================================
        while (!bGameOver && (gState == GS_PLAYING || gState == GS_PAUSE))
        {
            // GAME TIMING =====================================================
            // Find execution time
            timer = high_resolution_clock::now();
            duration<double, std::milli> time_span = timer - timer_last;
            timer_last = high_resolution_clock::now();
            
            // Calculate FPS
            fps_counter++;
            duration<double, std::milli> time_span_fps = timer - fps_timer_last;
            if (time_span_fps >= 1000ms)
            {
                fps = (fps_counter / time_span_fps.count()) * 1000.0f;
                fps_counter = 0;
                fps_timer_last = high_resolution_clock::now();
            }

            // Sleep to keep framerate
            this_thread::sleep_for(16.67ms - (time_span));

            // INPUT ===========================================================
            // Get inputs
            GetInputs(bKey);

            // PAUSE ===========================================================
            if (gState == GS_PAUSE)
            {
                // Draw pause screen
                FillScreen(screen, pausedSreen);

                // Esc to resume
                if (bKey[KEY_ESC] && !bPauseHold)
                {
                    // Clear the screen
                    clearScreen(screen);

                    // Set state to playing
                    gState = GS_PLAYING;

                    // Holding Esc
                    bPauseHold = true;
                }
                // Let go of Esc
                else if (!bKey[KEY_ESC])
                {
                    // Not holding Esc
                    bPauseHold = false;
                }

                // R to restart
                if (bKey[KEY_R])
                {
                    // Clear the screen
                    clearScreen(screen);

                    // Setup the field
                    setupField(pField);

                    // Set state to playing
                    gState = GS_PLAYING;

                    // Reset game over
                    bGameOver = false;

                    // Reset score
                    nScore = 0;

                    // Reset difficulty
                    nDifficulty = nSelectedDifficulty;

                    // Reset pieces
                    RandomizePieces(nNextPiece, nCurrentPiece);
                    ResetCurrentPiece(nCurrentX, nCurrentY, nCurrentRotation, nDistPushed, nProjectedY, tetromino[nCurrentPiece], pField);

                    // Reset timers
                    ResetTimers(timer, timer_last, fps_timer_last);
                }

                // Q to quit
                if (bKey[KEY_Q])
                {
                    bGameOver = true;
                    bPlaying = false;
                }

                // M to go to main menu
                if (bKey[KEY_M])
                {
                    // Set game state to main menu
                    gState = GS_MAIN_MENU;

                    // Clear the screen
                    clearScreen(screen);

                    // No game over
                    bGameOver = false;
                }
            }
            else 
            {
                // GAME LOGIC ======================================================

                // Update difficulty
                if (nLineCount >= nInitialLevelTransition && nLineCount != nLineCountLast && nLineCount % 10)
                {
                    nDifficulty++;
                    nSpeed = FindSpeed(nDifficulty);
                }

                // Update line count last
                nLineCountLast = nLineCount;

                // Increment speed counter
                nSpeedCounter++;

                // When the speed counter crosses a threshold force down
                bForceDown = (nSpeedCounter >= nSpeed);

                // Holding a deirection?
                if (!bKey[KEY_L_ARROW] && !bKey[KEY_R_ARROW] && !bKey[KEY_U_ARROW] && !bKey[KEY_D_ARROW])
                {
                    bDirectionHeld = false;
                    nDASCounter = nDASCounter >= 0 ? (nDASCounter - 1) : 0;
                }

                // Left arrow pressed
                if (bKey[KEY_L_ARROW])
                {
                    if (!bDirectionHeld)
                    {
                        nCurrentX -= (DoesPieceFit(tetromino[nCurrentPiece], pField, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
                        bDirectionHeld = true;
                    }
                    else
                    {
                        if (nDASCounter < nDAS)
                            nDASCounter++;
                        else
                        {
                            nCurrentX -= (DoesPieceFit(tetromino[nCurrentPiece], pField, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
                        }
                    }

                    nProjectedY = nCurrentY;
                    while (DoesPieceFit(tetromino[nCurrentPiece], pField, nCurrentRotation, nCurrentX, nProjectedY))
                        nProjectedY++;
                    nProjectedY--;
                }

                // Right arrow pressed
                if (bKey[KEY_R_ARROW])
                {
                    if (!bDirectionHeld)
                    {
                        nCurrentX += (DoesPieceFit(tetromino[nCurrentPiece], pField, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
                        bDirectionHeld = true;
                    }
                    else
                    {
                        if (nDASCounter < nDAS)
                            nDASCounter++;
                        else
                        {
                            nCurrentX += (DoesPieceFit(tetromino[nCurrentPiece], pField, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
                        }
                    }

                    nProjectedY = nCurrentY;
                    while (DoesPieceFit(tetromino[nCurrentPiece], pField, nCurrentRotation, nCurrentX, nProjectedY))
                        nProjectedY++;
                    nProjectedY--;
                }
                
                // Down arrow pressed
                if (bKey[KEY_D_ARROW])
                {
                    if (DoesPieceFit(tetromino[nCurrentPiece], pField, nCurrentRotation, nCurrentX, nCurrentY + 1))
                    {
                        nCurrentY++;
                        nDistPushed++;
                    }
                    else
                        bForceDown = true;
                }

                // Up arrow pressed
                if (bKey[KEY_U_ARROW] && ! bRotateHold)
                {
                    if (nCurrentPiece != PI_BLOCK)
                    {
                        nCurrentRotation++;
                        int nXAttempt = nCurrentX;
                        int nYAttempt = nCurrentY;
                        while (!DoesPieceFit(tetromino[nCurrentPiece], pField, nCurrentRotation, nXAttempt, nYAttempt))
                        {
                            if (nXAttempt < FIELD_WIDTH / 2)
                            {
                                nXAttempt++;
                                if (nXAttempt >= FIELD_WIDTH / 2)
                                {
                                    nXAttempt = nCurrentX;
                                    nCurrentRotation--;
                                    break;
                                }
                            }
                            else
                            {
                                nXAttempt--;
                                if (nXAttempt <= FIELD_WIDTH / 2)
                                {
                                    nXAttempt = nCurrentX;
                                    nCurrentRotation--;
                                    break;
                                }
                            }
                        }
                        nCurrentX = nXAttempt;
                        nProjectedY = nCurrentY;
                        while (DoesPieceFit(tetromino[nCurrentPiece], pField, nCurrentRotation, nCurrentX, nProjectedY))
                            nProjectedY++;
                        nProjectedY--;
                    }
                }

                bRotateHold = bKey[KEY_U_ARROW];

                // Esc pressed
                if (bKey[KEY_ESC] && !bPauseHold)
                {
                    bPauseHold = true;
                    gState = GS_PAUSE;
                }
                else if (!bKey[KEY_ESC])
                    bPauseHold = false;
                bPlaying = !bGameOver;

                // Q pressed
                if (bKey[KEY_Q])
                {
                    bGameOver = true;
                    bPlaying = false;
                }

                // Piece forced down
                if (bForceDown)
                {
                    if (DoesPieceFit(tetromino[nCurrentPiece], pField, nCurrentRotation, nCurrentX, nCurrentY + 1))
                        nCurrentY++;
                    else
                    {
                        // Lock the current piece in the field
                        for (int px = 0; px < NUM_TETROMINO_COLS; px++)
                            for (int py = 0; py < NUM_TETROMINO_LINES; py++)
                                if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
                                    pField[(nCurrentY + py) * FIELD_WIDTH + (nCurrentX + px)] = nCurrentPiece + 1;


                        // Check if we have any lines
                        for (int py = 0; py < NUM_TETROMINO_LINES; py++)
                            if (nCurrentY + py < FIELD_HEIGHT - 1)
                            {
                                bool bLine = true;
                                for (int px = 1; px < FIELD_WIDTH - 1; px++)
                                    bLine &= (pField[(nCurrentY + py) * FIELD_WIDTH + px]) != 0;

                                if (bLine)
                                {
                                    for (int px = 1; px < FIELD_WIDTH - 1; px++)
                                        pField[(nCurrentY + py) * FIELD_WIDTH + px] = 8;

                                    vLines.push_back(nCurrentY + py);

                                    nLineCount++;
                                }
                            }
                        int nMultiplier = nDifficulty + 1;
                        switch (vLines.size())
                        {
                            
                        case 4:
                            nScore += 1200 * nMultiplier;
                            break;
                        case 3:
                            nScore += 300 * nMultiplier;
                            break;
                        case 2:
                            nScore += 100 * nMultiplier;
                            break;
                        case 1:
                            nScore += 40 * nMultiplier;
                            break;
                        default:
                            break;
                        }

                        nScore += nDistPushed;

                        if (nScore > nHighScore)
                            nHighScore = nScore;

                        NextPieceToCurrent(nNextPiece, nCurrentPiece);
                        ResetCurrentPiece(nCurrentX, nCurrentY, nCurrentRotation, nDistPushed, nProjectedY, tetromino[nCurrentPiece], pField);

                        if (!DoesPieceFit(tetromino[nCurrentPiece], pField, nCurrentRotation, nCurrentX, nCurrentY))
                        {
                            bGameOver = true;
                            gState = GS_GAME_OVER;
                        }
                    }

                    nSpeedCounter = 0;
                }

                // RENDER OUTPUT ===================================================

                // Draw Field
                for (int x = 0; x < FIELD_WIDTH; x++)
                    for (int y = 0; y < FIELD_HEIGHT; y++)
                        screen[(y + STARTING_OFFSET) * NUM_SCREEN_COLS + (x + STARTING_OFFSET)] = L" \u2588\u2588\u2588\u2588\u2588\u2588\u2588\u2593"[pField[y * FIELD_WIDTH + x]];
                
                // Draw Current Piece
                DrawPiece(tetromino[nCurrentPiece], nCurrentRotation, nCurrentX, nProjectedY, screen, L'\u25a1');
                DrawPiece(tetromino[nCurrentPiece], nCurrentRotation, nCurrentX, nCurrentY, screen, L'\u2588');

                // Draw UI
                swprintf_s(&screen[2 * NUM_SCREEN_COLS + FIELD_WIDTH + 6], 16, L"SCORE: %8d", nScore);
                swprintf_s(&screen[4 * NUM_SCREEN_COLS + FIELD_WIDTH + 6], 5, L"HiGH");
                swprintf_s(&screen[5 * NUM_SCREEN_COLS + FIELD_WIDTH + 6], 16, L"SCORE: %8d", nHighScore);
                swprintf_s(&screen[7 * NUM_SCREEN_COLS + FIELD_WIDTH + 6], 12, L"NEXT PIECE:");
                swprintf_s(&screen[18 * NUM_SCREEN_COLS + FIELD_WIDTH + 6], 10, L"LEVEL: %02d", nDifficulty);
                
                // Show FPS if debug
                if(DEBUG)
                    swprintf_s(&screen[20 * NUM_SCREEN_COLS + FIELD_WIDTH + 6], 11, L"FPS: %02.02f", fps);

                // Draw Next Piece border
                for (int px = 0; px < 8; px++)
                    for (int py = 0; py < 8; py++)
                        if (nextPieceBorder[px + py * 8] == L'X')
                            screen[(7 + py + STARTING_OFFSET) * NUM_SCREEN_COLS + (FIELD_WIDTH + 4 + px + STARTING_OFFSET)] = L'\u2593';
                        else if (nextPieceBorder[px + py * 8] == L' ')
                            screen[(7 + py + STARTING_OFFSET) * NUM_SCREEN_COLS + (FIELD_WIDTH + 4 + px + STARTING_OFFSET)] = L' ';
                
                // Draw Next Piece
                DrawPiece(tetromino[nNextPiece], 0, FIELD_WIDTH + 6, 9, screen, L'\u2588', L' ');

                // Lines cleared?
                if (!vLines.empty())
                {
                    // Disaplay Frame
                    WriteConsoleOutputCharacter(hConsole, screen, NUM_SCREEN_COLS * NUM_SCREEN_ROWS, { 0,0 }, &dwBytesWritten);
                    
                    // Pause for animation timing
                    this_thread::sleep_for(400ms);

                    // Animation pt 1
                    for (auto& v : vLines)
                        for (int px = 1; px < FIELD_WIDTH - 1; px++)
                        {
                            screen[(v + STARTING_OFFSET) * NUM_SCREEN_COLS + (px + STARTING_OFFSET)] = L'\u2592';
                        }

                    // Disaplay Frame
                    WriteConsoleOutputCharacter(hConsole, screen, NUM_SCREEN_COLS * NUM_SCREEN_ROWS, { 0,0 }, &dwBytesWritten);
                    
                    // Pause for animation timing
                    this_thread::sleep_for(50ms);

                    // Animation pt 2

                    for (auto& v : vLines)
                        for (int px = 1; px < FIELD_WIDTH - 1; px++)
                        {
                            screen[(v + STARTING_OFFSET) * NUM_SCREEN_COLS + (px + STARTING_OFFSET)] = L'\u2591';
                        }

                    // Disaplay Frame
                    WriteConsoleOutputCharacter(hConsole, screen, NUM_SCREEN_COLS * NUM_SCREEN_ROWS, { 0,0 }, &dwBytesWritten);
                    
                    // Pause for animation timing
                    this_thread::sleep_for(50ms);

                    // Shift lines down
                    for (auto& v : vLines)
                        for (int px = 1; px < FIELD_WIDTH - 1; px++)
                        {
                            for (int py = v; py > 0; py--)
                                pField[py * FIELD_WIDTH + px] = pField[(py - 1) * FIELD_WIDTH + px];
                            pField[px] = 0;
                        }

                    // Clear out lines vector
                    vLines.clear();
                }
            }

            // Disaplay Frame
            WriteConsoleOutputCharacter(hConsole, screen, NUM_SCREEN_COLS * NUM_SCREEN_ROWS, { 0,0 }, &dwBytesWritten);
        }

        // GAME OVER ==========================================================
        if (gState == GS_GAME_OVER)
        {
            if (nHighScore > nStoredScore)
            {
                nHighScore = nScore;
                score_file.open("cTetScore.txt", ios::out);

                if (score_file)
                {
                    score_file << nHighScore << '\n';

                    score_file.close();

                    nStoredScore = nHighScore;
                }
            }

            if (nScore < MAX_SCORE)
            {
                AnimateStar(gameOverSreen);

                FillScreen(screen, gameOverSreen);
            }
            else
            {
                FillScreen(screen, maxOutScreen);
            }

            if (bKey[KEY_R])
            {
                clearScreen(screen);

                setupField(pField);

                gState = GS_PLAYING;

                bGameOver = false;

                nScore = 0;

                nLineCount = 0;
                nLineCountLast = 0;

                nDifficulty = nSelectedDifficulty;

                RandomizePieces(nNextPiece, nCurrentPiece);
                ResetCurrentPiece(nCurrentX, nCurrentY, nCurrentRotation, nDistPushed, nProjectedY,tetromino[nCurrentPiece], pField);

                ResetTimers(timer, timer_last, fps_timer_last);
            }

            if (bKey[KEY_Q])
            {
                bPlaying = false;
            }

            if (bKey[KEY_M])
            {
                gState = GS_MAIN_MENU;

                clearScreen(screen);
                
                bGameOver = false;
            }
        }
            
        // Disaplay Frame
        WriteConsoleOutputCharacter(hConsole, screen, NUM_SCREEN_COLS * NUM_SCREEN_ROWS, { 0,0 }, &dwBytesWritten);
    }
    return 0;
}