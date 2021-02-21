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
#define NUM_SCREEN_ROWS 22
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
int nFieldHeight = 18;
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
    titleSreen.append(L"#                                #");
    titleSreen.append(L"#        []   [][]   []  []      #");
    titleSreen.append(L"#        []   [][]   []  []      #");
    titleSreen.append(L"#        [][]      [][]  []      #");
    titleSreen.append(L"#                        []      #");
    titleSreen.append(L"#         []   []     []         #");
    titleSreen.append(L"#         [][] [][] [][]         #");
    titleSreen.append(L"#           [] []   []           #");
    titleSreen.append(L"#                                #");
    titleSreen.append(L"==================================");

    difficultySreen.append(L"==================================");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#            CONSOLE             #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#     ### ## ### ### ### ###     #");
    difficultySreen.append(L"#      #  #   #  # #  #  #       #");
    difficultySreen.append(L"#      #  ##  #  ##   #  ###     #");
    difficultySreen.append(L"#      #  #   #  # #  #    #     #");
    difficultySreen.append(L"#      #  ##  #  # # ### ###     #");
    difficultySreen.append(L"#     ======================     #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#      PRESS SPACE TO START      #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"#        []   [][]   []  []      #");
    difficultySreen.append(L"#        []   [][]   []  []      #");
    difficultySreen.append(L"#        [][]      [][]  []      #");
    difficultySreen.append(L"#                        []      #");
    difficultySreen.append(L"#         []   []     []         #");
    difficultySreen.append(L"#         [][] [][] [][]         #");
    difficultySreen.append(L"#           [] []   []           #");
    difficultySreen.append(L"#                                #");
    difficultySreen.append(L"==================================");

    gameOverSreen.append(L"==================================");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#            CONSOLE             #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#     ### ## ### ### ### ###     #");
    gameOverSreen.append(L"#      #  #   #  # #  #  #       #");
    gameOverSreen.append(L"#      #  ##  #  ##   #  ###     #");
    gameOverSreen.append(L"#      #  #   #  # #  #    #     #");
    gameOverSreen.append(L"#      #  ##  #  # # ### ###     #");
    gameOverSreen.append(L"#     ======================     #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#      PRESS SPACE TO START      #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"#        []   [][]   []  []      #");
    gameOverSreen.append(L"#        []   [][]   []  []      #");
    gameOverSreen.append(L"#        [][]      [][]  []      #");
    gameOverSreen.append(L"#                        []      #");
    gameOverSreen.append(L"#         []   []     []         #");
    gameOverSreen.append(L"#         [][] [][] [][]         #");
    gameOverSreen.append(L"#           [] []   []           #");
    gameOverSreen.append(L"#                                #");
    gameOverSreen.append(L"==================================");

    pausedSreen.append(L"==================================");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#       ____                     #");
    pausedSreen.append(L"#      /   /                     #");
    pausedSreen.append(L"#     /___/ ___       __  __ _   #");
    pausedSreen.append(L"#    /     /__/ /  / /_  /_ / \\  #");
    pausedSreen.append(L"#   /     /  / /__/ __/ /_ /__/  #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#     PRESS ESC TO RESUME        #");
    pausedSreen.append(L"#     PRESS R TO RESTART         #");
    pausedSreen.append(L"#     PRESS M FOR MAIN MENU      #");
    pausedSreen.append(L"#     PRESS Q TO QUIT            #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"#                                #");
    pausedSreen.append(L"==================================");

    pField = new unsigned char[nFieldWidth * nFieldHeight];
    for (int x = 0; x < nFieldWidth; x++)
        for (int y = 0; y < nFieldHeight; y++)
            pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 8 : 0;

    wchar_t* screen = new wchar_t[nSW * nSH];
    Console::SetWindowSize(nSW, nSH);
    for (int i = 0; i < nSW * nSH; i++) screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);


    DWORD dwBytesWritten = 0;

    bool bGameOver = false;

    int  nCurrentPiece = rand() % NUM_TETROMINOES;
    int  nNextPiece = rand() % NUM_TETROMINOES;
    int  nCurrentRotation = 0;
    int  nCurrentX = nFieldWidth / 2;
    int  nCurrentY = 0;
    bool bKey[6];
    bool bRotateHold = false;
    bool bPauseHold = false;

    int nSpeed = 20;
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

    while (bPlaying)
    {
        // GAME TIMING =====================================================
        this_thread::sleep_for(50ms);

        // INPUT ===========================================================
        for (int k = 0; k < sizeof(bKey); k++)
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26\x1b\x20"[k]))) != 0;
        
        if (gState == GS_MAIN_MENU)
        {
            for (int x = 0; x < nSW; x++)
                for (int y = 0; y < nSH; y++)
                    screen[y * nSW + x] = titleSreen[y * nSW + x];

            if (bKey[5])
            {
                for (int x = 0; x < nSW; x++)
                    for (int y = 0; y < nSH; y++)
                        screen[y * nSW + x] = L' ';

                gState = GS_PLAYING;
            }

            bGameOver = bKey[4];
            bPlaying = !bGameOver;
        }
        
        while (!bGameOver && (gState == GS_PLAYING || gState == GS_PAUSE))
        {
            // GAME TIMING =====================================================
            this_thread::sleep_for(50ms);

            // INPUT ===========================================================
            for (int k = 0; k < sizeof(bKey); k++)
                bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26\x1b\x20"[k]))) != 0;

            if (gState == GS_PAUSE)
            {
                for (int x = 0; x < nSW; x++)
                    for (int y = 0; y < nSH; y++)
                        screen[y * nSW + x] = pausedSreen[y * nSW + x];

                if (bKey[4] && !bPauseHold)
                {
                    for (int x = 0; x < nSW; x++)
                        for (int y = 0; y < nSH; y++)
                            screen[y * nSW + x] = L' ';

                    gState = GS_PLAYING;

                    bPauseHold = true;
                }
                else if (!bKey[4])
                {
                    bPauseHold = false;
                }
            }
            else 
            {
                if (nLineCount % 10)
                    if (nSpeed >= 10) nSpeed--;

                nSpeedCounter++;
                bForceDown = (nSpeedCounter == nSpeed);

                // GAME LOGIC ======================================================
                nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
                nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
                nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

                nCurrentRotation += (bKey[3] && !bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;

                bRotateHold = bKey[3];

                if (bKey[4] && !bPauseHold)
                {
                    bPauseHold = true;
                    gState = GS_PAUSE;
                }
                else if (!bKey[4])
                    bPauseHold = false;
                bPlaying = !bGameOver;

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

                        switch (vLines.size())
                        {
                        case 4:
                            nScore += 1000;
                            break;
                        case 3:
                            nScore += 500;
                            break;
                        case 2:
                            nScore += 250;
                            break;
                        case 1:
                            nScore += 100;
                            break;
                        default:
                            break;
                        }


                        // Choose next piece
                        nCurrentX = nFieldWidth / 2;
                        nCurrentY = 0;
                        nCurrentRotation = 0;
                        nCurrentPiece = nNextPiece;
                        nNextPiece = rand() % NUM_TETROMINOES;

                        bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
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
            
        WriteConsoleOutputCharacter(hConsole, screen, nSW * nSH, { 0,0 }, &dwBytesWritten);

    }
    return 0;
}