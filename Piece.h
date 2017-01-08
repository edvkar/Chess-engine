#pragma once
#include "defines.h"

using namespace std;


class Piece
{
public:
	Piece(int piece, int rank, int file, vector<string> moves);

	// Vars
	const int tile = 75;

	int rank;
	int file;
	int colour;
	int piece;
	bool isCaptured = false;
	bool clicked = false;
	sf::Vector2f pos;

	sf::Sprite sprite;
	vector<string> possibleMoves;
	vector<sf::Vector2f> possibleMovePos;

	// Functions
	void update
	(
		vector<string> moves,
		vector<vector<int> > board = chessBoard,
		vector<string>movelist = allmoves,
		vector<vector<vector<int>>> boards = historyBoards
	);

	bool isClicked(sf::Vector2f m_pos);
	void setPosition(sf::Vector2f pos);
};

