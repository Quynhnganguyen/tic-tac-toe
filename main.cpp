/**
 * This program plays tic-tac-toe on a kBoardSize by kBoardSize grid.
 * It is either two player or with a computer.
 * It uses the allegro library and must be run through the command line.
 */

#include <string>
#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

/* Width and height of the graphics window. */
const size_t kDisplaySize = 600;

/* Number of rows and columns in the game board. */
const size_t kBoardSize = 3;

/* Rate to update graphics display. */
const size_t kTargetFPS = 60;

/* Width and height of the "X" and "O" images. */
const size_t kImageSize = 200;

/* An enum which represents markers for the game board. One byte is used (char) 
 * to represent the markers (Note: this is only allowed in C++11). */
enum class Player : char { EMPTY = 0, X, O };

/* A struct containing relevant data for the Allegro library. */
struct AllegroWrapper {
	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *eventQueue;
	ALLEGRO_TIMER *timer;
	ALLEGRO_FONT *font;

	/* Constructor performs necessary Allegro initialization for the game. */
	AllegroWrapper() {
		bool initSuccessful = al_init();
		assert(initSuccessful);

		display = al_create_display(kDisplaySize, kDisplaySize);
		assert(display != NULL);

		// init addons here
		al_init_primitives_addon();
		al_install_mouse();
		al_init_font_addon();
		al_init_ttf_addon();
		al_init_image_addon();

		timer = al_create_timer(1.0 / kTargetFPS);
		eventQueue = al_create_event_queue();

		al_register_event_source(eventQueue, al_get_mouse_event_source());
		al_register_event_source(eventQueue, al_get_timer_event_source(timer));

		font = al_load_font("arial.ttf", 20, 0);
	}

	~AllegroWrapper() {
		al_destroy_display(display);
		al_destroy_event_queue(eventQueue);
		al_destroy_timer(timer);
		al_destroy_font(font);
	}
};

/* A struct containing relevant game information. */
struct gameT {
	/* The game board. Each position denotes an X, O, or empty space. */
	Player board[kBoardSize][kBoardSize]; 
	Player currPlayer;       // The player whose turn is up

	/* Graphics */
	AllegroWrapper allegro; 
	ALLEGRO_BITMAP *xImage;
	ALLEGRO_BITMAP *yImage;

	gameT() {
		/* Initialize board to contain only empty spaces. */
		memset(board, static_cast<char>(Player::EMPTY), sizeof(board));

		currPlayer = Player::X;
		xImage = al_load_bitmap("X.png");
		yImage = al_load_bitmap("O.png");
	}

	/* Sets the current player to the opposite (i.e. X->O, O->X). */
	void switchPlayer() {
		currPlayer == Player::X? currPlayer = Player::O : currPlayer = Player::X;
	}
};

/* A struct encoding a point in a two-dimensional grid.
 * In this game, it is used for a (row, col) location in the grid
 * and for an exact (x, y) pixel location on the game window. */
struct pointT {
  	size_t x, y;

	/* Utility constructor. */
	pointT(size_t x, size_t y) {
		this->x = x;
		this->y = y;
	}

	/* Default constructor sets everything to 0. */
	pointT() {
		x = y = 0;
	}
};


/* Returns the string representation of the player enum. Assumes the player is 
 * not empty (X or O). */
std::string PlayerToString(Player player) {
	return player == Player::X? "X" : "O";
}

/* Wait until the user clicks on the window. */
void WaitForClick(gameT& game) {
	while (true) {
		ALLEGRO_EVENT event;
		al_wait_for_event(game.allegro.eventQueue, &event);
		if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) break;
	}
}

/* Checks each possible "3-in-a-row" and returns the winning player. If no 
 * one has won, return the EMPTY player marker. */
