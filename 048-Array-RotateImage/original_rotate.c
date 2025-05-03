// My solution for: https://leetcode.com/problems/rotate-image/description/?envType=problem-list-v2&envId=array
void rotate(int** matrix, int matrixSize, int* matrixColSize) {
    for (int i = 0; i < matrixSize; ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            int temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
    }

    for (int i = 0; i < matrixSize; ++i)
    {
        for (int j = 0; j < matrixSize / 2; ++j)
        {
            int temp = matrix[i][j];
            matrix[i][j] = matrix[i][matrixSize - 1 - j];
            matrix[i][matrixSize - 1 - j] = temp;
        }
    }
    *matrixColSize = matrixSize;
}
