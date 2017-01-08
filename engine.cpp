#include "defines.h"
#include "Piece.h"

using namespace std;

// Boards
const vector<vector<int> > initialBoard =
{
	{ r, n, b, q, k, b, n, r }, // 8 (index 0)
	{ p, p, p, p, p, p, p, p }, // 7
	{ 0, 0, 0, 0, 0, 0, 0, 0 }, // 6
	{ 0, 0, 0, 0, 0, 0, 0, 0 }, // 5
	{ 0, 0, 0, 0, 0, 0, 0, 0 }, // 4 
	{ 0, 0, 0, 0, 0, 0, 0, 0 }, // 3
	{ P, P, P, P, P, P, P, P }, // 2
	{ R, N, B, Q, K, B, N, R }  // 1
	//A, B, C, D, E, F, G, H
};

vector<vector<int>> chessBoard = initialBoard;
vector<vector<vector<int>>> historyBoards;


// Game Vars
vector<string> allmoves;
int ply = allmoves.size();
const int searchDepth = 3;


// UI Vars
const int scr_w = 600;
const int scr_h = 600;
const int tile = 75;


// UI Functions
void movePiece_ui(Piece &piece2move, vector<Piece> &pieces, sf::Vector2f pos);
float negaMax_ui
(
	vector<Piece> &pieces,
	vector<vector<int> > &board = chessBoard,
	vector<string> &movelist = allmoves,
	vector<vector<vector<int>>> boards = historyBoards,
	int depth = searchDepth
);

int main()
{
	startGame_ui();
}

void startGame_ui()
{
	const int userColour = white;

	sf::RenderWindow gameWindow(sf::VideoMode(scr_w, scr_h), "Chess");
	gameWindow.setVerticalSyncEnabled(false);
	gameWindow.setFramerateLimit(60);


	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;


	// Texture: Chess board
	sf::Texture txt_chessBoard;
	if (!txt_chessBoard.loadFromFile("chessBoard.png"))
		EXIT_FAILURE;


	// Texture: Chess pieces
	sf::Texture txt_chessPieces;
	if (!txt_chessPieces.loadFromFile("chessPieces.png"))
		EXIT_FAILURE;
	txt_chessPieces.setSmooth(true);


	// Soundbuffer: Chess move
	sf::SoundBuffer sbfr_chessMove;
	if (!sbfr_chessMove.loadFromFile("sndChessMove.wav"))
		EXIT_FAILURE;


	// Sound: Chess move
	sf::Sound snd_chessMove(sbfr_chessMove);
	snd_chessMove.setLoop(false);

	
	// Sprite: Chess board
	sf::Sprite spr_chessBoard;
	spr_chessBoard.setTexture(txt_chessBoard);
	spr_chessBoard.setPosition(0, 0);

	


	// Initialize all pieces
	vector<Piece> pieces;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (chessBoard[i][j] != 0)
			{
				vector<string> moves = chessBoard[i][j] > 0 ? possibleWhite() : possibleBlack();
				Piece piece(chessBoard[i][j], i, j, moves);
				piece.sprite.setTexture(txt_chessPieces);
				pieces.push_back(piece);
			}
		}
	}



	historyBoards.push_back(chessBoard);


	while (gameWindow.isOpen())
	{


		sf::Event event;

		// User's turn
		if (who2move() == userColour)
		{
			while (gameWindow.pollEvent(event))
			{
				if (is_Checkmate())
				{
					cout << "Checkmate" << endl;
				}
				// Mouse coordinates
				sf::Vector2f mouse_pos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);


				switch (event.type)
				{

					// Mouse Button
				case sf::Event::MouseButtonPressed:

					if (event.mouseButton.button == sf::Mouse::Left)
					{

						for (int i = 0; i < pieces.size(); i++)
						{

							// Piece click
							if (pieces[i].isClicked(mouse_pos) && pieces[i].colour == who2move())
							{
								pieces[i].clicked = !pieces[i].clicked;

								vector<string> moves = pieces[i].colour == white ? possibleWhite() : possibleBlack();
								pieces[i].update(moves);

							}

							// Move piece click
							else if (pieces[i].clicked == true)
							{

								for (sf::Vector2f pos : pieces[i].possibleMovePos)
								{

									if (mouse_pos.x > pos.x && mouse_pos.x < pos.x + tile && mouse_pos.y > pos.y && mouse_pos.y < pos.y + tile)
									{
										pieces[i].clicked = false;

										movePiece_ui(pieces[i], pieces, pos);
										historyBoards.push_back(chessBoard);
									}
								}
								pieces[i].clicked = false;
							}

							// Void clicked
							else
							{
								pieces[i].clicked = false;
							}

						}
					}
					break;


					// Closing Window
				case sf::Event::Closed:
					gameWindow.close();
					break;


				default:
					break;
				}
			}
		}
		// Nega Max
		else
		{
			negaMax_ui(pieces);
			historyBoards.push_back(chessBoard);
		}


		// Drawring
		gameWindow.clear(sf::Color::Black);


		gameWindow.draw(spr_chessBoard);


		// Handling pieces
		for (int i = 0; i < pieces.size(); i++)
		{
			if (!pieces[i].isCaptured)
			{
				vector<string> moves = pieces[i].colour == white ? possibleWhite() : possibleBlack();
				pieces[i].update(moves);


				gameWindow.draw(pieces[i].sprite);


				// Highlight the tiles the clicked piece is allowed to move
				if (pieces[i].clicked)
				{
					for (sf::Vector2f pos : pieces[i].possibleMovePos)
					{
						sf::RectangleShape hilghtd_tile(sf::Vector2f(tile - 5, tile - 5));
						hilghtd_tile.setFillColor(sf::Color(0, 175, 0, 128));
						hilghtd_tile.setOutlineThickness(5);
						hilghtd_tile.setOutlineColor(sf::Color(0, 100, 0, 255));
						hilghtd_tile.setPosition(pos);
						gameWindow.draw(hilghtd_tile);
					}
				}
			}
		}


		gameWindow.display();
	}
}

void startGame_console()
{
	historyBoards.push_back(chessBoard);

	char colour;
	int attempts = 0;

	// Game starts
	do
	{
		if (attempts > 0)
			cout << "Invalid colour\n";

		cout << "Which side? (w/b)";
		cin >> colour;

		attempts++;
	} while (colour != 'w' && colour != 'b' && colour != 'W' && colour != 'B');

	// Game loop
	while (!is_Checkmate())
	{
		// User colour = white
		if (colour == 'w' || colour == 'W')
		{

			system("CLS");
			string whosTurn = who2move() == white ? "White" : "Black";


			// Information output
			cout << "Ply: " << allmoves.size() << endl;


			if (allmoves.size() > 0)
			{
				cout << "Last move: " << allmoves.back() << endl;
			}


			cout << whosTurn << "'s turn\n";
			printBoard();


			// User's turn
			if (who2move() == white)
			{

				vector<string> possibleMoves = possibleWhite();
				string move;


				if (is_wk_attacked())
				{
					cout << "Check!\n";
				}


				cout << "Enter a move: ";
				cin >> move;


				// Valid move
				if (is_inVector(move, possibleMoves))
				{
					movePiece(move);
				}
				else
				{
					// Invalid move
					while (!is_inVector(move, possibleMoves) || !isMoveValid(move, chessBoard, allmoves))
					{
						cout << "invalid move\n";
						cout << "Enter a move: ";
						cin >> move;
						if (is_inVector(move, possibleMoves))
						{
							movePiece(move);
						}
					}
				}
			}
			// Computer's turn 
			else
			{
				negaMax();
			}
		}
		// User colour = black
		else
		{

			system("CLS");
			string whosTurn = who2move() == white ? "White" : "Black";


			// Information output
			cout << "Ply: " << allmoves.size() << endl;

			if (allmoves.size() > 0)
			{
				cout << "Last move: " << allmoves.back() << endl;
			}

			cout << whosTurn << "'s turn\n";
			printBoard();

			// User's turn
			if (who2move() == black)
			{

				vector<string> possibleMoves = possibleBlack();
				string move;


				if (is_bk_attacked())
				{
					cout << "Check!\n";
				}

				cout << "enter a move: ";
				cin >> move;


				// Valid move
				if (is_inVector(move, possibleMoves))
				{
					movePiece(move);
				}
				else
				{
					// Invalid move
					while (!is_inVector(move, possibleMoves) || !isMoveValid(move, chessBoard, allmoves))
					{
						cout << "invalid move\n";
						cout << "Enter a move: ";
						cin >> move;
						if (is_inVector(move, possibleMoves))
						{
							movePiece(move);
						}
					}
				}
			}
			// Computer's turn
			else
			{
				negaMax();
			}
		}
	}

	// Game end
	if (is_Checkmate())
	{
		if (who2move() == white)
		{
			cout << "Checkmate! Black wins\n";
		}
		else
		{
			cout << "Checkmate! White wins\n";
		}
	}
}

void pcvspc()
{
	historyBoards.push_back(chessBoard);

	// Game loop
	while (!is_Checkmate())
	{
		system("CLS");
		string whosTurn = who2move() == white ? "White" : "Black";

		// Information output
		cout << "Ply: " << allmoves.size() << endl;
		if (allmoves.size() > 0)
			cout << "Last move: " << allmoves.back() << endl;

		cout << whosTurn << "'s turn\n";
		printBoard();
		Sleep(500);

		// User's turn
		if (who2move() == white)
		{
			negaMax();
			historyBoards.push_back(chessBoard);
		}
		else
		{
			negaMax();
			historyBoards.push_back(chessBoard);
		}
	}

	// Game end
	if (is_Checkmate())
	{
		if (who2move() == white)
		{
			cout << "Checkmate! Black wins\n";
		}
		else
		{
			cout << "Checkmate! White wins\n";
		}
	}
}

bool isMoveValid(string move, vector<vector<int> > board, vector<string>movelist, vector<vector<vector<int>>> boards)
{
	vector<vector<int>> tempBoard = board;
	vector<string> tempList = movelist;
	vector<vector<vector<int>>> tempBoards = boards;

	movePiece(move, tempBoard, tempList, tempBoards);

	if (who2move(movelist) == white)
	{
		return is_wk_attacked(tempBoard, tempList) ? false : true;
	}
	else
	{
		return is_bk_attacked(tempBoard, tempList) ? false : true;
	}
}

bool is_inVector(string str, vector<string>vect)
{
	return (find(vect.begin(), vect.end(), str) != vect.end());
}

