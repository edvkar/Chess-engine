#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <conio.h>
#include <math.h>
#include <float.h>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <windows.h>

using namespace std;


// White pieces
#define P 1
#define R 5
#define N 3
#define B 4
#define Q 9
#define K 1000


// Black pieces
#define p -1
#define r -5
#define n -3
#define b -4
#define q -9
#define k -1000


// Using standard chess notation
enum FILES { file_A, file_B, file_C, file_D, file_E, file_F, file_G, file_H };
enum RANKS { rank_8, rank_7, rank_6, rank_5, rank_4, rank_3, rank_2, rank_1 };


// Colour values
#define white 1
#define black -1


// Boards
extern const vector<vector<int> > initialBoard;
extern vector<vector<int>> chessBoard;
extern vector<vector<vector<int>>> historyBoards;


// Game Vars
extern vector<string> allmoves;
extern int ply;
extern const int searchDepth;


// UI Vars
extern const int scr_w;
extern const int scr_h;
extern const int tile;


// General functions
extern inline int who2move(vector<string>movelist = allmoves);
extern inline void printBoard(vector<vector<int> > board = chessBoard);
extern inline void startGame_console();
extern inline void startGame_ui();
extern inline void pcvspc();
extern inline bool is_inVector(string str, vector<string>vect);


// Search and evaluation functions
extern inline float negaMax(
	vector<vector<int> > &board = chessBoard,
	vector<string> &movelist = allmoves, vector<vector<vector<int>>> boards = historyBoards,
	int depth = searchDepth
);
extern inline float evaluate(vector<vector<int> > board, vector<string>movelist);


// Game state functions
extern inline bool is_wk_attacked(vector<vector<int> > board = chessBoard, vector<string>movelist = allmoves);
extern inline bool is_bk_attacked(vector<vector<int> > board = chessBoard, vector<string>movelist = allmoves);
extern inline bool is_Checkmate(vector<vector<int> > board = chessBoard, vector<string>movelist = allmoves);
extern inline bool castling(vector<vector<int> > board, vector<string>movelist, string side, int colour);


// Possible moves
extern inline vector<string> possibleWhite(vector<vector<int> > board = chessBoard, vector<string>movelist = allmoves, vector<vector<vector<int>>> boards = historyBoards);
extern inline vector<string> possibleWP(vector<vector<int> > board, vector<vector<vector<int>>> boards = historyBoards, vector<string>movelist = allmoves);
extern inline vector<string> possibleWR(vector<vector<int> > board);
extern inline vector<string> possibleWN(vector<vector<int> > board);
extern inline vector<string> possibleWB(vector<vector<int> > board);
extern inline vector<string> possibleWQ(vector<vector<int> > board);
extern inline vector<string> possibleWK(vector<vector<int> > board, vector<string>movelist);


extern inline vector<string> possibleBlack(vector<vector<int> > board = chessBoard, vector<string>movelist = allmoves, vector<vector<vector<int>>> boards = historyBoards);
extern inline vector<string> possibleBP(vector<vector<int> > board, vector<vector<vector<int>>> boards = historyBoards, vector<string>movelist = allmoves);
extern inline vector<string> possibleBR(vector<vector<int> > board);
extern inline vector<string> possibleBN(vector<vector<int> > board);
extern inline vector<string> possibleBB(vector<vector<int> > board);
extern inline vector<string> possibleBQ(vector<vector<int> > board);
extern inline vector<string> possibleBK(vector<vector<int> > board, vector<string>movelist);



// Conversion functions
extern inline int char2move(char str);
extern inline char toCharPiece(int piece);
extern inline int char2piece(char piece);
extern inline string toStringMove(int rank, int file);


// Move Validity
extern inline bool isMoveValid(string move, vector<vector<int> > board = chessBoard, vector<string>movelist = allmoves, vector<vector<vector<int>>> boards = historyBoards);


// Movement functions
extern inline void movePiece
(
	string move,
	vector<vector<int> > &board = chessBoard,
	vector<string> &movelist = allmoves,
	vector<vector<vector<int>>> &boards = historyBoards
);
extern inline void movePiece_negaMax
(
	string move,
	vector<vector<int> > &board = chessBoard,
	vector<string> &movelist = allmoves,
	vector<vector<vector<int>>> &boards = historyBoards
);

