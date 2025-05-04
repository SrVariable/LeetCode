#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>

typedef struct Matrix
{
	int **content;
	int width;
	int height;
} Matrix;

// TODO: Create another struct XPM_Header, to facilitate generating new XPM files
typedef struct XPM
{
	char **header;
	char **matrix;
} XPM;

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

// From: https://github.com/SrVariable/funca/blob/617e8fb2f1794f43f1fdc1eab7d750500cb2a73c/funca.c#L152
char	*read_entire_file(const char *filename)
{
	FILE *f = fopen(filename, "rb");
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
void	generate_xpm(const char *filename, char **xpm_header, Matrix *matrix)
{
	FILE * out;

	out = fopen(filename, "wb");
	for (int i = 0; xpm_header[i]; ++i)
	{
		fprintf(out, "%s\n", xpm_header[i]);
	}
	fprintf(out, "\n");
	for (int i = 0; i < matrix->height; ++i)
	{
		for (int j = 0; j < matrix->width; ++j)
		{
			fprintf(out, "%c", matrix->content[i][j]);
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
			matrix[i][j] = s[i][j];
		}
	}

	return matrix;
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
void rotate(Matrix *matrix)
{
	if (matrix->width != matrix->height)
	{
		return rotate_different_size(matrix);
	}

	return rotate_same_size(matrix);
}

char **splitndup(char **s, size_t n)
{
	char **new = malloc(sizeof(*new) * (n + 1));
	if (!new)
	{
		return NULL;
	}
	new[n] = NULL;

	for (size_t i = 0; i < n && s[i]; ++i)
	{
		new[i] = strdup(s[i]);
		if (!new[i])
		{
			free_double_pointer((void **)new);
			return NULL;
		}
	}

	return new;
}

int	get_xpm_header_end_index(char **s)
{
	regex_t reg;
	bool found = false;

	if (regcomp(&reg, ". c #[0-9|A-F|a-f]{6}$", REG_EXTENDED) != 0)
	{
		return -1;
	}
	int i = 0;
	for (; s[i]; ++i)
	{
		if (regexec(&reg, s[i], 0, NULL, 0) == 0)
		{
			found = true;
		}
		else
		{
			if (found)
			{
				break;
			}
		}
	}
	regfree(&reg);

	return i;
}

char **get_xpm_header(char **s)
{
	return splitndup(s, get_xpm_header_end_index(s));
}

void	update_xpm_header_size(char **header, Matrix *matrix)
{
	char buf[64] = {0};

	char **s = split(header[1], ' ');
	if (!s)
	{
		return;
	}
	snprintf(buf, sizeof(buf), "%d %d ", matrix->width, matrix->height);
	memcpy(header[1], buf, strlen(buf));
	free_double_pointer((void **)s);
}

char **get_xpm_matrix(char **s)
{
	int start = get_xpm_header_end_index(s);

	while (s && s[start] && !s[start][0])
	{
		++start;
	}
	int end = 0;
	while (s && s[start + end])
	{
		++end;
	}

	return splitndup(&s[start], end);
}

XPM get_xpm(const char *filename)
{
	XPM xpm = {0};

	char *content = read_entire_file(filename);
	if (!content)
	{
		return xpm;
	}
	char **s = split(content, '\n');
	free(content);
	if (!s)
	{
		return xpm;
	}
	xpm.header = get_xpm_header(s);
	xpm.matrix = get_xpm_matrix(s);
	free_double_pointer((void **)s);
	if (!xpm.matrix)
	{
		free_double_pointer((void **)xpm.header);
	}

	return xpm;
}

void	free_xpm(XPM *xpm)
{
	free_double_pointer((void **)xpm->header);
	free_double_pointer((void **)xpm->matrix);
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
	XPM xpm = get_xpm(infile);
	if (!xpm.header || !xpm.matrix)
	{
		return 1;
	}
	Matrix matrix = {0};
	get_matrix_size(xpm.matrix, &matrix.width, &matrix.height);
	matrix.content = extract_matrix(xpm.matrix, matrix.width, matrix.height);
	if (!matrix.content)
	{
		free_xpm(&xpm);
		return (1);
	}
	rotate(&matrix);
	update_xpm_header_size(xpm.header, &matrix);
	generate_xpm(outfile, xpm.header, &matrix);
	free_matrix(matrix.content, matrix.height);
	free_xpm(&xpm);
	return 0;
}