void movePiece(string move, vector<vector<int> > &board, vector<string> &movelist, vector<vector<vector<int>>> &boards)
{
	int piece;
	int file0;
	int rank0;
	int file;
	int rank;
	vector<string> moves = who2move(movelist) == white ? possibleWhite(board, movelist) : possibleBlack(board, movelist);

	if (who2move(movelist) == white)
	{
		// White pawn (e.g. a2a3)
		if (move.length() == 4)
		{
			if (is_inVector(move, moves))
			{
				file0 = char2move(move.at(0));
				rank0 = char2move(move.at(1));
				file = char2move(move.at(2));
				rank = char2move(move.at(3));

				board[rank0][file0] = 0;
				board[rank][file] = P;
				movelist.push_back(move);
			}
			else
				cout << "Invalid move: " << move << endl;
		}
		// Rest of white pieces (e.g. Qd4e5)
		else if (move.length() == 5)
		{
			piece = char2piece(move.at(0));
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));

			rank = char2move(move.at(4));

			switch (piece)
			{
			case R:
				if (is_inVector(move, moves))
				{
					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case N:
				if (is_inVector(move, moves))
				{
					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case B:
				if (is_inVector(move, moves))
				{
					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case Q:
				if (is_inVector(move, moves))
				{
					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case K:
				if (is_inVector(move, moves))
				{
					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			default:
				cout << "White piece with move string length 5 not found\n";
				break;
			}
		}
		// White pawn capture (e.g. d2e3-p)
		else if (move.length() == 6)
		{
			if (is_inVector(move, moves))
			{
				file0 = char2move(move.at(0));
				rank0 = char2move(move.at(1));
				file = char2move(move.at(2));
				rank = char2move(move.at(3));

				board[rank0][file0] = 0;
				board[rank][file] = P;
				movelist.push_back(move);
				
			}
			else
				cout << "Invalid move: " << move << endl;
		}
		// Rest of white pieces capture (e.g. Bc1f4-n)
		else if (move.length() == 7)
		{
			piece = char2piece(move.at(0));
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));
			rank = char2move(move.at(4));

			switch (piece)
			{
			case R:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case N:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case B:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case Q:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case K:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			default:
				cout << "White piece with move string length 5 not found\n";
				break;
			}
		}
		// En passant rule
		else if (move.length() == 9)
		{
			file0 = char2move(move.at(0));
			rank0 = char2move(move.at(1));
			file = char2move(move.at(2));
			rank = char2move(move.at(3));

			if (is_inVector(move, moves))
			{

				board[rank0][file0] = 0;
				board[rank0][file] = 0;
				board[rank][file] = P;
				movelist.push_back(move);
				
			}
			else
				cout << "Invalid move: " << move << endl;
		}
		// Castling rule
		else if (move.length() == 11)
		{
			piece = K;
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));
			rank = char2move(move.at(4));
			char castle_side = move.at(6);
			if (is_inVector(move, moves))
			{
				if (castle_side == 'r')
				{

					board[rank0][file0] = 0;
					board[rank][file] = K;
					board[rank][file_H] = 0;
					board[rank][file - 1] = R;
					movelist.push_back(move);
					
				}
				else if (castle_side == 'l')
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					board[rank][file_A] = 0;
					board[rank][file + 1] = R;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
			}
			else
				cout << "Invalid move: " << move << endl;
		}
	}
	else if (who2move(movelist) == black)
	{
		// Black pawn move
		if (move.length() == 4)
		{
			if (is_inVector(move, moves))
			{
				file0 = char2move(move.at(0));
				rank0 = char2move(move.at(1));
				file = char2move(move.at(2));
				rank = char2move(move.at(3));

				board[rank0][file0] = 0;
				board[rank][file] = p;
				movelist.push_back(move);
				
			}
			else
				cout << "Invalid move: " << move << endl;
		}
		// Rest of black pieces move
		else if (move.length() == 5)
		{
			piece = char2piece(move.at(0));
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));
			rank = char2move(move.at(4));

			switch (piece)
			{
			case r:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case n:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case b:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case q:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case k:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			default:
				cout << "Black piece with move string length 5 not found\n";
				break;
			}
		}
		// Black pawn capture
		else if (move.length() == 6)
		{
			if (is_inVector(move, moves))
			{
				file0 = char2move(move.at(0));
				rank0 = char2move(move.at(1));
				file = char2move(move.at(2));
				rank = char2move(move.at(3));

				board[rank0][file0] = 0;
				board[rank][file] = p;
				movelist.push_back(move);
				
			}
			else
				cout << "Invalid move: " << move << endl;
		}
		// Rest of black pieces capture
		else if (move.length() == 7)
		{
			piece = char2piece(move.at(0));
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));
			rank = char2move(move.at(4));

			switch (piece)
			{
			case r:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case n:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case b:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case q:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			case k:
				if (is_inVector(move, moves))
				{

					board[rank0][file0] = 0;
					board[rank][file] = piece;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
				break;

			default:
				cout << "Black piece with move string length 5 not found\n";
				break;
			}
		}
		// En passant rule
		else if (move.length() == 9)
		{
			file0 = char2move(move.at(0));
			rank0 = char2move(move.at(1));
			file = char2move(move.at(2));
			rank = char2move(move.at(3));

			if (is_inVector(move, moves))
			{

				board[rank0][file0] = 0;
				board[rank0][file] = 0;
				board[rank][file] = P;
				movelist.push_back(move);
				
			}
			else
				cout << "Invalid move: " << move << endl;
		}
		// Castling rule
		else if (move.length() == 11)
		{
			piece = k;
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));
			rank = char2move(move.at(4));
			char castle_side = move.at(6);

			if (is_inVector(move, moves))
			{
				if (castle_side == 'r')
				{
					board[rank0][file0] = 0;
					board[rank][file] = piece;
					board[rank][file_H] = 0;
					board[rank][file - 1] = r;
					movelist.push_back(move);
					
				}
				else if (castle_side == 'l')
				{
					board[rank0][file0] = 0;
					board[rank][file] = piece;
					board[rank][file_A] = 0;
					board[rank][file + 1] = r;
					movelist.push_back(move);
					
				}
				else
					cout << "Invalid move: " << move << endl;
			}
			else
				cout << "Invalid move: " << move << endl;
		}
	}
}

void movePiece_ui(Piece &piece2move, vector<Piece> &pieces, sf::Vector2f pos)
{
	int file = pos.x / 75;
	int rank = pos.y / 75;

	string fromPos = toStringMove(piece2move.rank, piece2move.file);
	string toPos = toStringMove(rank, file);

	piece2move.file = file;
	piece2move.rank = rank;

	// Pawn move
	if (piece2move.piece == P || piece2move.piece == p)
	{
		string move = fromPos + toPos;

		for (string x : piece2move.possibleMoves)
		{
			if (x.find(move) != string::npos)
			{
				move = x;
			}
		}

		// Capture/En Passant
		if (move.find("-") != string::npos)
		{
			// En Passant
			if (move.find("enPa") != string::npos)
			{
				// White
				if (piece2move.piece == P)
				{
					for (int j = 0; j < pieces.size(); j++)
					{
						if (pieces[j].pos.x == pos.x && pieces[j].pos.y == pos.y + tile)
						{
							pieces[j].isCaptured = true;
						}
					}
				}
				// Black
				else
				{
					for (int j = 0; j < pieces.size(); j++)
					{
						if (pieces[j].pos.x == pos.x && pieces[j].pos.y == pos.y - tile)
						{
							pieces[j].isCaptured = true;
						}
					}
				}
			}
			// Capture
			else
			{
				for (int j = 0; j < pieces.size(); j++)
				{
					if (pieces[j].pos == pos)
					{
						pieces[j].isCaptured = true;
					}
				}
			}
		}

		piece2move.setPosition(pos);
		movePiece(move);
	}
	// Other pieces
	else
	{
		string move = toCharPiece(piece2move.piece) + fromPos + toPos;

		for (string x : piece2move.possibleMoves)
		{
			if (x.find(move) != string::npos)
			{
				move = x;
			}
		}

		// Capture and castling
		if (move.find("-") != string::npos)
		{
			// Right castling
			if (move.find("rcast") != string::npos)
			{
				for (int j = 0; j < pieces.size(); j++)
				{
					if (pieces[j].pos.y == pos.y && pieces[j].pos.x == pos.x + tile)
					{
						pieces[j].setPosition(sf::Vector2f(pos.x - tile, pos.y));
					}
				}
			}
			// Left castling
			else if (move.find("lcast") != string::npos)
			{
				for (int j = 0; j < pieces.size(); j++)
				{
					if (pieces[j].pos.y == pos.y && pieces[j].pos.x == pos.x - 2 * tile)
					{
						pieces[j].setPosition(sf::Vector2f(pos.x + tile, pos.y));
					}
				}
			}
			// Capture
			else
			{
				for (int j = 0; j < pieces.size(); j++)
				{
					if (pieces[j].pos == pos)
					{
						pieces[j].isCaptured = true;
					}
				}
			}
		}

		piece2move.setPosition(pos);
		movePiece(move);
	}
}