// TODO: simplify function
Player GetWinner(const Player board[kBoardSize][kBoardSize]) {
	size_t xCount = 0;
	size_t oCount = 0;

	/* Check rows */
	for (size_t row = 0; row < kBoardSize; ++row) {
		xCount = 0;
		oCount = 0;
		for (size_t col = 0; col < kBoardSize; ++col) {
			if (board[row][col] == Player::X)
				++xCount;
			else if (board[row][col] == Player::O)
				++oCount;
		}

		if (xCount == kBoardSize) return Player::X;
		else if (oCount == kBoardSize) return Player::O;
	}

	/* Check columns */
	for (size_t col = 0; col < kBoardSize; ++col) {
		xCount = 0;
		oCount = 0;
		for (size_t row = 0; row < kBoardSize; ++row) {
			if (board[row][col] == Player::X)
				++xCount;
			else if (board[row][col] == Player::O)
				++oCount;
		}

		if (xCount == kBoardSize) return Player::X;
		else if (oCount == kBoardSize) return Player::O;
	}

	/* Check diagonal */
	xCount = 0;
	oCount = 0;
	for (size_t i = 0; i < kBoardSize; ++i) {
		if (board[i][i] == Player::X)
			++xCount;
		else if (board[i][i] == Player::O)
			++oCount;
	}
	if (xCount == kBoardSize) return Player::X;
	else if (oCount == kBoardSize) return Player::O;

	/* Check anti-diagonal */
	xCount = 0;
	oCount = 0;
	for (size_t i = 0; i < kBoardSize; ++i) {
		if (board[i][(kBoardSize - 1) - i] == Player::X)
			++xCount;
		else if (board[i][(kBoardSize - 1) - i] == Player::O)
			++oCount;
	}
	if (xCount == kBoardSize) return Player::X;
	else if (oCount == kBoardSize) return Player::O;

	return Player::EMPTY;
}

/* Render the game's results. Does not flip back buffer to screen. */
void RenderGameEnd(const gameT& game) {
	Player winner = GetWinner(game.board);
	std::string toPrint;
	if (winner != Player::EMPTY)
		toPrint = "Player " + PlayerToString(winner) + " has won!";
	else
		toPrint = "Cats game!";
	toPrint += " (click to exit)";

	al_draw_text(game.allegro.font, al_map_rgb(255, 255, 255), kDisplaySize / 2, kDisplaySize / 2, ALLEGRO_ALIGN_CENTRE, toPrint.c_str());
}

/* Render the game. Does not flip back buffer to screen. */
void Render(const gameT& game) {
	/* Print who's turn is up. */
	std::string message = PlayerToString(game.currPlayer) + "'s turn";
	al_draw_text(game.allegro.font, al_map_rgb(255, 255, 255), 5, 5, 0, message.c_str());

	const size_t squareSize = kDisplaySize / kBoardSize;

	/* Render grid. */
	for (size_t row = 1; row < kBoardSize; ++row) 
		al_draw_line(0, row * squareSize, kDisplaySize, row * squareSize, al_map_rgb(255, 255, 255), 0);
	
	for (size_t col = 1; col < kBoardSize; ++col) 
		al_draw_line(col * squareSize, 0, col * squareSize, kDisplaySize, al_map_rgb(255, 255, 255), 0);
	
	
	/* The new, scaled size of the "X" and "O" images. */
	const float newImageSize = ((float) kDisplaySize / kBoardSize);

	/* Print "X", "O", or nothing in each grid space. */
	for (size_t row = 0; row < kBoardSize; ++row) {
		for (size_t col = 0; col < kBoardSize; ++col) {

			/* Only draw the scaled bitmap for non-empty spaces. */
			if (game.board[row][col] != Player::EMPTY)
				al_draw_scaled_bitmap(game.board[row][col] == Player::X? game.xImage : game.yImage, 
					0, 0, kImageSize, kImageSize, col * squareSize, row * squareSize, newImageSize, newImageSize, 0);			
		}
	}
}

/* Returns a new pointT holding the row and column of the game board
 * corresponding to the x and y within mouseClick. */
pointT GetBoardLocation(const pointT& mouseClick) {
	const size_t squareSize = kDisplaySize / kBoardSize;

	for (size_t row = 0; row < kBoardSize; ++row) {

		/* If the mouseClick is within this row... */
		if (mouseClick.y >= row * squareSize && mouseClick.y <= (row + 1) * squareSize) {

			/* Find the column */
			for (size_t col = 0; col < kBoardSize; ++col) {
				if (mouseClick.x >= col * squareSize && mouseClick.x <= (col + 1) * squareSize) 
					return pointT(row, col);
			}
		}
	}

	/* If nothing was returned, the mouse click (somehow) was not on the screen.
	 * Although drastic, we report an error to the user and close the program. This
	 * should never happen as it is (at least theoretically) impossible. */
	std::cerr << "Something went wrong. Your mouse click (at point " << 
		mouseClick.x << ", " << mouseClick.y << ") does not appear to be on the game window." << std::endl;
	exit(EXIT_FAILURE);
}

