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

#ifndef NUM_TETROMINOES
#define NUM_TETROMINOES 7
#define NUM_TETROMINO_LINES 4
#define NUM_TETROMINO_COLS 4
#define STARTING_OFFSET 2
#define NUM_SCREEN_COLS 34
#define NUM_SCREEN_ROWS 26
#define NUM_CONTROL_KEYS 20
#define MAX_SCORE 999999
#endif // !1

#ifndef DEBUG
#define DEBUG true
#endif // !DEBUG


enum Rotations
{
    ROT_0 = 0,
    ROT_90,
    ROT_180,
    ROT_270,
    ROT_NUM_ROT
};

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

int Rotate(int px, int py, int r)
{
    switch (r % ROT_NUM_ROT)
    {
    case ROT_0: return (py * NUM_TETROMINO_COLS) + px;      // 0   degrees
    case ROT_90: return 12 + py - (NUM_TETROMINO_LINES * px); // 90  degrees
    case ROT_180: return 15 - (py * NUM_TETROMINO_COLS) - px; // 180 degrees
    case ROT_270: return 3 - py + (NUM_TETROMINO_LINES * px);  // 2NUM_TETROMINOES0 degrees
    default: return 0;
    }
}


wstring tetromino[NUM_TETROMINOES];
int nFieldWidth = 12;
int nFieldHeight = 22;
int nSH = NUM_SCREEN_ROWS;
int nSW = NUM_SCREEN_COLS;
unsigned char* pField = nullptr;
wstring titleSreen;
wstring difficultySreen;
wstring gameOverSreen;
wstring pausedSreen;
wstring maxOutScreen;

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
    for (int px = 0; px < NUM_TETROMINO_COLS; px++)
    {
        for (int py = 0; py < NUM_TETROMINO_LINES; py++)
        {
            // Get index into piece
            int pi = Rotate(px, py, nRotation);

            // Get index into field
            int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

            if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
            {
                if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
                {
                    if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
                        return false;
                }
            }

        }
    }

    return true;
}
int main()
{
    srand(time(0));

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
    gameOverSreen.append(L"# *    *    *    *    *   *   *  #");
    gameOverSreen.append(L"#  *  *  *  *  *  *  *  *  *  *  #");
    gameOverSreen.append(L"# * * * * * * * * * * * * * * *  #");
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

    pField = new unsigned char[nFieldWidth * nFieldHeight];
    
    auto setupField = []() {
        for (int x = 0; x < nFieldWidth; x++)
            for (int y = 0; y < nFieldHeight; y++)
                pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 8 : 0;
    };


    wchar_t* screen = new wchar_t[nSW * nSH];
    Console::SetWindowSize(nSW, nSH);
    SetConsoleTitle(L"Console TETRIS");
    for (int i = 0; i < nSW * nSH; i++) screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);

    auto clearScreen = [] (wchar_t* screen){
        for (int x = 0; x < nSW; x++)
            for (int y = 0; y < nSH; y++)
                screen[y * nSW + x] = L' ';
    };

    DWORD dwBytesWritten = 0;

    bool bGameOver = false;

    int  nCurrentPiece = rand() % NUM_TETROMINOES;
    int  nNextPiece = rand() % NUM_TETROMINOES;
    int  nCurrentRotation = 0;
    int  nCurrentX = (nFieldWidth / 2) - STARTING_OFFSET;
    int  nCurrentY = 0;
    bool bKey[NUM_CONTROL_KEYS];
    bool bRotateHold = false;
    bool bPauseHold = false;
    bool bStartHold = false;
    int  nDAS = 10;
    int  nDASCounter = 0;
    bool bDirectionHeld = false;

    int nSpeed = 20;
    int nDiffuculty = 0;
    int nSpeedCounter = 0;
    bool bForceDown = false;
    int nLineCount = 0;
    int nLineCountLast = 0;
    int nDistPushed;

    high_resolution_clock::time_point timer;
    high_resolution_clock::time_point timer_last;
    high_resolution_clock::time_point fps_timer_last;
    int fps_counter = 0;
    float fps = 60.0f;

    int nScore = 0;
    int nInitialLevelTransition;

    bool bPlaying = true;

    fstream score_file;
    score_file.open("cTetScore.txt", ios::in);

    int nStoredScore = 0;
    int nHighScore = 0;
    if (score_file)
    {
        string scoreStr;
        getline(score_file, scoreStr);
        score_file.close();

        if (scoreStr != "\0")
            nStoredScore = std::stoi(scoreStr);

        nHighScore = nStoredScore;
    }
    
    enum GameState
    {
        GS_MAIN_MENU = 0,
        GS_DIFFICULTY_SELECTION,
        GS_PLAYING,
        GS_PAUSE,
        GS_RESTARTING,
        GS_GAME_OVER
    };

    vector<int> vLines;
    GameState gState = GS_MAIN_MENU;

    PlaySound(L"C:/Tetris.mp3", 0, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);

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
    
    auto ResetCurrentPiece = [](int& nCurrentX,
                                int& nCurrentY, 
                                int& nCurrentRotation, 
                                int& nDistPushed)
    {
        // Choose next piece
        nCurrentX = (nFieldWidth / 2) - STARTING_OFFSET;
        nCurrentY = 0;
        nCurrentRotation = 0;
        nDistPushed = 0;
    };

    auto NextPieceToCurrent = [](int& nNextPiece, int& nCurrentPiece) 
    {
        nCurrentPiece = nNextPiece;
        nNextPiece = rand() % NUM_TETROMINOES;
    };

    auto RandomizePieces = [](int& nNextPiece, 
                              int& nCurrentPiece)
    {
        nCurrentPiece = rand() % NUM_TETROMINOES;
        nNextPiece = rand() % NUM_TETROMINOES;
    };

    auto GetInputs = [](bool (&bKeys)[NUM_CONTROL_KEYS]) {
        for (int k = 0; k < NUM_CONTROL_KEYS; k++)
            //                                                     ->  <-   V   ^  esc spc  Q   R   M   0   1   2   3   4   5   6   7   8   9  SHIFT
            bKeys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26\x1b\x20\x51\x52\x4d\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x10"[k]))) != 0;
    };

    auto FillScreen = []() {};
    auto DrawShape = []() {};

    auto ResetTimers = [](high_resolution_clock::time_point& timer,
                          high_resolution_clock::time_point& timer_last,
                          high_resolution_clock::time_point& fps_timer_last) {
        timer = high_resolution_clock::now(); 
        timer_last = high_resolution_clock::now();
        fps_timer_last = high_resolution_clock::now();
    };

    auto AnimateStar = [](wstring& Screen)
    {
        static int animTimer = 0;

        if (animTimer >= 10)
        {
            for (int x = 0; x < nSW; x++)
                for (int y = 0; y < nSH; y++)
                {
                    int index = y * nSW + x;
                    if (Screen[index] == L'*')
                        Screen[y * nSW + x] = L'\u2736';
                    else if (Screen[index] == L'\u2736')
                        Screen[y * nSW + x] = L'*';
                }
            animTimer = 0;
        }
        else
            animTimer++;
    };

    while (bPlaying)
    {
        // GAME TIMING =====================================================
        this_thread::sleep_for(50ms);

        // INPUT ===========================================================
        GetInputs(bKey);

        if (gState == GS_MAIN_MENU)
        {
            for (int x = 0; x < nSW; x++)
                for (int y = 0; y < nSH; y++)
                    screen[y * nSW + x] = titleSreen[y * nSW + x];

            if (bKey[KEY_SPACE] && !bStartHold)
            {
                clearScreen(screen);

                gState = GS_DIFFICULTY_SELECTION;

                bStartHold = true;
            }
            else if (!bKey[KEY_SPACE])
                bStartHold = false;

            bGameOver = bKey[KEY_Q];
            bPlaying = !bGameOver;
        }

        if (gState == GS_DIFFICULTY_SELECTION)
        {
            AnimateStar(difficultySreen);

            for (int x = 0; x < nSW; x++)
                for (int y = 0; y < nSH; y++)
                    screen[y * nSW + x] = difficultySreen[y * nSW + x];

            swprintf_s(&screen[16 * nSW + 19], 3, L"%02d", nDiffuculty);


            // Note can be switch
            if (bKey[KEY_0])
                nDiffuculty = 0 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_1])
                nDiffuculty = 1 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_2])
                nDiffuculty = 2 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_3])
                nDiffuculty = 3 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_4])
                nDiffuculty = 4 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_5])
                nDiffuculty = 5 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_6])
                nDiffuculty = 6 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_7])
                nDiffuculty = 7 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_8])
                nDiffuculty = 8 + (bKey[KEY_SHIFT] ? 10 : 0);
            else if (bKey[KEY_9])
                nDiffuculty = 9 + (bKey[KEY_SHIFT] ? 10 : 0);

            if (bKey[KEY_SPACE] && !bStartHold)
            {
                clearScreen(screen);

                setupField();

                gState = GS_PLAYING;

                bStartHold = true;

                nScore = 0;
                nLineCount = 0;
                nLineCountLast = 0;

                // Choose next piece
                nCurrentX = (nFieldWidth / 2) - STARTING_OFFSET;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = rand() % NUM_TETROMINOES;
                nNextPiece = rand() % NUM_TETROMINOES;
                nDistPushed = 0;
                nSpeed = FindSpeed(nDiffuculty);

                nInitialLevelTransition = max(100, nDiffuculty * 10 - 50);

                ResetTimers(timer, timer_last, fps_timer_last);
            }
            else if(!bKey[KEY_SPACE])
                bStartHold = false;

            bGameOver = bKey[KEY_Q];
            bPlaying = !bGameOver;
        }
        
        while (!bGameOver && (gState == GS_PLAYING || gState == GS_PAUSE))
        {
            
            // GAME TIMING =====================================================
            duration<double, std::milli> time_span = timer - timer_last;
            this_thread::sleep_for(14.1ms - (time_span));
            timer_last = high_resolution_clock::now();

            fps_counter++;
            duration<double, std::milli> time_span_fps = timer - fps_timer_last;
            if (time_span_fps >= 1000ms)
            {
                fps = (fps_counter / time_span_fps.count()) * 1000.0f;
                fps_counter = 0;
                fps_timer_last = high_resolution_clock::now();
            }

            // INPUT ===========================================================
            GetInputs(bKey);

            if (gState == GS_PAUSE)
            {
                for (int x = 0; x < nSW; x++)
                    for (int y = 0; y < nSH; y++)
                        screen[y * nSW + x] = pausedSreen[y * nSW + x];

                if (bKey[KEY_ESC] && !bPauseHold)
                {
                    clearScreen(screen);

                    gState = GS_PLAYING;

                    bPauseHold = true;
                }
                else if (!bKey[KEY_ESC])
                {
                    bPauseHold = false;
                }

                if (bKey[KEY_R])
                {
                    clearScreen(screen);

                    setupField();

                    gState = GS_PLAYING;

                    bGameOver = false;

                    nScore = 0;

                    // Choose next piece
                    nCurrentX = (nFieldWidth / 2) - STARTING_OFFSET;
                    nCurrentY = 0;
                    nCurrentRotation = 0;
                    nCurrentPiece = rand() % NUM_TETROMINOES;
                    nNextPiece = rand() % NUM_TETROMINOES;
                    nDistPushed = 0;

                    ResetTimers(timer, timer_last, fps_timer_last);
                }

                if (bKey[KEY_Q])
                {
                    bGameOver = true;
                    bPlaying = false;
                }

                if (bKey[KEY_M])
                {
                    gState = GS_MAIN_MENU;

                    clearScreen(screen);

                    bGameOver = false;
                }
            }
            else 
            {
                if (nLineCount >= nInitialLevelTransition && nLineCount != nLineCountLast && nLineCount % 10)
                {
                    nDiffuculty++;
                    nSpeed = FindSpeed(nDiffuculty);
                }

                nLineCountLast = nLineCount;

                nSpeedCounter++;

                bForceDown = (nSpeedCounter >= nSpeed);

                // GAME LOGIC ======================================================
                if (!bKey[KEY_L_ARROW] && !bKey[KEY_R_ARROW] && !bKey[KEY_U_ARROW] && !bKey[KEY_D_ARROW])
                {
                    bDirectionHeld = false;
                    nDASCounter = nDASCounter >= 0 ? (nDASCounter - 1) : 0;
                }

                if (bKey[KEY_L_ARROW])
                {
                    if (!bDirectionHeld)
                    {
                        nCurrentX -= (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
                        bDirectionHeld = true;
                    }
                    else
                    {
                        if (nDASCounter < nDAS)
                            nDASCounter++;
                        else
                        {
                            nCurrentX -= (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
                        }
                    }
                }

                if (bKey[KEY_R_ARROW])
                {
                    if (!bDirectionHeld)
                    {
                        nCurrentX += (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
                        bDirectionHeld = true;
                    }
                    else
                    {
                        if (nDASCounter < nDAS)
                            nDASCounter++;
                        else
                        {
                            nCurrentX += (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
                        }
                    }
                }
                
                if (bKey[KEY_D_ARROW])
                {
                    if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                    {
                        nCurrentY++;
                        nDistPushed++;
                    }
                    else
                        bForceDown = true;
                }
                nCurrentRotation += (bKey[3] && !bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;

                bRotateHold = bKey[3];

                if (bKey[KEY_ESC] && !bPauseHold)
                {
                    bPauseHold = true;
                    gState = GS_PAUSE;
                }
                else if (!bKey[KEY_ESC])
                    bPauseHold = false;
                bPlaying = !bGameOver;

                if (bKey[KEY_Q])
                {
                    bGameOver = true;
                    bPlaying = false;
                }

                if (bForceDown)
                {
                    if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                        nCurrentY++;
                    else
                    {
                        // Lock the current piece in the field
                        for (int px = 0; px < NUM_TETROMINO_COLS; px++)
                            for (int py = 0; py < NUM_TETROMINO_LINES; py++)
                                if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
                                    pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;


                        // Check if we have any lines
                        for (int py = 0; py < NUM_TETROMINO_LINES; py++)
                            if (nCurrentY + py < nFieldHeight - 1)
                            {
                                bool bLine = true;
                                for (int px = 1; px < nFieldWidth - 1; px++)
                                    bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

                                if (bLine)
                                {
                                    for (int px = 1; px < nFieldWidth - 1; px++)
                                        pField[(nCurrentY + py) * nFieldWidth + px] = 8;

                                    vLines.push_back(nCurrentY + py);

                                    nLineCount++;
                                }
                            }
                        int nMultiplier = nDiffuculty + 1;
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

                        // Choose next piece
                        nCurrentX = (nFieldWidth / 2) - STARTING_OFFSET;
                        nCurrentY = 0;
                        nCurrentRotation = 0;
                        nCurrentPiece = nNextPiece;
                        nNextPiece = rand() % NUM_TETROMINOES;
                        nDistPushed = 0;

                        if (!DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY))
                        {
                            bGameOver = true;
                            gState = GS_GAME_OVER;
                        }
                    }

                    nSpeedCounter = 0;
                }

                // RENDER OUTPUT ===================================================


                // Draw Field
                for (int x = 0; x < nFieldWidth; x++)
                    for (int y = 0; y < nFieldHeight; y++)
                        screen[(y + STARTING_OFFSET) * nSW + (x + STARTING_OFFSET)] = L" \u2588\u2588\u2588\u2588\u2588\u2588\u2588\u2593"[pField[y * nFieldWidth + x]];
                
                // Draw Current Piece
                for (int px = 0; px < NUM_TETROMINO_COLS; px++)
                    for (int py = 0; py < NUM_TETROMINO_LINES; py++)
                        if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
                            screen[(nCurrentY + py + STARTING_OFFSET) * nSW + (nCurrentX + px + STARTING_OFFSET)] = L"\u2588\u2588\u2588\u2588\u2588\u2588\u2588"[nCurrentPiece];

                swprintf_s(&screen[2 * nSW + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);
                swprintf_s(&screen[4 * nSW + nFieldWidth + 6], 5, L"HiGH");
                swprintf_s(&screen[5 * nSW + nFieldWidth + 6], 16, L"SCORE: %8d", nHighScore);

                swprintf_s(&screen[7 * nSW + nFieldWidth + 6], 12, L"NEXT PIECE:");
                swprintf_s(&screen[17 * nSW + nFieldWidth + 6], 10, L"LEVEL: %02d", nDiffuculty);
                
                if(DEBUG)
                    swprintf_s(&screen[19 * nSW + nFieldWidth + 6], 11, L"FPS: %02.02f", fps);

                // Draw Current Piece
                for (int px = 0; px < NUM_TETROMINO_COLS; px++)
                    for (int py = 0; py < NUM_TETROMINO_LINES; py++)
                        if (tetromino[nNextPiece][Rotate(px, py, 0)] == L'X')
                            screen[(9 + py + STARTING_OFFSET) * nSW + (nFieldWidth + 6 + px + STARTING_OFFSET)] = L"\u2588\u2588\u2588\u2588\u2588\u2588\u2588"[nNextPiece];
                        else
                            screen[(9 + py + STARTING_OFFSET) * nSW + (nFieldWidth + 6 + px + STARTING_OFFSET)] = L' ';

                if (!vLines.empty())
                {
                    // Disaplay Frame
                    WriteConsoleOutputCharacter(hConsole, screen, nSW * nSH, { 0,0 }, &dwBytesWritten);
                    this_thread::sleep_for(400ms);

                    for (auto& v : vLines)
                        for (int px = 1; px < nFieldWidth - 1; px++)
                        {
                            screen[(v + STARTING_OFFSET) * nSW + (px + STARTING_OFFSET)] = L'\u2592';
                        }
                    WriteConsoleOutputCharacter(hConsole, screen, nSW * nSH, { 0,0 }, &dwBytesWritten);
                    this_thread::sleep_for(50ms);

                    for (auto& v : vLines)
                        for (int px = 1; px < nFieldWidth - 1; px++)
                        {
                            screen[(v + STARTING_OFFSET) * nSW + (px + STARTING_OFFSET)] = L'\u2591';
                        }
                    WriteConsoleOutputCharacter(hConsole, screen, nSW * nSH, { 0,0 }, &dwBytesWritten);
                    this_thread::sleep_for(50ms);

                    for (auto& v : vLines)
                        for (int px = 1; px < nFieldWidth - 1; px++)
                        {
                            for (int py = v; py > 0; py--)
                                pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
                            pField[px] = 0;
                        }
                    vLines.clear();
                }
            }

            WriteConsoleOutputCharacter(hConsole, screen, nSW * nSH, { 0,0 }, &dwBytesWritten);

            timer = high_resolution_clock::now();

        }

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

                for (int x = 0; x < nSW; x++)
                    for (int y = 0; y < nSH; y++)
                        screen[y * nSW + x] = gameOverSreen[y * nSW + x];
            }
            else
            {
                for (int x = 0; x < nSW; x++)
                    for (int y = 0; y < nSH; y++)
                        screen[y * nSW + x] = maxOutScreen[y * nSW + x];
            }

            if (bKey[KEY_R])
            {
                clearScreen(screen);

                setupField();

                gState = GS_PLAYING;

                bGameOver = false;

                nScore = 0;

                nLineCount = 0;
                nLineCountLast = 0;

                // Choose next piece
                nCurrentX = (nFieldWidth / 2) - STARTING_OFFSET;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = rand() % NUM_TETROMINOES;
                nNextPiece = rand() % NUM_TETROMINOES;
                nDistPushed = 0;

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
            
        WriteConsoleOutputCharacter(hConsole, screen, nSW * nSH, { 0,0 }, &dwBytesWritten);
    }
    return 0;
}