void movePiece_negaMax(string move, vector<vector<int> > &board, vector<string> &movelist, vector<vector<vector<int>>> &boards)
{
	int piece;
	int file0;
	int rank0;
	int file;
	int rank;
	// vector<string> moves = who2move(movelist) == white ? possibleWhite(board, movelist) : possibleBlack(board, movelist);

	if (who2move(movelist) == white)
	{
		// White pawn (e.g. a2a3)
		if (move.length() == 4)
		{
			file0 = char2move(move.at(0));
			rank0 = char2move(move.at(1));
			file = char2move(move.at(2));
			rank = char2move(move.at(3));

			board[rank0][file0] = 0;
			board[rank][file] = P;
			movelist.push_back(move);
		}
		// Rest of white pieces (e.g. Qd4e5)
		else if (move.length() == 5)
		{
			piece = char2piece(move.at(0));
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));

			rank = char2move(move.at(4));

			switch (piece)
			{
			case R:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case N:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case B:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case Q:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case K:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			default:
				cout << "White piece with move string length 5 not found\n";
				break;
			}
		}
		// White pawn capture (e.g. d2e3-p)
		else if (move.length() == 6)
		{
			file0 = char2move(move.at(0));
			rank0 = char2move(move.at(1));
			file = char2move(move.at(2));
			rank = char2move(move.at(3));

			board[rank0][file0] = 0;
			board[rank][file] = P;
			movelist.push_back(move);
		}
		// Rest of white pieces capture (e.g. Bc1f4-n)
		else if (move.length() == 7)
		{
			piece = char2piece(move.at(0));
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));
			rank = char2move(move.at(4));

			switch (piece)
			{
			case R:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case N:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case B:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case Q:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case K:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			default:
				cout << "White piece with move string length 5 not found\n";
				break;
			}
		}
		// En passant rule
		else if (move.length() == 9)
		{
			file0 = char2move(move.at(0));
			rank0 = char2move(move.at(1));
			file = char2move(move.at(2));
			rank = char2move(move.at(3));

			board[rank0][file0] = 0;
			board[rank0][file] = 0;
			board[rank][file] = P;
			movelist.push_back(move);
		}
		// Castling rule
		else if (move.length() == 11)
		{
			piece = K;
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));
			rank = char2move(move.at(4));
			char castle_side = move.at(6);

			if (castle_side == 'r')
			{

				board[rank0][file0] = 0;
				board[rank][file] = K;
				board[rank][file_H] = 0;
				board[rank][file - 1] = R;
				movelist.push_back(move);

			}
			else if (castle_side == 'l')
			{

				board[rank0][file0] = 0;
				board[rank][file] = piece;
				board[rank][file_A] = 0;
				board[rank][file + 1] = R;
				movelist.push_back(move);

			}
			else
				cout << "Invalid move: " << move << endl;
		}
	}
	else if (who2move(movelist) == black)
	{
		// Black pawn move
		if (move.length() == 4)
		{
			file0 = char2move(move.at(0));
			rank0 = char2move(move.at(1));
			file = char2move(move.at(2));
			rank = char2move(move.at(3));

			board[rank0][file0] = 0;
			board[rank][file] = p;
			movelist.push_back(move);
		}
		// Rest of black pieces move
		else if (move.length() == 5)
		{
			piece = char2piece(move.at(0));
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));
			rank = char2move(move.at(4));

			switch (piece)
			{
			case r:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case n:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case b:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case q:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case k:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			default:
				cout << "Black piece with move string length 5 not found\n";
				break;
			}
		}
		// Black pawn capture
		else if (move.length() == 6)
		{
			file0 = char2move(move.at(0));
			rank0 = char2move(move.at(1));
			file = char2move(move.at(2));
			rank = char2move(move.at(3));

			board[rank0][file0] = 0;
			board[rank][file] = p;
			movelist.push_back(move);
		}
		// Rest of black pieces capture
		else if (move.length() == 7)
		{
			piece = char2piece(move.at(0));
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));
			rank = char2move(move.at(4));

			switch (piece)
			{
			case r:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case n:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case b:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case q:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			case k:
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				movelist.push_back(move);
				break;

			default:
				cout << "Black piece with move string length 5 not found\n";
				break;
			}
		}
		// En passant rule
		else if (move.length() == 9)
		{
			file0 = char2move(move.at(0));
			rank0 = char2move(move.at(1));
			file = char2move(move.at(2));
			rank = char2move(move.at(3));

			board[rank0][file0] = 0;
			board[rank0][file] = 0;
			board[rank][file] = P;
			movelist.push_back(move);

		}
		// Castling rule
		else if (move.length() == 11)
		{
			piece = k;
			file0 = char2move(move.at(1));
			rank0 = char2move(move.at(2));
			file = char2move(move.at(3));
			rank = char2move(move.at(4));
			char castle_side = move.at(6);

			if (castle_side == 'r')
			{
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				board[rank][file_H] = 0;
				board[rank][file - 1] = r;
				movelist.push_back(move);

			}
			else if (castle_side == 'l')
			{
				board[rank0][file0] = 0;
				board[rank][file] = piece;
				board[rank][file_A] = 0;
				board[rank][file + 1] = r;
				movelist.push_back(move);

			}
			else
				cout << "Invalid move: " << move << endl;
		}
	}
}

int who2move(vector<string>movelist)
{
	if (movelist.size() % 2 == 0)
		return white;
	else
		return black;
}

char toCharPiece(int piece)
{
	switch (piece)
	{
		// White
	case P:
		return 'P';
		break;

	case R:
		return 'R';
		break;

	case N:
		return 'N';
		break;

	case B:
		return 'B';
		break;

	case Q:
		return 'Q';
		break;

	case K:
		return 'K';
		break;

		// Black
	case p:
		return 'p';
		break;

	case r:
		return 'r';
		break;

	case n:
		return 'n';
		break;

	case b:
		return 'b';
		break;

	case q:
		return 'q';
		break;

	case k:
		return 'k';
		break;

	default:
		return 'X';
		break;

	}
	// Error
	cout << "Function toCharPiece error (invalid piece)\n";
	return '6';
}

int char2piece(char piece)
{
	switch (piece)
	{
		// White
	case 'P':
		return P;
		break;

	case 'R':
		return R;
		break;

	case 'N':
		return N;
		break;

	case 'B':
		return B;
		break;

	case 'Q':
		return Q;
		break;

	case 'K':
		return K;
		break;


		// Black
	case 'p':
		return p;
		break;

	case 'r':
		return r;
		break;

	case 'n':
		return n;
		break;

	case 'b':
		return b;
		break;

	case 'q':
		return q;
		break;

	case 'k':
		return k;
		break;

	default:
		break;
	}
	// Error
	cout << "Function char2piece error (invalid piece)\n";
	return 666;
}

int char2move(char pos)
{
	// Files
	if (pos == 'a')
		return file_A;

	else if (pos == 'b')
		return file_B;

	else if (pos == 'c')
		return file_C;

	else if (pos == 'd')
		return file_D;

	else if (pos == 'e')
		return file_E;

	else if (pos == 'f')
		return file_F;

	else if (pos == 'g')
		return file_G;

	else if (pos == 'h')
		return file_H;

	// Ranks
	else if (pos == '1')
		return rank_1;

	else if (pos == '2')
		return rank_2;

	else if (pos == '3')
		return rank_3;

	else if (pos == '4')
		return rank_4;

	else if (pos == '5')
		return rank_5;

	else if (pos == '6')
		return rank_6;

	else if (pos == '7')
		return rank_7;

	else if (pos == '8')
		return rank_8;

	// invalid input
	else
		return 666;
}

void printBoard(vector<vector<int> > board)
{
	cout << "#########################################################  |  " << endl;
	// Loops through rows
	for (int i = 0; i < 8; i++) {
		cout << "#      #      #      #      #      #      #      #      #  |  " << endl;
		// Loops through columns
		for (int j = 0; j < 8; j++) {
			// Checking value of piece
			switch (board[i][j])
			{

				// White pieces
			case P:
				if (j == 7)
					cout << "#  wP  #  |  " << 8 - i << endl;
				else
					cout << "#  wP  ";
				break;

			case R:
				if (j == 7)
					cout << "#  wR  #  |  " << 8 - i << endl;
				else
					cout << "#  wR  ";
				break;

			case N:
				if (j == 7)
					cout << "#  wN  #  |  " << 8 - i << endl;
				else
					cout << "#  wN  ";
				break;

			case B:
				if (j == 7)
					cout << "#  wB  #  |  " << 8 - i << endl;
				else
					cout << "#  wB  ";
				break;

			case Q:
				if (j == 7)
					cout << "#  wQ  #  |  " << 8 - i << endl;
				else
					cout << "#  wQ  ";
				break;

			case K:
				if (j == 7)
					cout << "#  wK  #  |  " << 8 - i << endl;
				else
					cout << "#  wK  ";
				break;

				// Black pieces
			case p:
				if (j == 7)
					cout << "#  bP  #  |  " << 8 - i << endl;
				else
					cout << "#  bP  ";
				break;

			case r:
				if (j == 7)
					cout << "#  bR  #  |  " << 8 - i << endl;
				else
					cout << "#  bR  ";
				break;

			case n:
				if (j == 7)
					cout << "#  bN  #  |  " << 8 - i << endl;
				else
					cout << "#  bN  ";
				break;

			case b:
				if (j == 7)
					cout << "#  bB  #  |  " << 8 - i << endl;
				else
					cout << "#  bB  ";
				break;

			case q:
				if (j == 7)
					cout << "#  bQ  #  |  " << 8 - i << endl;
				else
					cout << "#  bQ  ";
				break;

			case k:
				if (j == 7)
					cout << "#  bK  #  |  " << 8 - i << endl;
				else
					cout << "#  bK  ";
				break;

			default:
				if (j == 7)
					cout << "#      #  |  " << 8 - i << endl;
				else
					cout << "#      ";
			}

		}
		cout << "#      #      #      #      #      #      #      #      #  |  " << endl;
		cout << "#########################################################  |  " << endl;
	}

	cout << "\n";
	cout << "---------------------------------------------------------" << endl;
	cout << "   A      B      C      D      E      F      G      H   " << endl;
	cout << "\n\n";
}

string toStringMove(int rank, int file)
{
	char ra;
	char fi;

	// Ranks
	switch (rank)
	{
	case rank_8:
		ra = '8';
		break;

	case rank_7:
		ra = '7';
		break;

	case rank_6:
		ra = '6';
		break;

	case rank_5:
		ra = '5';
		break;

	case rank_4:
		ra = '4';
		break;

	case rank_3:
		ra = '3';
		break;

	case rank_2:
		ra = '2';
		break;

	case rank_1:
		ra = '1';
		break;

	default:
		cout << "invalid rank (toStringMove functions)\n" << "rank = " << rank << endl;
		ra = '0';
		break;
	}

	// Files
	switch (file)
	{
	case 0:
		fi = 'a';
		break;

	case 1:
		fi = 'b';
		break;

	case 2:
		fi = 'c';
		break;

	case 3:
		fi = 'd';
		break;

	case 4:
		fi = 'e';
		break;

	case 5:
		fi = 'f';
		break;

	case 6:
		fi = 'g';
		break;

	case 7:
		fi = 'h';
		break;

	default:
		cout << "invalid file (toStringMove function)\n";
		fi = '0';
		break;
	}
	string pos;
	pos += fi;
	pos += ra;
	return pos;
}