/* Simple AI to find the best move in the game. 
 * The rules are (in this order) as follows: 
 * 		1. If the computer can win, make the move.
 * 		2. If the human can win on the next move, block that move.
 * 		3. Otherwise, place a piece at random. 
 */
/* TODO: Use MiniMax algorithm. */
pointT GetBestMove(const gameT& game) {
	/* Look for a winning move */
	for (size_t row = 0; row < kBoardSize; ++row) {
		for (size_t col = 0; col < kBoardSize; ++col) {
			if (game.board[row][col] == Player::EMPTY) {
				Player newBoard[kBoardSize][kBoardSize];
				memcpy(newBoard, game.board, sizeof(game.board));
				newBoard[row][col] = Player::O;
				if (GetWinner(newBoard) == Player::O) return pointT(row, col);
			}
		}
	}

	/* Look for a blocking move */
	for (size_t row = 0; row < kBoardSize; ++row) {
		for (size_t col = 0; col < kBoardSize; ++col) {
			if (game.board[row][col] == Player::EMPTY) {
				Player newBoard[kBoardSize][kBoardSize];
				memcpy(newBoard, game.board, sizeof(game.board));
				newBoard[row][col] = Player::X;
				if (GetWinner(newBoard) == Player::X) return pointT(row, col);
			}
		}
	}

	/* Return a random move */
	while (true) {
		size_t row = rand() % kBoardSize;
		size_t col = rand() % kBoardSize;
		if (game.board[row][col] == Player::EMPTY) return pointT(row, col);
	}
}

/* Returns true if the game board has any valid moves (i.e. empty spaces). */
bool HasValidMoves(const gameT& game) {
	for (size_t row = 0; row < kBoardSize; ++row) {
		for (size_t col = 0; col < kBoardSize; ++col) {
			if (game.board[row][col] == Player::EMPTY)
				return true;
		}
	}

	return false;
}

bool HasNoWinner(const gameT& game) {
	return GetWinner(game.board) == Player::EMPTY;
}

void DrawGame(gameT& game) {
	Render(game);
	al_flip_display();
	al_clear_to_color(al_map_rgb(0, 0, 0));
}

void DrawGameEnd(gameT& game) {
	Render(game);
	RenderGameEnd(game);
	al_flip_display();
}

void RunGameLoop(gameT& game, const bool useAI) {
	DrawGame(game);
	/* Continue game while there is no winner and the board isn't empty. */
	while (HasNoWinner(game) && HasValidMoves(game)) {
		ALLEGRO_EVENT event;
		al_wait_for_event(game.allegro.eventQueue, &event);

		if (game.currPlayer == Player::O && useAI) {
			pointT bestMove = GetBestMove(game);
			game.board[bestMove.x][bestMove.y] = game.currPlayer;
			game.switchPlayer();

			DrawGame(game);
		}

		else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			/* Get the row and column of the board corresponding to the player's mouse click. */
			pointT boardLocation = GetBoardLocation(pointT(event.mouse.x, event.mouse.y));

			/* If the board location has not been previously selected, set the
			 * location and switch the current player. */
			if (game.board[boardLocation.x][boardLocation.y] == Player::EMPTY) {
				game.board[boardLocation.x][boardLocation.y] = game.currPlayer;
				game.switchPlayer();
			}

			DrawGame(game);
		}
	}  

	DrawGameEnd(game);

	/* Click to end game */
	WaitForClick(game);
}

int main(int argc, const char *argv[]) {
	std::cout << "Would you like to play the computer? ";
	std::string useAIString;
	getline(std::cin, useAIString);
	gameT game;
	RunGameLoop(game, tolower(useAIString[0]) == 'y');
}
