#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "tictactoe.h"

#define SIZE       3
#define CROSS      1
#define BLANK      0
#define CIRCLE    -1
#define MAX_DEPTH  3

#define COL(i) ((i) % (SIZE))
#define ROW(i) ((i) / (SIZE))

static unsigned int  best_move[2];
static unsigned int  empty_cells;
static int           grid[SIZE][SIZE];
static char          sym[3] = {'O', '-', 'X'};

inline static int    abs_sub(int, int);
static int           eval_grid(bool);
static bool          is_game_over(void);
static int           negamax(int, int);
static void          play_round(int);
static void          print_grid(void);

inline static int abs_sub(int minuend, int subtrahend)
{
	if(!minuend)
		return 0;
	return (abs(minuend) - subtrahend) * minuend/abs(minuend);
}

static int eval_grid(bool only_win)
{
	int eval            = 0;
	int sum[2*SIZE+2]   = {0};
	int count[2*SIZE+2] = {0};

	for(unsigned int i=0; i<SIZE*SIZE; i++)
	{
		sum[i/SIZE]        += grid[i/SIZE][i%SIZE];
		sum[i/SIZE+SIZE]   += grid[i%SIZE][i/SIZE];
		count[i/SIZE]      += grid[i/SIZE][i%SIZE] != 0;
		count[i/SIZE+SIZE] += grid[i%SIZE][i/SIZE] != 0;
	}

	for(unsigned int i=0; i<SIZE; i++)
	{
		sum[2*SIZE]     += grid[i][i];
		count[2*SIZE]   += grid[i][i] != 0;
		sum[2*SIZE+1]   += grid[i][SIZE-1-i];
		count[2*SIZE+1] += grid[i][SIZE-1-i] != 0;
	}

	for(unsigned int i=0; i<2*SIZE+2; i++)
	{
		if(abs(sum[i]) == SIZE)
		{
			if(only_win)
				return 1000 * sum[i] / SIZE;
			eval += 1000 * sum[i] / SIZE;
		}
		else if(only_win)
			continue;
		else if(sum[i] != 0 && abs(sum[i]) == count[i])
			eval += sum[i];
	}

/*	printf("sum/count");
	for(unsigned int i=0; i<SIZE; i++)
	{
		printf("\n");
		printf("[%i] row s=%2i c=%2i\tcol s=%2i c=%2i",
				i, sum[i], count[i], sum[i+SIZE],
				count[i+SIZE]);
	}
	printf("\ndiag: s=%2i c=%2i\ts=%2i c=%2i",
			sum[2*SIZE], count[2*SIZE], sum[2*SIZE+1],
			count[2*SIZE+1]);
	printf("\n\n");*/

	return eval;
}

static bool is_game_over(void)
{
	return eval_grid(true) || !empty_cells;
}

static int negamax(int depth, int sign)
{
	int max = -INT_MAX;

	if(depth == MAX_DEPTH || is_game_over())
		return sign * abs_sub(eval_grid(false), depth);

	for(unsigned int i=0; i<SIZE*SIZE; i++)
	{
		int val;

		if(grid[ROW(i)][COL(i)] != BLANK)
			continue;

		grid[ROW(i)][COL(i)] = sign;
		empty_cells--;
		val = -negamax(depth+1, -sign);
		grid[ROW(i)][COL(i)] =  BLANK;
		empty_cells++;

		if(val >= max)
		{
			if(val == max && rand()%2)
				continue;

			max = val;
			if(depth == 0)
			{
				best_move[0] = ROW(i);
				best_move[1] = COL(i);
				printf("best move is: %u:%u\n", best_move[0],
						best_move[1]);
			}
		}
	}

	return max;
}

static void play_round(int sign)
{
	unsigned int row, col;

	print_grid();
	printf("%c's round\n", sym[1-sign]);

	if(is_game_over())
		return;

	if(sign < 0)
	{
get_input:
		printf("Enter the row:col to play (eg. 0:2): ");
		while(scanf("%u:%u", &row, &col) != 2)
			printf("Unvalid input\n");
		if(grid[row][col] != BLANK)
		{
			printf("Cell not free, give another try...\n");
			goto get_input;
		}
	}
	else
	{
		negamax(0, sign);
		row = best_move[0];
		col = best_move[1];
	}
	printf("%u:%u := %d\n", row, col, sign);
	grid[row][col] = sign;
	empty_cells--;

	play_round(-sign);
}

static void print_grid(void)
{
	for(unsigned int i=0; i<SIZE*SIZE; i++)
	{
		if(i%SIZE == 0 && i>0)
			printf("\n");
		printf("[%c]", sym[1+grid[ROW(i)][COL(i)]]);
	}
	printf("\n\n");
}

void start_game(void)
{
	srand(time(NULL));
	empty_cells = SIZE*SIZE;
	for(unsigned int i=0; i<SIZE*SIZE; i++)
		grid[ROW(i)][COL(i)] = 0;

	play_round(1);
}

void test_eval(void)
{
	srand(time(NULL));

	for(unsigned int i=0; i<5; i++)
	{
		for(unsigned int j=0; j<SIZE*SIZE; j++)
		{
			int rnd = rand()%3;

			grid[ROW(j)][COL(j)] = rnd == 0? CIRCLE : rnd == 1?
				BLANK : CROSS;
		}

		print_grid();
		printf("Evaluation: %i\n", eval_grid(false));
	}
}

