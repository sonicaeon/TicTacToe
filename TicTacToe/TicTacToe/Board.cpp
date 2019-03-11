#include "pch.h"
#include "Board.h"

using namespace std;

namespace TicTacToe
{	
	Board::Board() :
		turn(MAX), move(-1), depth(0), alpha(-INF), beta(+INF), board(), parent(nullptr)
	{
		Minimax();
	}

	Board::Board(const Board *parent, smallint move, smallint alpha, smallint beta) :
		turn(-parent->turn), move(move), depth(parent->depth + 1), alpha(alpha), beta(beta), parent(parent)
	{
		copy(begin(parent->board), end(parent->board), board);
		board[move] = parent->turn;
		bool winState, drawState;
		winState = WinState();
		drawState = (depth == MAX_POSITIONS);

		if (winState || drawState)
		{
			// Game just ended either by draw or someone won
			if (winState)
			{
				// X or O won
				value = parent->turn * (10 - depth);
			}
			else
			{
				// Draw
				value = ZERO;
			}
		}
		else
		{
			// Search for further cases
			Minimax();
		}
	}

	Board *Board::get_child(smallint choice)
	{
		if (!children[choice])
			children[choice] = new Board(this, choice, -INF, +INF);
		return children[choice];
	}

	bool Board::WinState() const
	{
		const smallint pt = parent->turn;
		switch (move)
		{
		case 0:
			return (board[1] == pt && board[2] == pt) ||
				(board[3] == pt && board[6] == pt) ||
				(board[4] == pt && board[8] == pt);
		case 1:
			return (board[0] == pt && board[2] == pt) ||
				(board[4] == pt && board[7] == pt);
		case 2:
			return (board[1] == pt && board[0] == pt) ||
				(board[5] == pt && board[8] == pt) ||
				(board[4] == pt && board[6] == pt);
		case 3:
			return (board[4] == pt && board[5] == pt) ||
				(board[0] == pt && board[6] == pt);
		case 4:
			return (board[3] == pt && board[5] == pt) ||
				(board[1] == pt && board[7] == pt) ||
				(board[0] == pt && board[8] == pt) ||
				(board[2] == pt && board[6] == pt);
		case 5:
			return (board[4] == pt && board[3] == pt) ||
				(board[2] == pt && board[8] == pt);
		case 6:
			return (board[7] == pt && board[8] == pt) ||
				(board[3] == pt && board[0] == pt) ||
				(board[4] == pt && board[2] == pt);
		case 7:
			return (board[6] == pt && board[8] == pt) ||
				(board[4] == pt && board[1] == pt);
		case 8:
			return (board[7] == pt && board[6] == pt) ||
				(board[5] == pt && board[2] == pt) ||
				(board[4] == pt && board[0] == pt);
		default:
			return false;
		}
	}

	void Board::Minimax()
	{
		if (turn == MAX)
		{
			smallint max = -INF;
			for (smallint p = 0; p < MAX_POSITIONS; ++p)
			{
				if (board[p] == ZERO)
				{
					children[p] = new Board(this, p, alpha, beta);
					if (children[p]->value > max)
					{
						max = children[p]->value;
						// alpha/beta pruning
						if (max > alpha && (alpha = max) >= beta)
							break;
					}
				}
			}
			value = max;
		}
		else
		{
			smallint min = +INF;
			for (smallint p = 0; p < MAX_POSITIONS; ++p)
			{
				if (board[p] == ZERO)
				{
					children[p] = new Board(this, p, alpha, beta);
					if (children[p]->value < min)
					{
						min = children[p]->value;
						// alpha/beta pruning
						if (min < beta && (beta = min) <= alpha)
							break;
					}
				}
			}
			value = min;
		}
	}

	Board::~Board()
	{
		for (Board *child : children)
			delete child;
	}
}