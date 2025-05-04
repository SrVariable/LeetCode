// Original
// int numEquivDominoPairs(int** dominoes, int dominoesSize, int* dominoesColSize) {
//     int kv[100] = {0};
//     int count = 0;
// 
//     for (int i = 0; i < dominoesSize; ++i)
//     {
//         if (dominoes[i][0] > dominoes[i][1])
//         {
//             int temp = dominoes[i][0];
//             dominoes[i][0] = dominoes[i][1];
//             dominoes[i][1] = temp;
//         }
//         int key = dominoes[i][0] * 10 + dominoes[i][1];
//         kv[key]++;
//         count += kv[key] - 1;
//     }
//     return count;
// }

// Memory optimization
int numEquivDominoPairs(int** dominoes, int dominoesSize, int* dominoesColSize) {
    int kv[81] = {0};
    int count = 0;

    for (int i = 0; i < dominoesSize; ++i)
    {
        if (dominoes[i][0] > dominoes[i][1])
        {
            int temp = dominoes[i][0];
            dominoes[i][0] = dominoes[i][1];
            dominoes[i][1] = temp;
        }
        int key = (dominoes[i][0] - 1) * 9 + (dominoes[i][1] - 1);
        kv[key]++;
        count += kv[key] - 1;
    }
    return count;
}
