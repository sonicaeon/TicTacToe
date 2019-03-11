#pragma once

namespace TicTacToe
{
	class Board 
	{
	public:
		typedef int_fast8_t smallint;
		static const smallint MAX = 1, MIN = -1, ZERO = 0, INF = 64, MAX_POSITIONS = 9;
		Board();
		~Board();
		const smallint turn;
		const smallint move;
		const smallint depth;

		// Terminal State Evaluation Value between turns (5 <= n <= 9)
		// - Scenario where X wins: designated as a positive int when value > 0 then X has won
		//     value = MAX * (10 - depth)
		// - Scenario where O wins: designated as a negative int when value < 0 then O has won
		//     value = MIN * (10 - depth)
		// - Draw: When depth (# of turns) is on turn 9 is set to 10 results in a value of 0
		//     value = ZERO
		smallint value;
		smallint alpha, beta;
		smallint board[MAX_POSITIONS];

		// The previous TicTacToe state (parent node)
		const Board *parent;

		// Array of pointers to the child nodes
		Board *children[MAX_POSITIONS] = {};
		Board *get_child(smallint move);

		bool WinState() const;
		void Minimax();

	private:
		// Constructer for non-root node and its branches
		Board(const Board *parent, smallint move, smallint alpha, smallint beta);
	};
}