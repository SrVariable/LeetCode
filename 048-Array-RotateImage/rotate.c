#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void	rotate_2(int **matrix, int width, int height)
{
	printf("Not implemented yet\n");
}

// My solution for: https://leetcode.com/problems/rotate-image/description/?envType=problem-list-v2&envId=array
// Slightly modified
void rotate(int **matrix, int width, int height) {
	if (width != height)
	{
		return rotate_2(matrix, width, height);
	}
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (i < j) break;
			int temp = matrix[i][j];
			matrix[i][j] = matrix[j][i];
			matrix[j][i] = temp;
		}
	}

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (j >= width / 2) break;
			int temp = matrix[i][j];
			matrix[i][j] = matrix[i][width - 1 - j];
			matrix[i][width - 1 - j] = temp;
		}
	}
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
	int **m = malloc(sizeof(*m) * height);
	if (!m)
	{
		return NULL;
	}
	for (int i = 0; i < height; ++i)
	{
		m[i] = malloc(sizeof(**m) * width);
		if (!m[i])
		{
			free_matrix(m, i);
			return NULL;
		}
		for (int j = 0; s[i][j]; ++j)
		{
			m[i][j] = s[i][j] - '0';
		}
	}
	return m;
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

	char *content = read_entire_file(infile);
	if (!content)
	{
		return 1;
	}

	char **s = split(content, '\n');
	free(content);
	if (!s)
	{
		return 1;
	}

	int width = 0;
	int height = 0;
	int startIndex = get_matrix_index(s);
	get_matrix_size(&s[startIndex], &width, &height);
	int **matrix = extract_matrix(&s[startIndex], width, height);
	free_double_pointer((void **)s);
	if (!matrix)
	{
		return 1;
	}

	int matrixCol;
	rotate(matrix, width, height);
	generate_xpm(outfile, matrix, width, height);

	free_matrix(matrix, height);
	return 0;
}
