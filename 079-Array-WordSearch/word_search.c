#include <stdbool.h>
#include <string.h>

bool has_match_word(char **board, int boardSize, int *boardColSize, char *word, int i, int j, int count, int length)
{
	if (count == length)
	{
		return true;
	}
	if (i < 0 || i >= boardSize || j < 0 || j >= *boardColSize || board[i][j] != word[count])
	{
		return false;
	}

	char temp = board[i][j];
	board[i][j] = '.';
	bool match = has_match_word(board, boardSize, boardColSize, word, i, j + 1, count + 1, length)
				|| has_match_word(board, boardSize, boardColSize, word, i, j - 1, count + 1, length)
				|| has_match_word(board, boardSize, boardColSize, word, i + 1, j, count + 1, length)
				|| has_match_word(board, boardSize, boardColSize, word, i - 1, j, count + 1, length);
	board[i][j] = temp;

	return match;
}

bool exist(char** board, int boardSize, int* boardColSize, char* word) {
	int len = strlen(word);

	for (int i = 0; i < boardSize; ++i)
	{
		for (int j = 0; j < boardColSize[i]; ++j)
		{
			if (has_match_word(board, boardSize, boardColSize, word, i, j, 0, len))
			{
				return true;
			}
		}
	}

	return false;
}