float evaluate(vector<vector<int> > board, vector<string>movelist)
{
	float score = 0;

	// Stores the amount of each piece
	float wP = 0;
	float wR = 0;
	float wN = 0;
	float wB = 0;
	float wK = 0;
	float wQ = 0;

	float bP = 0;
	float bR = 0;
	float bN = 0;
	float bB = 0;
	float bK = 0;
	float bQ = 0;

	// Stores the number of legal moves
	float wMobility = possibleWhite(board, movelist).size();
	float bMobility = possibleBlack(board, movelist).size();

	// Stores the number of doubled, blocked or isolated pawns
	float wPdoubled = 0;
	float wPblocked = 0;
	float wPisolated = 0;

	float bPdoubled = 0;
	float bPblocked = 0;
	float bPisolated = 0;

	// Used to check if a pawn is doubled
	float wP_onFile = 0;
	float bP_onFile = 0;

	int file0;
	int rank0;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			rank0 = i;
			file0 = j;
			switch (board[rank0][file0])
			{
			case P:
				wP++;
				if (rank0 != rank_8)
					if (board[rank0 - 1][file0] != 0)
						wPblocked++;

				if (file_A < file0 && file0 < file_H)
					if (board[rank0][file0 - 1] == 0 && board[rank0][file0 + 1] == 0)
						wPisolated++;

					else if (file0 == file_H && board[rank0][file_G] == 0)
						wPisolated++;

					else if (file0 == file_A && board[rank0][file_B] == 0)
						wPisolated++;

				for (int x = 0; x < 8; x++)
				{
					if (chessBoard[x][file0] == P)
						wP_onFile++;
				}

				if (wP_onFile > 1)
					wPdoubled++;

				break;

			case R:
				wR++;
				break;

			case N:
				wN++;
				break;

			case B:
				wB++;
				break;

			case Q:
				wQ++;
				break;

			case K:
				wK++;
				break;


			case p:
				bP++;
				if (rank0 != rank_8)
					if (board[rank0 + 1][file0] != 0)
						bPblocked++;

				if (file_A < file0 && file0 < file_H)
					if (board[rank0][file0 - 1] == 0 && board[rank0][file0 + 1] == 0)
						bPisolated++;

					else if (file0 == file_H && board[rank0][file_G] == 0)
						bPisolated++;

					else if (file0 == file_A && board[rank0][file_B] == 0)
						bPisolated++;

				for (int x = 0; x < 8; x++)
				{
					if (chessBoard[x][file0] == p)
						bP_onFile++;
				}

				if (bP_onFile > 1)
					bPdoubled++;

				break;

			case r:
				bR++;
				break;

			case n:
				bN++;
				break;

			case b:
				bB++;
				break;

			case q:
				bQ++;
				break;

			case k:
				bK++;
				break;

			default:
				break;
			}
		}
	}
	score =
		1000 * (wK - bK)
		+ 20 * (wQ - bQ)
		+ 5 * (wR - bR)
		+ 3 * (wB - bB + wN - bN)
		+ 1 * (wP - bP)
		- 0.5 * (wPdoubled - bPdoubled + wPblocked - bPblocked + wPisolated - bPisolated)
		+ 0.1 * (wMobility - bMobility);

	return who2move(movelist) * score;
}

float negaMax(vector<vector<int> > &board, vector<string>&movelist, vector<vector<vector<int>>> boards, int depth)
{
	if (depth == 0)
	{
		return evaluate(board, movelist);
	}

	vector<string> moves = who2move(movelist) == white ? possibleWhite(board, movelist, boards) : possibleBlack(board, movelist, boards);
	string bestMove;
	string move;
	clock_t time0 = clock();


	float max = -1 * INFINITY;

	for (int m = 0; m < moves.size(); m++)
	{
		move = moves.at(m);
		vector<vector<int> > tempBoard = board;
		vector<string> tempList = movelist;
		vector<vector<vector<int>>> tempBoards = boards;

		movePiece_negaMax(move, tempBoard, tempList, tempBoards);
		tempBoards.push_back(tempBoard);
		
		

		float moveScore = (-1) * negaMax(tempBoard, tempList, tempBoards, depth - 1);


		// Debug
		if (depth == searchDepth)
		{
			cout << fixed << setprecision(0) << "Percent loaded: " << ((float)m / moves.size()) * 100 << "%\n";
			clock_t time1 = (clock() - time0);
			double time = time1 / (float)CLOCKS_PER_SEC;
			cout << fixed << setprecision(2) << "Time: " << time << "s\n\n";
			cout << "Amount of moves: " << moves.size() << endl;
		}

		if (moveScore > max)
		{
			bestMove = move;
			max = moveScore;
		}

	}
	movePiece(bestMove, board, movelist);
	return max;
}

float negaMax_ui(vector<Piece> &pieces, vector<vector<int> > &board, vector<string>&movelist, vector<vector<vector<int>>> boards, int depth)
{
	if (depth == 0)
	{
		return evaluate(board, movelist);
	}

	vector<string> moves = who2move(movelist) == white ? possibleWhite(board, movelist, boards) : possibleBlack(board, movelist, boards);
	string bestMove;
	string move;
	clock_t time0 = clock();


	float max = -1 * INFINITY;

	for (int m = 0; m < moves.size(); m++)
	{
		move = moves.at(m);
		vector<vector<int> > tempBoard = board;
		vector<string> tempList = movelist;
		vector<vector<vector<int>>> tempBoards = boards;

		
		movePiece_negaMax(move, tempBoard, tempList, tempBoards);
		tempBoards.push_back(tempBoard);
		

		float moveScore = (-1) * negaMax(tempBoard, tempList, tempBoards, depth - 1);


		// Debug
		if (depth == searchDepth)
		{
			cout << fixed << setprecision(0) << "Percent loaded: " << ((float)m / moves.size()) * 100 << "%\n";
			clock_t time1 = (clock() - time0);
			double time = time1 / (float)CLOCKS_PER_SEC;
			cout << fixed << setprecision(2) << "Time: " << time << "s\n\n";
		}

		if (moveScore > max)
		{
			bestMove = move;
			max = moveScore;
		}

	}
	if (depth == searchDepth)
	{
		cout << bestMove << endl;
		for (int i = 0; i < pieces.size(); i++)
		{
			// Pawns
			if (bestMove.length() == 4 || bestMove.length() == 6 || bestMove.length() == 9)
			{
				int file0 = char2move(bestMove.at(0));
				int rank0 = char2move(bestMove.at(1));

				if (file0 == pieces[i].file && rank0 == pieces[i].rank)
				{
					int file = char2move(bestMove.at(2));
					int rank = char2move(bestMove.at(3));

					sf::Vector2f dest = sf::Vector2f(file * tile, rank * tile);

					movePiece_ui(pieces[i], pieces, dest);
				}
			}
			// Other pieces
			else
			{
				int file0 = char2move(bestMove.at(1));
				int rank0 = char2move(bestMove.at(2));

				if (file0 == pieces[i].file && rank0 == pieces[i].rank)
				{
					int file = char2move(bestMove.at(3));
					int rank = char2move(bestMove.at(4));

					sf::Vector2f dest = sf::Vector2f(file * tile, rank * tile);

					movePiece_ui(pieces[i], pieces, dest);
				}
			}
		}
	}
	else
	{
		movePiece(bestMove, board, movelist);
	}
	return max;
}

bool is_wk_attacked(vector<vector<int> > board, vector<string>movelist)
{
	vector<string> moves = possibleBlack(board, movelist);
	string move;

	for (int i = 0; i < moves.size(); i++)
	{
		move = moves.at(i);
		switch (move.length())
		{
		case 6:
			if (move.at(5) == 'K')
				return true;
			break;

		case 7:
			if (move.at(6) == 'K')
				return true;
			break;

		default:
			break;
		}
	}
	return false;
}

bool is_bk_attacked(vector<vector<int> > board, vector<string>movelist)
{
	vector<string> moves = possibleWhite(board, movelist);
	string move;

	for (int i = 0; i < moves.size(); i++)
	{
		move = moves.at(i);
		switch (move.length())
		{
		case 6:
			if (move.at(5) == 'k')
				return true;
			break;

		case 7:
			if (move.at(6) == 'k')
				return true;
			break;

		default:
			break;
		}
	}
	return false;
}

bool is_Checkmate(vector<vector<int> > board, vector<string>movelist)
{
	vector<vector<int> > tempBoard;
	vector<string> tempList;
	vector<string> possibleMoves;
	string move;

	if (who2move(movelist) == white)
	{
		if (is_wk_attacked(board, movelist))
		{
			possibleMoves = possibleWhite(board, movelist);
			for (int m = 0; m < possibleMoves.size(); m++)
			{
				tempBoard = board;
				tempList = movelist;
				move = possibleMoves.at(m);
				movePiece(move, tempBoard, tempList);

				if (!is_wk_attacked(tempBoard, tempList))
					return false;
			}
			return true;
		}
		else
			return false;
	}
	else
	{
		if (is_bk_attacked(board, movelist))
		{
			possibleMoves = possibleBlack(board, movelist);
			for (int m = 0; m < possibleMoves.size(); m++)
			{
				tempBoard = board;
				tempList = movelist;
				move = possibleMoves.at(m);
				movePiece(move, tempBoard, tempList);

				if (!is_bk_attacked(tempBoard, tempList))
					return false;
			}
			return true;
		}
		else
			return false;
	}
}

bool castling(vector<vector<int> > board, vector<string>movelist, string side, int colour)
{
	vector<string> moves = movelist;
	string move;

	if (side == "right")
	{
		if (colour == white)
		{
			for (int m = 0; m < moves.size(); m++)
			{
				move = moves.at(m);
				if (move.at(0) == 'K')
					return false;

				else if (move.at(0) == 'R' && move.at(1) == 'h' && move.at(2) == '1')
					return false;
			}
			if (board[rank_1][file_F] == 0 && board[rank_1][file_G] == 0)
				return true;
			else
				return false;
		}
		else
		{
			for (int m = 0; m < moves.size(); m++)
			{
				move = moves.at(m);
				if (move.at(0) == 'k')
					return false;

				else if (move.at(0) == 'r' && move.at(1) == 'h' && move.at(2) == '8')
					return false;
			}
			if (board[rank_8][file_F] == 0 && board[rank_8][file_G] == 0)
				return true;
			else
				return false;
		}
	}
	else if (side == "left")
	{
		if (colour == white)
		{
			for (int m = 0; m < moves.size(); m++)
			{
				move = moves.at(m);
				if (move.at(0) == 'K')
					return false;

				else if (move.at(0) == 'R' && move.at(1) == 'a' && move.at(2) == '1')
					return false;
			}
			if (board[rank_1][file_D] == 0 && board[rank_1][file_C] == 0 && board[rank_1][file_B] == 0)
				return true;
			else
				return false;
		}
		else
		{
			for (int m = 0; m < moves.size(); m++)
			{
				move = moves.at(m);
				if (move.at(0) == 'k')
					return false;


				else if (move.at(0) == 'r' && move.at(1) == 'a' && move.at(2) == '8')
					return false;
			}
			if (board[rank_8][file_D] == 0 && board[rank_8][file_C] == 0 && board[rank_8][file_B] == 0)
				return true;
			else
				return false;
		}
	}
	else
	{
		cout << "castling function error\n";
		return false;
	}
}

