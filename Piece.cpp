#include "Piece.h"

using namespace std;


Piece::Piece(int piece, int rank, int file, vector<string> moves)
{
	this->piece = piece;
	this->rank = rank;
	this->file = file;

	switch (piece)
	{
	case P:
		sprite.setTextureRect(sf::IntRect(375, 0, 75, 75));
		break;
	case R:
		sprite.setTextureRect(sf::IntRect(300, 0, 75, 75));
		break;
	case N:
		sprite.setTextureRect(sf::IntRect(225, 0, 75, 75));
		break;
	case B:
		sprite.setTextureRect(sf::IntRect(150, 0, 75, 75));
		break;
	case Q:
		sprite.setTextureRect(sf::IntRect(75, 0, 75, 75));
		break;
	case K:
		sprite.setTextureRect(sf::IntRect(0, 0, 75, 75));
		break;


	case p:
		sprite.setTextureRect(sf::IntRect(375, 75, 75, 75));
		break;
	case r:
		sprite.setTextureRect(sf::IntRect(300, 75, 75, 75));
		break;
	case n:
		sprite.setTextureRect(sf::IntRect(225, 75, 75, 75));
		break;
	case b:
		sprite.setTextureRect(sf::IntRect(150, 75, 75, 75));
		break;
	case q:
		sprite.setTextureRect(sf::IntRect(75, 75, 75, 75));
		break;
	case k:
		sprite.setTextureRect(sf::IntRect(0, 75, 75, 75));
		break;


	default:
		cout << "Piece object error" << endl;
		break;
	}

	if (piece > 0)
	{
		colour = white;
	}
	else
	{
		colour = black;
	}

	pos = sf::Vector2f(file * tile, rank * tile);
	sprite.setPosition(pos);
}

bool Piece::isClicked(sf::Vector2f m_pos)
{
	if (isCaptured == false)
	{
		if (m_pos.x > pos.x && m_pos.x < pos.x + tile && m_pos.y > pos.y && m_pos.y < pos.y + tile)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void Piece::update
(
	vector<string> moves,
	vector<vector<int> > board,
	vector<string>movelist,
	vector<vector<vector<int>>> boards
)
{
	if (isCaptured == false)
	{
		possibleMoves.clear();
		// Sorting out piece's possible moves

		// Pawns
		if (piece == P || piece == p)
		{
			for (string move : moves)
			{
				string tile = move.substr(0, 2);

				if (tile == toStringMove(rank, file))
				{
					if (who2move() == colour)
					{
						if (isMoveValid(move, board, movelist, boards))
						{
							possibleMoves.push_back(move);
						}
					}
					else
					{
						possibleMoves.push_back(move);
					}
				}
			}
		}
		// Other pieces
		else
		{
			for (string move : moves)
			{
				if (move.length() != 4 && move.length() != 6 && move.length() != 9)
				{
					string tile = move.substr(1, 2);

					if (tile == toStringMove(rank, file))
					{
						if (who2move() == colour)
						{
							if (isMoveValid(move, board, movelist, boards))
							{
								possibleMoves.push_back(move);
							}
						}
						else
						{
							possibleMoves.push_back(move);
						}
					}
				}
			}
		}

		// Calculate possible move positions
		possibleMovePos.clear();
		if (piece == p || piece == P)
		{
			for (string move : possibleMoves)
			{
				int file0 = char2move(move.at(2));
				int rank0 = char2move(move.at(3));
				int pos_x = file0 * tile;
				int pos_y = rank0 * tile;

				sf::Vector2f tilePos(pos_x, pos_y);
				possibleMovePos.push_back(tilePos);
			}
		}
		else
		{
			for (string move : possibleMoves)
			{
				int file0 = char2move(move.at(3));
				int rank0 = char2move(move.at(4));
				int pos_x = file0 * tile;
				int pos_y = rank0 * tile;

				sf::Vector2f tilePos(pos_x, pos_y);
				possibleMovePos.push_back(tilePos);
			}
		}

		// Update position data
		pos = sprite.getPosition();
	}
	else
	{
		possibleMoves.clear();
		possibleMovePos.clear();
		rank = -1;
		file = -1;
		pos.x = -tile, pos.y = -tile;
	}
}

void Piece::setPosition(sf::Vector2f pos)
{
	sprite.setPosition(pos);
	rank = pos.y / tile;
	file = pos.x / tile;
}

