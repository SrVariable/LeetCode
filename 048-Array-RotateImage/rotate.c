#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Matrix
{
	int **content;
	int width;
	int height;
} Matrix;

// From: https://github.com/SrVariable/AdventOfCode2024/blob/85867edb618cf433ac4a8c90c6b346159e9797e4/day6/main.c#L28C1-L62C2
void	free_double_pointer(void **ptr)
{
	for (int i = 0; ptr[i]; ++i)
	{
		free(ptr[i]);
	}
	free(ptr);
}

int	count_lines(const char *line)
{
	int count = 0;
	for (int i = 0; line[i]; ++i)
	{
		if (line[i] == '\n')
		{
			++count;
		}
	}
	return count;
}

char **split(const char *line, char delim)
{
	int n_lines = count_lines(line);
	char **lines = malloc((n_lines + 1) * sizeof(*lines));
	if (!lines)
	{
		return NULL;
	}
	size_t offset = 0;
	for (int i = 0; i < n_lines; ++i)
	{
		size_t line_len = strchr(line + offset, delim) - (line + offset);
		lines[i] = strndup(line + offset, line_len);
		if (!lines[i])
		{
			free_double_pointer((void **)lines);
			return NULL;
		}
		offset += line_len + 1;
	}
	lines[n_lines] = NULL;
	return lines;
}

int split_len(char **s)
{
	int i = 0;

	while (s[i])
	{
		++i;
	}
	return i;
}

// From: https://github.com/SrVariable/funca/blob/617e8fb2f1794f43f1fdc1eab7d750500cb2a73c/funca.c#L152
char	*read_entire_file(char *filename)
{
	FILE *f = fopen(filename, "r");
	if (!f)
	{
		return NULL;
	}
	if (fseek(f, 0, SEEK_END) < 0)
	{
		fclose(f);
		return NULL;
	}
	long file_size = ftell(f);
	rewind(f);

	char *content = calloc(file_size + 1, sizeof(*content));
	if (!content)
	{
		fclose(f);
		return NULL;
	}
	if (fread(content, file_size, sizeof(*content), f) != 1)
	{
		free(content);
		fclose(f);
		return NULL;
	}
	fclose(f);

	return content;
}

// Format: https://en.wikipedia.org/wiki/X_PixMap
void	generate_xpm(const char *file, int **matrix, int width, int height)
{
	FILE * out;
	out = fopen(file, "wb");
	fprintf(out, "! XPM2\n");
	fprintf(out, "%d %d 4 1\n", width, height);
	fprintf(out, "0 c #FFFFFF\n");
	fprintf(out, "1 c #FF0000\n");
	fprintf(out, "2 c #00FF00\n");
	fprintf(out, "3 c #0000FF\n");
	fprintf(out, "\n");
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			fprintf(out, "%d", matrix[i][j]);
		}
		fprintf(out, "\n");
	}
	fclose(out);
}

void	free_matrix(int **m, int size)
{
	for (int i = 0; i < size; ++i)
	{
		free(m[i]);
	}
	free(m);
}

void	get_matrix_size(char **s, int *width, int *height)
{
	*width = strlen(s[0]);
	for (int i = 0; s[i]; ++i)
	{
		(*height)++;
	}
}

int	**extract_matrix(char **s, int width, int height)
{
	int **matrix = malloc(sizeof(*matrix) * height);
	if (!matrix)
	{
		return NULL;
	}
	for (int i = 0; i < height; ++i)
	{
		matrix[i] = malloc(sizeof(**matrix) * width);
		if (!matrix[i])
		{
			free_matrix(matrix, i);
			return NULL;
		}
		for (int j = 0; s[i][j]; ++j)
		{
			matrix[i][j] = s[i][j] - '0';
		}
	}
	return matrix;
}

// NOTE: Assuming there's an empty line before the image
int	get_matrix_index(char **s)
{
	int i = 0;
	while (s[i] && s[i][0])
	{
		++i;
	}
	++i;
	return i;
}

Matrix get_matrix_from_xpm(char *infile)
{
	Matrix matrix = {0};
	char *content = read_entire_file(infile);
	if (!content)
	{
		return matrix;
	}

	char **s = split(content, '\n');
	free(content);
	if (!s)
	{
		return matrix;
	}

	int startIndex = get_matrix_index(s);
	get_matrix_size(&s[startIndex], &matrix.width, &matrix.height);
	matrix.content = extract_matrix(&s[startIndex], matrix.width, matrix.height);
	free_double_pointer((void **)s);

	return matrix;
}

void	print_matrix(Matrix *matrix)
{
	for (int i = 0; i < matrix->height; ++i)
	{
		for (int j = 0; j < matrix->width; ++j)
		{
			printf("%d ", matrix->content[i][j]);
		}
		printf("\n");
	}
}

Matrix	*reverse_matrix(Matrix *matrix)
{
	for (int i = 0; i < matrix->height; ++i)
	{
		for (int j = 0; j < matrix->width / 2; ++j)
		{
			int temp = matrix->content[i][j];
			matrix->content[i][j] = matrix->content[i][matrix->width - 1 - j];
			matrix->content[i][matrix->width - 1 - j] = temp;
		}
	}
	return matrix;
}

void	rotate_different_size(Matrix *matrix)
{
	Matrix transpose = {0};

	transpose.width = matrix->height;
	transpose.height = matrix->width;
	transpose.content = malloc(sizeof(*transpose.content) * transpose.height);
	if (!transpose.content)
	{
		return;
	}

	for (int i = 0; i < transpose.height; ++i)
	{
		transpose.content[i] = malloc(sizeof(**transpose.content) * transpose.width);
		if (!transpose.content)
		{
			free_matrix(transpose.content, transpose.height);
			transpose.content = NULL;
			return;
		}
		for (int j = 0; j < transpose.width; ++j)
		{
			transpose.content[i][j] = matrix->content[j][i];
		}
	}

	transpose = *reverse_matrix(&transpose);
	free_matrix(matrix->content, matrix->height);
	matrix->content = transpose.content;
	matrix->width = transpose.width;
	matrix->height = transpose.height;
}

void	rotate_same_size(Matrix *matrix)
{
	for (int i = 0; i < matrix->height; ++i)
	{
		for (int j = 0; j < i; ++j)
		{
			int temp = matrix->content[i][j];
			matrix->content[i][j] = matrix->content[j][i];
			matrix->content[j][i] = temp;
		}
	}

	matrix = reverse_matrix(matrix);
}

// My solution for: https://leetcode.com/problems/rotate-image/description/?envType=problem-list-v2&envId=array
// Slightly modified
void rotate(Matrix *matrix) {
	if (matrix->width != matrix->height)
	{
		return rotate_different_size(matrix);
	}

	return rotate_same_size(matrix);
}

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: %s <infile> <outfile>\n", argv[0]);
		return 1;
	}
	char *infile = argv[1];
	char *outfile = argv[2];

	Matrix matrix = get_matrix_from_xpm(infile);
	if (!matrix.content)
	{
		return 1;
	}

	rotate(&matrix);
	generate_xpm(outfile, matrix.content, matrix.width, matrix.height);
	free_matrix(matrix.content, matrix.height);

	return 0;
}
