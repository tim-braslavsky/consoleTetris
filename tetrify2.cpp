#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <vector>
#pragma comment(lib,"Winmm.lib")

using namespace std;
using namespace System;

#ifndef NUM_TETROMINOES
#define NUM_TETROMINOES 7
#define NUM_TETROMINO_LINES 4
#define NUM_TETROMINO_COLS 4
#define STARTING_OFFSET 2
#define NUM_SCREEN_COLS 34
#define NUM_SCREEN_ROWS 26
#endif // !1

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
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#            CONTROLS            #");
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
    difficultySreen.append(L"#     PRESS SPACE TO START       #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"==================================");

    gameOverSreen.append(L"==================================");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#     ####   ##   #   #  ###     #");
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
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#       PRESS R TO RETRY         #");
    gameOverSreen.append(L"#       PRESS M FOR MAIN MENU    #");
    gameOverSreen.append(L"#       PRESS Q TO QUIT          #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"==================================");

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
    bool bKey[20];
    bool bRotateHold = false;
    bool bPauseHold = false;
    bool bStartHold = false;

    int nSpeed = 20;
    int nDiffuculty = 0;
    int nSpeedCounter = 0;
    bool bForceDown = false;
    int nLineCount = 0;
    int nScore = 0;

    bool bPlaying = true;
    
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

    while (bPlaying)
    {
        // GAME TIMING =====================================================
        this_thread::sleep_for(50ms);

        // INPUT ===========================================================
        for (int k = 0; k < sizeof(bKey); k++)
            //                                                     ->  <-   V   ^  esc spc  Q   R   M   0   1   2   3   4   5   6   7   8   9  SHIFT
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26\x1b\x20\x51\x52\x4d\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x10"[k]))) != 0;
        
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
            for (int x = 0; x < nSW; x++)
                for (int y = 0; y < nSH; y++)
                    screen[y * nSW + x] = difficultySreen[y * nSW + x];

            swprintf_s(&screen[16 * nSW + 19], 3, L"%02d", nDiffuculty);

            if (bKey[KEY_SPACE] && !bStartHold)
            {
                clearScreen(screen);

                setupField();

                gState = GS_PLAYING;

                bStartHold = true;

                nScore = 0;
            }
            else if(!bKey[KEY_SPACE])
                bStartHold = false;

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


            bGameOver = bKey[KEY_Q];
            bPlaying = !bGameOver;
        }
        
        while (!bGameOver && (gState == GS_PLAYING || gState == GS_PAUSE))
        {
            // GAME TIMING =====================================================
            this_thread::sleep_for(16.66ms);

            // INPUT ===========================================================
            for (int k = 0; k < sizeof(bKey); k++)
                //                                                     ->  <-   V   ^  esc spc  Q   R   M
                bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26\x1b\x20\x51\x52\x4d"[k]))) != 0;

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
                if (nLineCount >= 10)
                {
                    if (nSpeed >= 1) nDiffuculty++;
                    nLineCount = nLineCount % 10;
                }
                nSpeedCounter++;
                bForceDown = (nSpeedCounter >= (nSpeed - nDiffuculty));

                // GAME LOGIC ======================================================
                nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
                nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
                nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

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
                        int nMultiplier = (nDiffuculty / 2) > 0 ? (nDiffuculty / 2) : 1;
                        switch (vLines.size())
                        {
                            
                        case 4:
                            nScore += 1000 * nMultiplier;
                            break;
                        case 3:
                            nScore += 500 * nMultiplier;
                            break;
                        case 2:
                            nScore += 250 * nMultiplier;
                            break;
                        case 1:
                            nScore += 100 * nMultiplier;
                            break;
                        default:
                            break;
                        }


                        // Choose next piece
                        nCurrentX = (nFieldWidth / 2) - STARTING_OFFSET;
                        nCurrentY = 0;
                        nCurrentRotation = 0;
                        nCurrentPiece = nNextPiece;
                        nNextPiece = rand() % NUM_TETROMINOES;

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

                swprintf_s(&screen[4 * nSW + nFieldWidth + 6], 12, L"NEXT PIECE:");

                // Draw Current Piece
                for (int px = 0; px < NUM_TETROMINO_COLS; px++)
                    for (int py = 0; py < NUM_TETROMINO_LINES; py++)
                        if (tetromino[nNextPiece][Rotate(px, py, 0)] == L'X')
                            screen[(6 + py + STARTING_OFFSET) * nSW + (nFieldWidth + 6 + px + STARTING_OFFSET)] = L"\u2588\u2588\u2588\u2588\u2588\u2588\u2588"[nNextPiece];
                        else
                            screen[(6 + py + STARTING_OFFSET) * nSW + (nFieldWidth + 6 + px + STARTING_OFFSET)] = L' ';

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
        }

        if (gState == GS_GAME_OVER)
        {
            for (int x = 0; x < nSW; x++)
                for (int y = 0; y < nSH; y++)
                    screen[y * nSW + x] = gameOverSreen[y * nSW + x];

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