vector<string> possibleWhite(vector<vector<int> > board, vector<string>movelist, vector<vector<vector<int>>> boards)
{
	vector<vector<int> > tempBoard = board;
	vector<string> tempList = movelist;

	vector<string>WP = possibleWP(board, boards, movelist);
	vector<string>WR = possibleWR(board);
	vector<string>WN = possibleWN(board);
	vector<string>WB = possibleWB(board);
	vector<string>WQ = possibleWQ(board);
	vector<string>WK = possibleWK(board, movelist);
	vector<string> whiteMoves;

	whiteMoves.reserve(WP.size() + WR.size() + WN.size() + WB.size() + WQ.size() + WK.size());
	whiteMoves.insert(whiteMoves.end(), WP.begin(), WP.end());
	whiteMoves.insert(whiteMoves.end(), WR.begin(), WR.end());
	whiteMoves.insert(whiteMoves.end(), WN.begin(), WN.end());
	whiteMoves.insert(whiteMoves.end(), WB.begin(), WB.end());
	whiteMoves.insert(whiteMoves.end(), WQ.begin(), WQ.end());
	whiteMoves.insert(whiteMoves.end(), WK.begin(), WK.end());

	return whiteMoves;
}
vector<string> possibleWP(vector<vector<int> > board, vector<vector<vector<int>>> boards, vector<string>movelist)
{
	vector<string> moves;
	vector<vector<int> > preBoard;
	int ply = boards.size() - 1;
	if (ply > 0)
	{
		preBoard = boards[ply - 1];
	}


	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == P)
			{
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;

				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// No capture
				if (rank0 > rank_8 && board[rank0 - 1][file0] == 0)
				{
					toPos = toStringMove(rank0 - 1, file0);
					move = fromPos + toPos;
					moves.push_back(move);
				}
				if (rank0 == rank_2 && board[rank0 - 2][file0] == 0 && board[rank0 - 1][file0] == 0)
				{
					toPos = toStringMove(rank0 - 2, file0);
					move = fromPos + toPos;
					moves.push_back(move);
				}

				// Right-capture
				if (rank0 > rank_8 && file0 < file_H && board[rank0 - 1][file0 + 1] < 0)
				{
					toPos = toStringMove(rank0 - 1, file0 + 1);
					capture = toCharPiece(board[rank0 - 1][file0 + 1]);
					move = fromPos + toPos + "-" + capture;
					moves.push_back(move);
				}

				// Left-capture
				if (rank0 > rank_8 && file0 > file_A && board[rank0 - 1][file0 - 1] < 0)
				{
					toPos = toStringMove(rank0 - 1, file0 - 1);
					capture = toCharPiece(board[rank0 - 1][file0 - 1]);
					move = fromPos + toPos + "-" + capture;
					moves.push_back(move);
				}

				// En passant
				if (ply > 0)
				{
					if (rank0 == rank_5 && file0 > file_A && board[rank0][file0 - 1] == p && preBoard[rank0 - 1][file0 - 1] == 0 && preBoard[rank0 - 2][file0 - 1] == p)
					{
						toPos = toStringMove(rank0 - 1, file0 - 1);
						move = fromPos + toPos + "-enPa";
						moves.push_back(move);
					}
					if (rank0 == rank_5 && file0 < file_H && board[rank0][file0 + 1] == p && preBoard[rank0 - 1][file0 + 1] == 0 && preBoard[rank0 - 2][file0 + 1] == p)
					{
						toPos = toStringMove(rank0 - 1, file0 + 1);
						move = fromPos + toPos + "-enPa";
						moves.push_back(move);
					}
				}
			}
		}
	}
	return moves;
}
vector<string> possibleWR(vector<vector<int> > board)
{
	vector<string> moves;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == R)
			{
				char piece = toCharPiece(R);
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;
				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// Movement up
				if (rank0 > rank_8)
				{
					for (int x = rank0 - 1; x >= rank_8; x--)
					{
						// Std move
						if (board[x][file0] == 0)
						{
							toPos = toStringMove(x, file0);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						// Capture
						else if (board[x][file0] < 0)
						{
							toPos = toStringMove(x, file0);
							capture = toCharPiece(board[x][file0]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						// If position on board is occupied by the same colour, break
						else
							break;
					}
				}

				// Movement down
				if (rank0 < rank_1)
				{
					for (int x = rank0 + 1; x <= rank_1; x++)
					{
						if (board[x][file0] == 0)
						{
							toPos = toStringMove(x, file0);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}

						else if (board[x][file0] < 0)
						{
							toPos = toStringMove(x, file0);
							move = piece + fromPos + toPos;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}

				// Movement right
				if (file0 < file_H)
				{
					for (int x = file0 + 1; x <= file_H; x++)
					{
						if (board[rank0][x] == 0)
						{
							toPos = toStringMove(rank0, x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0][x] < 0)
						{
							toPos = toStringMove(rank0, x);
							capture = toCharPiece(board[rank0][x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}

				// Movement left
				if (file0 > file_A)
				{
					for (int x = file0 - 1; x >= file_A; x--)
					{
						if (board[rank0][x] == 0)
						{
							toPos = toStringMove(rank0, x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0][x] < 0)
						{
							toPos = toStringMove(rank0, x);
							capture = toCharPiece(board[rank0][x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}
			}
		}
	}
	return moves;
}
vector<string> possibleWN(vector<vector<int> > board)
{
	vector<string> moves;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == N)
			{
				char piece = toCharPiece(N);
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;
				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// Long left move
				if (file0 > file_B)
				{
					if (rank0 == rank_8)
					{
						if (board[rank0 + 1][file0 - 2] == 0)
						{
							toPos = toStringMove(rank0 + 1, file0 - 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						// Capture
						else if (board[rank0 + 1][file0 - 2] < 0)
						{
							toPos = toStringMove(rank0 + 1, file0 - 2);
							capture = toCharPiece(board[rank0 + 1][file0 - 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					else if (rank0 == rank_1)
					{
						if (board[rank0 - 1][file0 - 2] == 0)
						{
							toPos = toStringMove(rank0 - 1, file0 - 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						// Capture
						else if (board[rank0 - 1][file0 - 2] < 0)
						{
							toPos = toStringMove(rank0 - 1, file0 - 2);
							capture = toCharPiece(board[rank0 - 1][file0 - 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					// Between rank 1 and 8
					else
					{
						if (board[rank0 + 1][file0 - 2] == 0)
						{
							toPos = toStringMove(rank0 + 1, file0 - 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 1][file0 - 2] < 0)
						{
							toPos = toStringMove(rank0 + 1, file0 - 2);
							capture = toCharPiece(board[rank0 + 1][file0 - 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
						if (board[rank0 - 1][file0 - 2] == 0)
						{
							toPos = toStringMove(rank0 - 1, file0 - 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 1][file0 - 2] < 0)
						{
							toPos = toStringMove(rank0 - 1, file0 - 2);
							capture = toCharPiece(board[rank0 - 1][file0 - 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
				}
				// Short left move
				if (file0 > file_A)
				{
					if (rank0 == rank_8 || rank0 == rank_7)
					{
						if (board[rank0 + 2][file0 - 1] == 0)
						{
							toPos = toStringMove(rank0 + 2, file0 - 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 2][file0 - 1] < 0)
						{
							toPos = toStringMove(rank0 + 2, file0 - 1);
							capture = toCharPiece(board[rank0 + 2][file0 - 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					else if (rank0 == rank_1 || rank0 == rank_2)
					{
						if (board[rank0 - 2][file0 - 1] == 0)
						{
							toPos = toStringMove(rank0 - 2, file0 - 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 2][file0 - 1] < 0)
						{
							toPos = toStringMove(rank0 - 2, file0 - 1);
							capture = toCharPiece(board[rank0 - 2][file0 - 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					// Between rank 2 and 7
					else
					{
						if (board[rank0 + 2][file0 - 1] == 0)
						{
							toPos = toStringMove(rank0 + 2, file0 - 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 2][file0 - 1] < 0)
						{
							toPos = toStringMove(rank0 + 2, file0 - 1);
							capture = toCharPiece(board[rank0 + 2][file0 - 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
						if (board[rank0 - 2][file0 - 1] == 0)
						{
							toPos = toStringMove(rank0 - 2, file0 - 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 2][file0 - 1] < 0)
						{
							toPos = toStringMove(rank0 - 2, file0 - 1);
							capture = toCharPiece(board[rank0 - 2][file0 - 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
				}
				// Long right move
				if (file0 < file_G)
				{
					if (rank0 == rank_8)
					{
						if (board[rank0 + 1][file0 + 2] == 0)
						{
							toPos = toStringMove(rank0 + 1, file0 + 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 1][file0 + 2] < 0)
						{
							toPos = toStringMove(rank0 + 1, file0 + 2);
							capture = toCharPiece(board[rank0 + 1][file0 + 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					else if (rank0 == rank_1)
					{
						if (board[rank0 - 1][file0 + 2] == 0)
						{
							toPos = toStringMove(rank0 - 1, file0 + 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 1][file0 + 2] < 0)
						{
							toPos = toStringMove(rank0 - 1, file0 + 2);
							capture = toCharPiece(board[rank0 - 1][file0 + 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					// Rank between 1 and 8
					else
					{
						if (board[rank0 + 1][file0 + 2] == 0)
						{
							toPos = toStringMove(rank0 + 1, file0 + 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 1][file0 + 2] < 0)
						{
							toPos = toStringMove(rank0 + 1, file0 + 2);
							capture = toCharPiece(board[rank0 + 1][file0 + 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
						if (board[rank0 - 1][file0 + 2] == 0)
						{
							toPos = toStringMove(rank0 - 1, file0 + 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 1][file0 + 2] < 0)
						{
							toPos = toStringMove(rank0 - 1, file0 + 2);
							capture = toCharPiece(board[rank0 - 1][file0 + 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
				}
				// Short right move
				if (file0 < file_H)
				{
					if (rank0 == rank_8 || rank0 == rank_7)
					{
						if (board[rank0 + 2][file0 + 1] == 0)
						{
							toPos = toStringMove(rank0 + 2, file0 + 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 2][file0 + 1] < 0)
						{
							toPos = toStringMove(rank0 + 2, file0 + 1);
							capture = toCharPiece(board[rank0 + 2][file0 + 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					else if (rank0 == rank_1 || rank0 == rank_2)
					{
						if (board[rank0 - 2][file0 + 1] == 0)
						{
							toPos = toStringMove(rank0 - 2, file0 + 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 2][file0 + 1] < 0)
						{
							toPos = toStringMove(rank0 - 2, file0 + 1);
							capture = toCharPiece(board[rank0 - 2][file0 + 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					// Rank between 2 and 7
					else
					{
						if (board[rank0 + 2][file0 + 1] == 0)
						{
							toPos = toStringMove(rank0 + 2, file0 + 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 2][file0 + 1] < 0)
						{
							toPos = toStringMove(rank0 + 2, file0 + 1);
							capture = toCharPiece(board[rank0 + 2][file0 + 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
						if (board[rank0 - 2][file0 + 1] == 0)
						{
							toPos = toStringMove(rank0 - 2, file0 + 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 2][file0 + 1] < 0)
						{
							toPos = toStringMove(rank0 - 2, file0 + 1);
							capture = toCharPiece(board[rank0 - 2][file0 + 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
				}
			}
		}
	}
	return moves;
}
vector<string> possibleWB(vector<vector<int> > board)
{
	vector<string> moves;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == B)
			{
				char piece = toCharPiece(B);
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;
				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// North east
				if (rank0 != rank_8 && file0 != file_H)
				{
					int x = 1;
					while (rank0 - x >= rank_8 && file0 + x <= file_H && board[rank0 - x][file0 + x] <= 0)
					{
						if (board[rank0 - x][file0 + x] == 0)
						{
							toPos = toStringMove(rank0 - x, file0 + x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - x][file0 + x] < 0)
						{
							toPos = toStringMove(rank0 - x, file0 + x);
							capture = toCharPiece(board[rank0 - x][file0 + x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// North west
				if (rank0 != rank_8 && file0 != file_A)
				{
					int x = 1;
					while (rank0 - x >= rank_8 && file0 - x >= file_A && board[rank0 - x][file0 - x] <= 0)
					{
						if (board[rank0 - x][file0 - x] == 0)
						{
							toPos = toStringMove(rank0 - x, file0 - x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - x][file0 - x] < 0)
						{
							toPos = toStringMove(rank0 - x, file0 - x);
							capture = toCharPiece(board[rank0 - x][file0 - x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// South east
				if (rank0 != rank_1 && file0 != file_H)
				{
					int x = 1;
					while (rank0 + x <= rank_1 && file0 + x <= file_H && board[rank0 + x][file0 + x] <= 0)
					{
						if (board[rank0 + x][file0 + x] == 0)
						{
							toPos = toStringMove(rank0 + x, file0 + x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + x][file0 + x] < 0)
						{
							toPos = toStringMove(rank0 + x, file0 + x);
							capture = toCharPiece(board[rank0 + x][file0 + x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// South west
				if (rank0 != rank_1 && file0 != file_A)
				{
					int x = 1;
					while (rank0 + x <= rank_1 && file0 - x >= file_A && board[rank0 + x][file0 - x] <= 0)
					{
						if (board[rank0 + x][file0 - x] == 0)
						{
							toPos = toStringMove(rank0 + x, file0 - x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + x][file0 - x] < 0)
						{
							toPos = toStringMove(rank0 + x, file0 - x);
							capture = toCharPiece(board[rank0 + x][file0 - x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
			}
		}
	}
	return moves;
}
vector<string> possibleWQ(vector<vector<int> > board)
{
	vector<string> moves;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == Q)
			{
				char piece = toCharPiece(Q);
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;
				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// North east
				if (rank0 != rank_8 && file0 != file_H)
				{
					int x = 1;
					while (rank0 - x >= rank_8 && file0 + x <= file_H && board[rank0 - x][file0 + x] <= 0)
					{
						if (board[rank0 - x][file0 + x] == 0)
						{
							toPos = toStringMove(rank0 - x, file0 + x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - x][file0 + x] < 0)
						{
							toPos = toStringMove(rank0 - x, file0 + x);
							capture = toCharPiece(board[rank0 - x][file0 + x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// North west
				if (rank0 != rank_8 && file0 != file_A)
				{
					int x = 1;
					while (rank0 - x >= rank_8 && file0 - x >= file_A && board[rank0 - x][file0 - x] <= 0)
					{
						if (board[rank0 - x][file0 - x] == 0)
						{
							toPos = toStringMove(rank0 - x, file0 - x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - x][file0 - x] < 0)
						{
							toPos = toStringMove(rank0 - x, file0 - x);
							capture = toCharPiece(board[rank0 - x][file0 - x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// South east
				if (rank0 != rank_1 && file0 != file_H)
				{
					int x = 1;
					while (rank0 + x <= rank_1 && file0 + x <= file_H && board[rank0 + x][file0 + x] <= 0)
					{
						if (board[rank0 + x][file0 + x] == 0)
						{
							toPos = toStringMove(rank0 + x, file0 + x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + x][file0 + x] < 0)
						{
							toPos = toStringMove(rank0 + x, file0 + x);
							capture = toCharPiece(board[rank0 + x][file0 + x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// South west
				if (rank0 != rank_1 && file0 != file_A)
				{
					int x = 1;
					while (rank0 + x <= rank_1 && file0 - x >= file_A && board[rank0 + x][file0 - x] <= 0)
					{
						if (board[rank0 + x][file0 - x] == 0)
						{
							toPos = toStringMove(rank0 + x, file0 - x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + x][file0 - x] < 0)
						{
							toPos = toStringMove(rank0 + x, file0 - x);
							capture = toCharPiece(board[rank0 + x][file0 - x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// Movement up
				if (rank0 > rank_8)
				{
					for (int x = rank0 - 1; x >= rank_8; x--)
					{
						// Std move
						if (board[x][file0] == 0)
						{
							toPos = toStringMove(x, file0);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						// Capture
						else if (board[x][file0] < 0)
						{
							toPos = toStringMove(x, file0);
							capture = toCharPiece(board[x][file0]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						// If position on board is occupied by the same colour, break
						else
							break;
					}
				}

				// Movement down
				if (rank0 < rank_1)
				{
					for (int x = rank0 + 1; x <= rank_1; x++)
					{
						if (board[x][file0] == 0)
						{
							toPos = toStringMove(x, file0);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						// Capture
						else if (board[x][file0] < 0)
						{
							toPos = toStringMove(x, file0);
							capture = toCharPiece(board[x][file0]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}

				// Movement right
				if (file0 < file_H)
				{
					for (int x = file0 + 1; x <= file_H; x++)
					{
						if (board[rank0][x] == 0)
						{
							toPos = toStringMove(rank0, x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0][x] < 0)
						{
							toPos = toStringMove(rank0, x);
							capture = toCharPiece(board[rank0][x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}

				// Movement left
				if (file0 > file_A)
				{
					for (int x = file0 - 1; x >= file_A; x--)
					{
						if (board[rank0][x] == 0)
						{
							toPos = toStringMove(rank0, x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0][x] < 0)
						{
							toPos = toStringMove(rank0, x);
							capture = toCharPiece(board[rank0][x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}
			}
		}
	}
	return moves;
}
vector<string> possibleWK(vector<vector<int> > board, vector<string>movelist)
{
	vector<string> moves;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == K)
			{
				char piece = toCharPiece(K);
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;
				bool right_castling = castling(board, movelist, "right", white) == true ? true : false;
				bool left_castling = castling(board, movelist, "left", white) == true ? true : false;
				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// Up 
				if (rank0 != rank_8)
				{
					if (board[rank0 - 1][file0] == 0)
					{
						toPos = toStringMove(rank0 - 1, file0);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 - 1][file0] < 0)
					{
						toPos = toStringMove(rank0 - 1, file0);
						capture = toCharPiece(board[rank0 - 1][file0]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// Down
				if (rank0 != rank_1)
				{
					if (board[rank0 + 1][file0] == 0)
					{
						toPos = toStringMove(rank0 + 1, file0);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 + 1][file0] < 0)
					{
						toPos = toStringMove(rank0 + 1, file0);
						capture = toCharPiece(board[rank0 + 1][file0]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// Right
				if (file0 != file_H)
				{
					if (board[rank0][file0 + 1] == 0)
					{
						toPos = toStringMove(rank0, file0 + 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0][file0 + 1] < 0)
					{
						toPos = toStringMove(rank0, file0 + 1);
						capture = toCharPiece(board[rank0][file0 + 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// Left
				if (file0 != file_A)
				{
					if (board[rank0][file0 - 1] == 0)
					{
						toPos = toStringMove(rank0, file0 - 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0][file0 - 1] < 0)
					{
						toPos = toStringMove(rank0, file0 - 1);
						capture = toCharPiece(board[rank0][file0 - 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// North east
				if (rank0 != rank_8 && file0 != file_H)
				{
					if (board[rank0 - 1][file0 + 1] == 0)
					{
						toPos = toStringMove(rank0 - 1, file0 + 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 - 1][file0 + 1] < 0)
					{
						toPos = toStringMove(rank0 - 1, file0 + 1);
						capture = toCharPiece(board[rank0 - 1][file0 + 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// North west
				if (rank0 != rank_8 && file0 != file_A)
				{
					if (board[rank0 - 1][file0 - 1] == 0)
					{
						toPos = toStringMove(rank0 - 1, file0 - 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 - 1][file0 - 1] < 0)
					{
						toPos = toStringMove(rank0 - 1, file0 - 1);
						capture = toCharPiece(board[rank0 - 1][file0 - 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// South west
				if (rank0 != rank_1 && file0 != file_A)
				{
					if (board[rank0 + 1][file0 - 1] == 0)
					{
						toPos = toStringMove(rank0 + 1, file0 - 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 + 1][file0 - 1] < 0)
					{
						toPos = toStringMove(rank0 + 1, file0 - 1);
						capture = toCharPiece(board[rank0 + 1][file0 - 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// South east
				if (rank0 != rank_1 && file0 != file_H)
				{
					if (board[rank0 + 1][file0 + 1] == 0)
					{
						toPos = toStringMove(rank0 + 1, file0 + 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 + 1][file0 + 1] < 0)
					{
						toPos = toStringMove(rank0 + 1, file0 + 1);
						capture = toCharPiece(board[rank0 + 1][file0 + 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				if (right_castling == true)
				{
					toPos = toStringMove(rank_1, file_G);
					move = piece + fromPos + toPos + "-" + "rcast";
					moves.push_back(move);
				}
				if (left_castling == true)
				{
					toPos = toStringMove(rank_1, file_C);
					move = piece + fromPos + toPos + "-" + "lcast";
					moves.push_back(move);
				}
			}
		}
	}
	return moves;
}

vector<string> possibleBlack(vector<vector<int> > board, vector<string>movelist, vector<vector<vector<int>>> boards)
{
	vector<vector<int> > tempBoard = board;
	vector<string> tempList = movelist;

	vector<string>BP = possibleBP(board, boards, movelist);
	vector<string>BR = possibleBR(board);
	vector<string>BN = possibleBN(board);
	vector<string>BB = possibleBB(board);
	vector<string>BQ = possibleBQ(board);
	vector<string>BK = possibleBK(board, movelist);
	vector<string> blackMoves;

	blackMoves.reserve(BP.size() + BR.size() + BN.size() + BB.size() + BQ.size() + BK.size());
	blackMoves.insert(blackMoves.end(), BP.begin(), BP.end());
	blackMoves.insert(blackMoves.end(), BR.begin(), BR.end());
	blackMoves.insert(blackMoves.end(), BN.begin(), BN.end());
	blackMoves.insert(blackMoves.end(), BB.begin(), BB.end());
	blackMoves.insert(blackMoves.end(), BQ.begin(), BQ.end());
	blackMoves.insert(blackMoves.end(), BK.begin(), BK.end());

	return blackMoves;
}
vector<string> possibleBP(vector<vector<int> > board, vector<vector<vector<int>>> boards, vector<string>movelist)
{
	vector<string> moves;
	vector<vector<int> > preBoard;
	int ply = boards.size() - 1;

	if (ply > 0)
	{
		preBoard = boards[ply - 1];
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == p)
			{
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;
				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// No capture
				if (rank0 < rank_1 && board[rank0 + 1][file0] == 0)
				{
					toPos = toStringMove(rank0 + 1, file0);
					move = fromPos + toPos;
					moves.push_back(move);
				}
				if (rank0 == rank_7 && board[rank0 + 2][file0] == 0 && board[rank0 + 1][file0] == 0)
				{
					toPos = toStringMove(rank0 + 2, file0);
					move = fromPos + toPos;
					moves.push_back(move);
				}

				// Right-capture
				if (rank0 < rank_1 && file0 < file_H && board[rank0 + 1][file0 + 1] > 0)
				{
					toPos = toStringMove(rank0 + 1, file0 + 1);
					capture = toCharPiece(board[rank0 + 1][file0 + 1]);
					move = fromPos + toPos + "-" + capture;
					moves.push_back(move);
				}

				// Left-capture
				if (rank0 < rank_1 && file0 > file_A && board[rank0 + 1][file0 - 1] > 0)
				{
					toPos = toStringMove(rank0 + 1, file0 - 1);
					capture = toCharPiece(board[rank0 + 1][file0 - 1]);
					move = fromPos + toPos + "-" + capture;
					moves.push_back(move);
				}
				// En passant
				if (ply > 0)
				{
					if (rank0 == rank_4 && file0 > file_A && board[rank0][file0 - 1] == P && preBoard[rank0 + 1][file0 - 1] == 0 && preBoard[rank0 + 2][file0 - 1] == P)
					{
						toPos = toStringMove(rank0 + 1, file0 - 1);
						move = fromPos + toPos + "-enPa";
						moves.push_back(move);
					}
					if (rank0 == rank_4 && file0 < file_H && board[rank0][file0 + 1] == P && preBoard[rank0 + 1][file0 + 1] == 0 && preBoard[rank0 + 2][file0 + 1] == P)
					{
						toPos = toStringMove(rank0 + 1, file0 + 1);
						move = fromPos + toPos + "-enPa";
						moves.push_back(move);
					}
				}
			}
		}
	}
	return moves;
}
vector<string> possibleBR(vector<vector<int> > board)
{
	vector<string> moves;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == r)
			{
				char piece = toCharPiece(r);
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;
				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// Movement up
				if (rank0 > rank_8)
				{
					for (int x = rank0 - 1; x >= rank_8; x--)
					{
						// Std move
						if (board[x][file0] == 0)
						{
							toPos = toStringMove(x, file0);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						// Capture
						else if (board[x][file0] > 0)
						{
							toPos = toStringMove(x, file0);
							capture = toCharPiece(board[x][file0]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						// If position on board is occupied by the same colour, break
						else
							break;
					}
				}

				// Movement down
				if (rank0 < rank_1)
				{
					for (int x = rank0 + 1; x <= rank_1; x++)
					{
						if (board[x][file0] == 0)
						{
							toPos = toStringMove(x, file0);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}

						else if (board[x][file0] > 0)
						{
							toPos = toStringMove(x, file0);
							capture = toCharPiece(board[x][file0]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}

				// Movement right
				if (file0 < file_H)
				{
					for (int x = file0 + 1; x <= file_H; x++)
					{
						if (board[rank0][x] == 0)
						{
							toPos = toStringMove(rank0, x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0][x] > 0)
						{
							toPos = toStringMove(rank0, x);
							capture = toCharPiece(board[rank0][x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}

				// Movement left
				if (file0 > file_A)
				{
					for (int x = file0 - 1; x >= file_A; x--)
					{
						if (board[rank0][x] == 0)
						{
							toPos = toStringMove(rank0, x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0][x] > 0)
						{
							toPos = toStringMove(rank0, x);
							capture = toCharPiece(board[rank0][x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}
			}
		}
	}
	return moves;
}
vector<string> possibleBN(vector<vector<int> > board)
{
	vector<string> moves;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == n)
			{
				char piece = toCharPiece(n);
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;
				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// Long left move
				if (file0 > file_B)
				{
					if (rank0 == rank_8)
					{
						if (board[rank0 + 1][file0 - 2] == 0)
						{
							toPos = toStringMove(rank0 + 1, file0 - 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						// Capture
						else if (board[rank0 + 1][file0 - 2] > 0)
						{
							toPos = toStringMove(rank0 + 1, file0 - 2);
							capture = toCharPiece(board[rank0 + 1][file0 - 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					else if (rank0 == rank_1)
					{
						if (board[rank0 - 1][file0 - 2] == 0)
						{
							toPos = toStringMove(rank0 - 1, file0 - 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						// Capture
						else if (board[rank0 - 1][file0 - 2] > 0)
						{
							toPos = toStringMove(rank0 - 1, file0 - 2);
							capture = toCharPiece(board[rank0 - 1][file0 - 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					// Between rank 1 and 8
					else
					{
						if (board[rank0 + 1][file0 - 2] == 0)
						{
							toPos = toStringMove(rank0 + 1, file0 - 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 1][file0 - 2] > 0)
						{
							toPos = toStringMove(rank0 + 1, file0 - 2);
							capture = toCharPiece(board[rank0 + 1][file0 - 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
						if (board[rank0 - 1][file0 - 2] == 0)
						{
							toPos = toStringMove(rank0 - 1, file0 - 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 1][file0 - 2] > 0)
						{
							toPos = toStringMove(rank0 - 1, file0 - 2);
							capture = toCharPiece(board[rank0 - 1][file0 - 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
				}
				// Short left move
				if (file0 > file_A)
				{
					if (rank0 == rank_8 || rank0 == rank_7)
					{
						if (board[rank0 + 2][file0 - 1] == 0)
						{
							toPos = toStringMove(rank0 + 2, file0 - 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 2][file0 - 1] > 0)
						{
							toPos = toStringMove(rank0 + 2, file0 - 1);
							capture = toCharPiece(board[rank0 + 2][file0 - 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					else if (rank0 == rank_1 || rank0 == rank_2)
					{
						if (board[rank0 - 2][file0 - 1] == 0)
						{
							toPos = toStringMove(rank0 - 2, file0 - 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 2][file0 - 1] > 0)
						{
							toPos = toStringMove(rank0 - 2, file0 - 1);
							capture = toCharPiece(board[rank0 - 2][file0 - 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					// Between rank 2 and 7
					else
					{
						if (board[rank0 + 2][file0 - 1] == 0)
						{
							toPos = toStringMove(rank0 + 2, file0 - 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 2][file0 - 1] > 0)
						{
							toPos = toStringMove(rank0 + 2, file0 - 1);
							capture = toCharPiece(board[rank0 + 2][file0 - 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
						if (board[rank0 - 2][file0 - 1] == 0)
						{
							toPos = toStringMove(rank0 - 2, file0 - 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 2][file0 - 1] > 0)
						{
							toPos = toStringMove(rank0 - 2, file0 - 1);
							capture = toCharPiece(board[rank0 - 2][file0 - 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
				}
				// Long right move
				if (file0 < file_G)
				{
					if (rank0 == rank_8)
					{
						if (board[rank0 + 1][file0 + 2] == 0)
						{
							toPos = toStringMove(rank0 + 1, file0 + 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 1][file0 + 2] > 0)
						{
							toPos = toStringMove(rank0 + 1, file0 + 2);
							capture = toCharPiece(board[rank0 + 1][file0 + 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					else if (rank0 == rank_1)
					{
						if (board[rank0 - 1][file0 + 2] == 0)
						{
							toPos = toStringMove(rank0 - 1, file0 + 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 1][file0 + 2] > 0)
						{
							toPos = toStringMove(rank0 - 1, file0 + 2);
							capture = toCharPiece(board[rank0 - 1][file0 + 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					// Rank between 1 and 8
					else
					{
						if (board[rank0 + 1][file0 + 2] == 0)
						{
							toPos = toStringMove(rank0 + 1, file0 + 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 1][file0 + 2] > 0)
						{
							toPos = toStringMove(rank0 + 1, file0 + 2);
							capture = toCharPiece(board[rank0 + 1][file0 + 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
						if (board[rank0 - 1][file0 + 2] == 0)
						{
							toPos = toStringMove(rank0 - 1, file0 + 2);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 1][file0 + 2] > 0)
						{
							toPos = toStringMove(rank0 - 1, file0 + 2);
							capture = toCharPiece(board[rank0 - 1][file0 + 2]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
				}
				// Short right move
				if (file0 < file_H)
				{
					if (rank0 == rank_8 || rank0 == rank_7)
					{
						if (board[rank0 + 2][file0 + 1] == 0)
						{
							toPos = toStringMove(rank0 + 2, file0 + 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 2][file0 + 1] > 0)
						{
							toPos = toStringMove(rank0 + 2, file0 + 1);
							capture = toCharPiece(board[rank0 + 2][file0 + 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					else if (rank0 == rank_1 || rank0 == rank_2)
					{
						if (board[rank0 - 2][file0 + 1] == 0)
						{
							toPos = toStringMove(rank0 - 2, file0 + 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 2][file0 + 1] > 0)
						{
							toPos = toStringMove(rank0 - 2, file0 + 1);
							capture = toCharPiece(board[rank0 - 2][file0 + 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
					// Rank between 2 and 7
					else
					{
						if (board[rank0 + 2][file0 + 1] == 0)
						{
							toPos = toStringMove(rank0 + 2, file0 + 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + 2][file0 + 1] > 0)
						{
							toPos = toStringMove(rank0 + 2, file0 + 1);
							capture = toCharPiece(board[rank0 + 2][file0 + 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
						if (board[rank0 - 2][file0 + 1] == 0)
						{
							toPos = toStringMove(rank0 - 2, file0 + 1);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - 2][file0 + 1] > 0)
						{
							toPos = toStringMove(rank0 - 2, file0 + 1);
							capture = toCharPiece(board[rank0 - 2][file0 + 1]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
						}
					}
				}
			}
		}
	}
	return moves;
}
vector<string> possibleBB(vector<vector<int> > board)
{
	vector<string> moves;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == b)
			{
				char piece = toCharPiece(b);
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;
				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// North east
				if (rank0 != rank_8 && file0 != file_H)
				{
					int x = 1;
					while (rank0 - x >= rank_8 && file0 + x <= file_H && board[rank0 - x][file0 + x] >= 0)
					{
						if (board[rank0 - x][file0 + x] == 0)
						{
							toPos = toStringMove(rank0 - x, file0 + x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - x][file0 + x] > 0)
						{
							toPos = toStringMove(rank0 - x, file0 + x);
							capture = toCharPiece(board[rank0 - x][file0 + x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// North west
				if (rank0 != rank_8 && file0 != file_A)
				{
					int x = 1;
					while (rank0 - x >= rank_8 && file0 - x >= file_A && board[rank0 - x][file0 - x] >= 0)
					{
						if (board[rank0 - x][file0 - x] == 0)
						{
							toPos = toStringMove(rank0 - x, file0 - x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - x][file0 - x] > 0)
						{
							toPos = toStringMove(rank0 - x, file0 - x);
							capture = toCharPiece(board[rank0 - x][file0 - x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// South east
				if (rank0 != rank_1 && file0 != file_H)
				{
					int x = 1;
					while (rank0 + x <= rank_1 && file0 + x <= file_H && board[rank0 + x][file0 + x] >= 0)
					{
						if (board[rank0 + x][file0 + x] == 0)
						{
							toPos = toStringMove(rank0 + x, file0 + x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + x][file0 + x] > 0)
						{
							toPos = toStringMove(rank0 + x, file0 + x);
							capture = toCharPiece(board[rank0 + x][file0 + x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// South west
				if (rank0 != rank_1 && file0 != file_A)
				{
					int x = 1;
					while (rank0 + x <= rank_1 && file0 - x >= file_A && board[rank0 + x][file0 - x] >= 0)
					{
						if (board[rank0 + x][file0 - x] == 0)
						{
							toPos = toStringMove(rank0 + x, file0 - x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + x][file0 - x] > 0)
						{
							toPos = toStringMove(rank0 + x, file0 - x);
							capture = toCharPiece(board[rank0 + x][file0 - x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
			}
		}
	}
	return moves;
}
vector<string> possibleBQ(vector<vector<int> > board)
{
	vector<string> moves;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == q)
			{
				char piece = toCharPiece(q);
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;
				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// North east
				if (rank0 != rank_8 && file0 != file_H)
				{
					int x = 1;
					while (rank0 - x >= rank_8 && file0 + x <= file_H && board[rank0 - x][file0 + x] >= 0)
					{
						if (board[rank0 - x][file0 + x] == 0)
						{
							toPos = toStringMove(rank0 - x, file0 + x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - x][file0 + x] > 0)
						{
							toPos = toStringMove(rank0 - x, file0 + x);
							capture = toCharPiece(board[rank0 - x][file0 + x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// North west
				if (rank0 != rank_8 && file0 != file_A)
				{
					int x = 1;
					while (rank0 - x >= rank_8 && file0 - x >= file_A && board[rank0 - x][file0 - x] >= 0)
					{
						if (board[rank0 - x][file0 - x] == 0)
						{
							toPos = toStringMove(rank0 - x, file0 - x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 - x][file0 - x] > 0)
						{
							toPos = toStringMove(rank0 - x, file0 - x);
							capture = toCharPiece(board[rank0 - x][file0 - x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// South east
				if (rank0 != rank_1 && file0 != file_H)
				{
					int x = 1;
					while (rank0 + x <= rank_1 && file0 + x <= file_H && board[rank0 + x][file0 + x] >= 0)
					{
						if (board[rank0 + x][file0 + x] == 0)
						{
							toPos = toStringMove(rank0 + x, file0 + x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + x][file0 + x] > 0)
						{
							toPos = toStringMove(rank0 + x, file0 + x);
							capture = toCharPiece(board[rank0 + x][file0 + x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// South west
				if (rank0 != rank_1 && file0 != file_A)
				{
					int x = 1;
					while (rank0 + x <= rank_1 && file0 - x >= file_A && board[rank0 + x][file0 - x] >= 0)
					{
						if (board[rank0 + x][file0 - x] == 0)
						{
							toPos = toStringMove(rank0 + x, file0 - x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0 + x][file0 - x] > 0)
						{
							toPos = toStringMove(rank0 + x, file0 - x);
							capture = toCharPiece(board[rank0 + x][file0 - x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						x++;
					}
				}
				// Movement up
				if (rank0 > rank_8)
				{
					for (int x = rank0 - 1; x >= rank_8; x--)
					{
						// Std move
						if (board[x][file0] == 0)
						{
							toPos = toStringMove(x, file0);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						// Capture
						else if (board[x][file0] > 0)
						{
							toPos = toStringMove(x, file0);
							capture = toCharPiece(board[x][file0]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						// If position on board is occupied by the same colour, break
						else
							break;
					}
				}

				// Movement down
				if (rank0 < rank_1)
				{
					for (int x = rank0 + 1; x <= rank_1; x++)
					{
						if (board[x][file0] == 0)
						{
							toPos = toStringMove(x, file0);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}

						else if (board[x][file0] > 0)
						{
							toPos = toStringMove(x, file0);
							capture = toCharPiece(board[x][file0]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}

				// Movement right
				if (file0 < file_H)
				{
					for (int x = file0 + 1; x <= file_H; x++)
					{
						if (board[rank0][x] == 0)
						{
							toPos = toStringMove(rank0, x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0][x] > 0)
						{
							toPos = toStringMove(rank0, x);
							capture = toCharPiece(board[rank0][x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}

				// Movement left
				if (file0 > file_A)
				{
					for (int x = file0 - 1; x >= file_A; x--)
					{
						if (board[rank0][x] == 0)
						{
							toPos = toStringMove(rank0, x);
							move = piece + fromPos + toPos;
							moves.push_back(move);
						}
						else if (board[rank0][x] > 0)
						{
							toPos = toStringMove(rank0, x);
							capture = toCharPiece(board[rank0][x]);
							move = piece + fromPos + toPos + "-" + capture;
							moves.push_back(move);
							break;
						}
						else
							break;
					}
				}
			}
		}
	}
	return moves;
}
vector<string> possibleBK(vector<vector<int> > board, vector<string>movelist)
{
	vector<string> moves;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] == k)
			{
				char piece = toCharPiece(k);
				string fromPos;
				string toPos;
				string move;
				string capture;
				int file0;
				int rank0;
				bool right_castling = castling(board, movelist, "right", black) == true ? true : false;
				bool left_castling = castling(board, movelist, "left", black) == true ? true : false;
				rank0 = i;
				file0 = j;
				fromPos = toStringMove(rank0, file0);

				// Up 
				if (rank0 != rank_8)
				{
					if (board[rank0 - 1][file0] == 0)
					{
						toPos = toStringMove(rank0 - 1, file0);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 - 1][file0] > 0)
					{
						toPos = toStringMove(rank0 - 1, file0);
						capture = toCharPiece(board[rank0 - 1][file0]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// Down
				if (rank0 != rank_1)
				{
					if (board[rank0 + 1][file0] == 0)
					{
						toPos = toStringMove(rank0 + 1, file0);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 + 1][file0] > 0)
					{
						toPos = toStringMove(rank0 + 1, file0);
						capture = toCharPiece(board[rank0 + 1][file0]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// Right
				if (file0 != file_H)
				{
					if (board[rank0][file0 + 1] == 0)
					{
						toPos = toStringMove(rank0, file0 + 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0][file0 + 1] > 0)
					{
						toPos = toStringMove(rank0, file0 + 1);
						capture = toCharPiece(board[rank0][file0 + 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// Left
				if (file0 != file_A)
				{
					if (board[rank0][file0 - 1] == 0)
					{
						toPos = toStringMove(rank0, file0 - 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0][file0 - 1] > 0)
					{
						toPos = toStringMove(rank0, file0 - 1);
						capture = toCharPiece(board[rank0][file0 - 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// North east
				if (rank0 != rank_8 && file0 != file_H)
				{
					if (board[rank0 - 1][file0 + 1] == 0)
					{
						toPos = toStringMove(rank0 - 1, file0 + 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 - 1][file0 + 1] > 0)
					{
						toPos = toStringMove(rank0 - 1, file0 + 1);
						capture = toCharPiece(board[rank0 - 1][file0 + 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// North west
				if (rank0 != rank_8 && file0 != file_A)
				{
					if (board[rank0 - 1][file0 - 1] == 0)
					{
						toPos = toStringMove(rank0 - 1, file0 - 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 - 1][file0 - 1] > 0)
					{
						toPos = toStringMove(rank0 - 1, file0 - 1);
						capture = toCharPiece(board[rank0 - 1][file0 - 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// South west
				if (rank0 != rank_1 && file0 != file_A)
				{
					if (board[rank0 + 1][file0 - 1] == 0)
					{
						toPos = toStringMove(rank0 + 1, file0 - 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 + 1][file0 - 1] > 0)
					{
						toPos = toStringMove(rank0 + 1, file0 - 1);
						capture = toCharPiece(board[rank0 + 1][file0 - 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}
				// South east
				if (rank0 != rank_1 && file0 != file_H)
				{
					if (board[rank0 + 1][file0 + 1] == 0)
					{
						toPos = toStringMove(rank0 + 1, file0 + 1);
						move = piece + fromPos + toPos;
						moves.push_back(move);
					}
					else if (board[rank0 + 1][file0 + 1] > 0)
					{
						toPos = toStringMove(rank0 + 1, file0 + 1);
						capture = toCharPiece(board[rank0 + 1][file0 + 1]);
						move = piece + fromPos + toPos + "-" + capture;
						moves.push_back(move);
					}
				}

				// Castling 
				if (right_castling == true)
				{
					toPos = toStringMove(rank_8, file_G);
					move = piece + fromPos + toPos + "-" + "rcast";
					moves.push_back(move);
				}
				if (left_castling == true)
				{
					toPos = toStringMove(rank_8, file_C);
					move = piece + fromPos + toPos + "-" + "lcast";
					moves.push_back(move);
				}
			}
		}
	}
	return moves;
}
