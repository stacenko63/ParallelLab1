#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <omp.h>
#include "mpi.h"
#include <windows.h>
using namespace std;

/// <summary>
/// ddd
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="aa"></param>

template<typename T>
void input_correctly_number(T& aa)
{
    while (!(cin >> aa) || cin.peek() != '\n')
    {
        cin.clear();
        cin.ignore(cin.rdbuf()->in_avail());
        cout << "Введено неверное значение. Повторите попытку: ";
    }
}

void Limitations(int min, int max, int& value)
{
    while (value < min || value > max)
    {
        cout << "Введенное число не соответствует промежутку. Повторите ввод: ";
        input_correctly_number(value);
    }
}

class Exception {
private:
    string str;

public:
    Exception(const string& Str) : str(Str) {}

    string What() const {
        return str;
    }
};


class Matrix {
private:



public:

    vector<vector<int>> GenerateMatrix(int row, int col, bool is_random_generate = false) {
        vector<vector<int>> v(row);
        for (int i = 0; i < row; i++) {
            v[i].resize(col);
            for (int j = 0; j < col; j++) {
                if (is_random_generate) v[i][j] = rand() % 100 + 1;
                else v[i][j] = 0;
            }
        }
        return v;
    }

    vector<vector<double>> Generate(int row, int col, bool is_random_generate = false) {
        vector<vector<double>> v(row);
        for (int i = 0; i < row; i++) {
            v[i].resize(col);
            for (int j = 0; j < col; j++) {
                if (is_random_generate) v[i][j] = rand() % 100 + 1;
                else v[i][j] = 0;
            }
        }
        return v;
    }

    vector<vector<int>> GetMatrixFromFile(const string& path) {
        ifstream file;
        file.open(path, ios::in);
        if (!file.is_open()) throw Exception("Ошибка при открытии файла " + path);
        int row, col;
        string line;
        file >> row >> col;
        if (row <= 0) throw Exception("Количество строк должно быть больше 0!\n");
        if (col <= 0) throw Exception("Количество столбцов должно быть больше 0!\n");
        vector<vector<int>> result = GenerateMatrix(row, col);
        getline(file, line, '\n');
        //getline(file, line, '\n');
        for (int i = 0; i < result.size(); i++) {
            for (int j = 0; j < result[i].size(); j++) {
                if (j < result[i].size() - 1) getline(file, line, ' ');
                else getline(file, line, '\n');
                result[i][j] = stoi(line);
            }
        }
        return result;
    }

    void WriteMatrixToFile(const vector<vector<int>>& v, const string& path) {
        ofstream file;
       file.open(path, ios::out);
        if (!file.is_open()) throw Exception("Ошибка при открытии файла!\n");
        file << v.size() << " " << v[0].size() << "\n";
        for (int i = 0; i < v.size(); i++) {
            for (int j = 0; j < v[i].size(); j++) {
                file << v[i][j];
                if (j < v[i].size() - 1) file << " ";
            }
            if (i < v.size() - 1) file << "\n";
        }
        file.close();
    }

    void WriteMatrixFile(const vector<vector<double>>& v, const string& path) {
        ofstream file;
        file.open(path, ios::out);
        if (!file.is_open()) throw Exception("Ошибка при открытии файла!\n");
        file << v.size() << " " << v[0].size() << "\n";
        for (int i = 0; i < v.size(); i++) {
            for (int j = 0; j < v[i].size(); j++) {
                file << v[i][j];
                if (j < v[i].size() - 1) file << " ";
            }
            if (i < v.size() - 1) file << "\n";
        }
        file.close();
    }

    void WriteMatrixToFileMpi(const vector<vector<int>>& v, const string& path, int world_rank, int world_sizes) {
        ofstream file;
        file.open(path, ios::out);
        if (!file.is_open()) throw Exception("Ошибка при открытии файла!\n");
        file << v.size() << " " << v[0].size() << "\n";
        for (int i = 0; i < v.size(); i++) {
            for (int j = 0; j < v[i].size(); j++) {
                file << v[i][j];
                if (j < v[i].size() - 1) file << " ";
            }
            if (i < v.size() - 1) file << "\n";
        }
        file.close();
    }

    void CreateMatrix(int row, int col, const string& path, int world_rank = 0, int world_size = 1) {
        if (row <= 0) throw Exception("Количество строк должно быть больше 0!\n");
        if (col <= 0) throw Exception("Количество столбцов должно быть больше 0!\n");
        WriteMatrixToFile(GenerateMatrix(row, col, true), path);
    }


    pair<float, int> SequentialMultiplicateTwoMatrix(const string& path1, const string& path2, const string& result_path) {
        auto m1 = GetMatrixFromFile(path1);
        auto m2 = GetMatrixFromFile(path2);



        if (m1[0].size() != m2.size()) throw Exception("Для перемножения двух матриц количество колонок первой матрицы должно быть равно количеству строк второй матрицы!");

        vector<vector<int>> result = GenerateMatrix(m1.size(), m2[0].size());

        clock_t time = clock();

        int actions_count = 0;


        for (int i = 0; i < m1.size(); i++)
        {
            for (int k = 0; k < m2[0].size(); k++)
            {
                for (int j = 0; j < m1[0].size(); j++)
                {
                    result[i][k] += m1[i][j] * m2[j][k];
                    actions_count++;
                }
            }
        }

        time = clock() - time;

        WriteMatrixToFile(result, result_path);

        return pair<float, int>((double)time / CLOCKS_PER_SEC, actions_count);
    }


    pair<float, int> ParallelMultiplicateTwoMatrix(const string& path1, const string& path2, const string& result_path, const int option = 1, int argc = 0, char* argv[] = NULL) {
        auto m1 = GetMatrixFromFile(path1);
        auto m2 = GetMatrixFromFile(path2);



        if (m1[0].size() != m2.size()) throw Exception("Для перемножения двух матриц количество колонок первой матрицы должно быть равно количеству строк второй матрицы!");

        vector<vector<int>> result = GenerateMatrix(m1.size(), m2[0].size());

        clock_t time = clock();

        int actions_count = 0;

        if (option == 1) {

            for (int i = 0; i < m1.size(); i++)
            {
                for (int k = 0; k < m2[0].size(); k++)
                {
                    #pragma omp for
                    for (int j = 0; j < m1[0].size(); j++)
                    {
                        result[i][k] += m1[i][j] * m2[j][k];
                        actions_count++;
                    }

                }
            }

        }
        //3.6318
        //6.3748
        //12.7894
        //18.7246
        //25.0028

        else if (option == 2) {
            for (int i = 0; i < m1.size(); i++)
            {
                #pragma omp for
                for (int k = 0; k < m2[0].size(); k++)
                {
                    for (int j = 0; j < m1[0].size(); j++)
                    {
                        result[i][k] += m1[i][j] * m2[j][k];
                        actions_count++;
                    }

                }
            }
        }

        else if (option == 3) {
            #pragma omp for
            for (int i = 0; i < m1.size(); i++)
            {
                for (int k = 0; k < m2[0].size(); k++)
                {
                    for (int j = 0; j < m1[0].size(); j++)
                    {
                        result[i][k] += m1[i][j] * m2[j][k];
                        actions_count++;
                    }

                }
            }
        }

        else if (option == 4) {

            #pragma omp parallel 
            {
                for (int i = 0; i < m1.size(); i++)
                {
                    for (int k = 0; k < m2[0].size(); k++)
                    {
                        for (int j = 0; j < m1[0].size(); j++)
                        {
                            result[i][k] += m1[i][j] * m2[j][k];
                            actions_count++;
                        }

                    }
                }
            }
        }

        else if (option == 5) {
            #pragma omp parallel for schedule (static)
            for (int i = 0; i < m1.size(); i++)
            {
                for (int k = 0; k < m2[0].size(); k++)
                {
                    for (int j = 0; j < m1[0].size(); j++)
                    {
                        result[i][k] += m1[i][j] * m2[j][k];
                        actions_count++;
                    }

                }
            }
        }

        else throw Exception("Указанная опция недоступна!");
        

        time = clock() - time;

        WriteMatrixToFile(result, result_path);

        return pair<float, int>((double)time / CLOCKS_PER_SEC, actions_count);
    }
};



void CreateMatrix() {
    int row, col;
    string path;
    cout << "Введите количество строк: ";
    input_correctly_number(row);
    cout << "Введите количество колонок: ";
    input_correctly_number(col);
    cout << "Введите путь для записи сгенерированной матрицы в файл: ";
    cin >> path;
    Matrix().CreateMatrix(row, col, path);
}





void MultiplicateByMpi(int argc, char* argv[], int world_rank, int world_size, const string& first_path, const string& second_path, const string& result_path, bool isTestFlag) {
    vector<vector<int>> m1 = Matrix().GetMatrixFromFile(first_path);
    vector<vector<int>> m2 = Matrix().GetMatrixFromFile(second_path);
    if (m1[0].size() != m2.size()) 
        throw Exception("Для перемножения двух матриц количество колонок первой матрицы должно быть равно количеству строк второй матрицы!");
    double sum = 0;
    double actions_count = 0, tmp_actions_count = 0, count = 0;
    double start_time = 0, time = 0;
    int tmp = 0;
    vector<vector<int>> result, tmp_result;
    result = Matrix().GenerateMatrix(m1.size(), m2[0].size());
    tmp_result = Matrix().GenerateMatrix(m1.size(), m2[0].size());
    for (int j = 0; j < 5; j++) {
        if (world_rank == 0) {
            start_time = MPI_Wtime();
            for (int a = 0; a < tmp_result.size(); a++) {
                MPI_Bcast(tmp_result[a].data(), tmp_result[a].size(), MPI_INT, 0, MPI_COMM_WORLD);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);         
        for (int i = world_rank; i < m1.size(); i += world_size)
        {
            for (int k = 0; k < m2[0].size(); k++)
            {
                for (int j = 0; j < m1[0].size(); j++)
                {
                    tmp_result[i][k] += m1[i][j] * m2[j][k];
                    tmp_actions_count++;
                }
            }
        }
        for (int i = 0; i < result.size(); i++) {
            MPI_Reduce(tmp_result[i].data(), result[i].data(), tmp_result[i].size(), MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        }
        MPI_Reduce(&tmp_actions_count, &actions_count, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        tmp_actions_count = 0;
        if (world_rank == 0) {
            double tmp_time = MPI_Wtime() - start_time;
            time += tmp_time;
            count += actions_count;
            //if (!isTestFlag) cout << m1.size() << " * " << m2[0].size() << " Attempt " << j + 1 << ": " << tmp_time << " seconds, " << actions_count << " actions\n";
            actions_count = 0;
            Matrix().WriteMatrixToFile(result, result_path);
        }
        if (isTestFlag) break;
        MPI_Barrier(MPI_COMM_WORLD);
    }
    if (world_rank == 0) {
        if (!isTestFlag) cout << m1.size() << " * " << m2[0].size() << " Average values: " << time / 5 << " seconds, " << count / 5 << " actions\n";
    }
    
    

}

void Multiplicate(int argc, char* argv[], int world_rank, int world_size, const vector<vector<int>>& m1, const vector<vector<int>>& m2, const vector<vector<int>>& result) {
    Matrix().WriteMatrixToFile(m1, "m1.txt");
    Matrix().WriteMatrixToFile(m2, "m2.txt");
    MultiplicateByMpi(argc, argv, world_rank, world_size, "m1.txt", "m2.txt", "m3.txt", true);
    auto tmp = Matrix().GetMatrixFromFile("m3.txt");
    if (world_rank == 0) {
        if (tmp == result) cout << "OK\n";
        else cout << "FAIL\n";
    }
}

void Tests(int argc, char* argv[], int world_rank, int world_size) {
    if (world_rank == 0) cout << "TEST1 : \n";
    Multiplicate(argc, argv, world_rank, world_size, { {3,3,3}, {3,3,3}, {3,3,3} }, { {9,9,9}, {9,9,9}, {9,9,9} }, { {81, 81, 81}, {81, 81, 81}, {81, 81, 81} });
    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0) cout << "TEST2 : \n";
    Multiplicate(argc, argv, world_rank, world_size, { {2,2}, {2,2} }, { {3,3}, {3,3}, }, { {12, 12}, {12, 12} });
    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0) cout << "TEST3 : \n";
    Multiplicate(argc, argv, world_rank, world_size, { {42,68,35,1,70}, {25,79,59,63,65}, {6,46,82,28,62} }, { {92, 96, 43}, {28, 37, 92}, {5, 3, 54}, {93, 83, 22}, {17, 19, 96} }, { {7226, 8066, 16694}, {11771, 11964, 19155}, {5908, 6026, 15486} });
    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0) cout << "TEST4 : \n";
    Multiplicate(argc, argv, world_rank, world_size, { {42,68,35,1,70}, {25,79,59,63,65}, {6,46,82,28,62}, { 42,68,35,1,70 }, { 42,68,35,1,70 } }, { {42,68,35,1,70}, {25,79,59,63,65}, {6,46,82,28,62}, { 42,68,35,1,70 }, { 42,68,35,1,70 } },
        { {6656, 14666, 10837, 5377, 14500}, {8755, 19359, 14854, 6782, 19503}, {5674, 13934, 12798, 5290, 14794}, { 6656, 14666, 10837, 5377, 14500 }, { 6656, 14666, 10837, 5377, 14500 } });
    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0) cout << "TEST5 : \n";
    Multiplicate(argc, argv, world_rank, world_size, { {0,0,0,0,0,0}, {0,0,0,0,0,0}, {0,0,0,0,0,0}, { 0,0,0,0,0,0 }, { 0,0,0,0,0,0 }, {0,0,0,0,0,0} }, { {0,0,0,0,0,0}, {0,0,0,0,0,0}, {0,0,0,0,0,0}, { 0,0,0,0,0,0 }, { 0,0,0,0,0,0 }, {0,0,0,0,0,0} },
        { {0,0,0,0,0,0}, {0,0,0,0,0,0}, {0,0,0,0,0,0}, { 0,0,0,0,0,0 }, { 0,0,0,0,0,0 }, {0,0,0,0,0,0} });
}

int main(int argc, char* argv[]) {
    try {
        int start = 500, finish = 1000;
        MPI_Init(&argc, &argv);
        setlocale(LC_ALL, "RUS");
        srand(time(NULL));
        int world_size = 1, world_rank = 0;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size); //количество процессов
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); //номер процесса
        for (int i = start; i < finish; i += 100) {
            Matrix().CreateMatrix(i, i, "m1.txt");
            MultiplicateByMpi(argc, argv, world_rank, world_size, "m1.txt", "m1.txt", "m1.txt", false);
        }
        //Tests(argc, argv, world_rank, world_size);
        MPI_Finalize();
    }
    catch (const char* ex) {
        cout << ex << "\n";
    }

}

//int main(int argc, char* argv[]) {
//        MPI_Init(&argc, &argv);
//        setlocale(LC_ALL, "RUS");
//        srand(time(NULL));
//        int world_size = 1, world_rank = 0;
//        MPI_Comm_size(MPI_COMM_WORLD, &world_size); //количество процессов
//        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); //номер процесса
//        vector<double> a0(40), a1(40);
//        for (int i = 0; i < 40; i++) {
//            a0[i] = 0;
//            a1[i] = 0;
//        }
//        MPI_Bcast(a1.data(), 40, MPI_DOUBLE, 0, MPI_COMM_WORLD);
//        for (int i = world_rank; i < 40; i += world_size)
//        {
//            if (world_rank == 1) {
//                a1[i] = 1;
//            }
//            if (world_rank == 0) {
//                a1[i] = 7;
//            }
//            else if (world_rank == 2) {
//                a1[i] = 2;
//            }
//            else if (world_rank == 3) {
//                a1[i] = 3;
//            }
//        }
//        MPI_Reduce(a1.data(), a0.data(), 40, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
//        if (world_rank == 0) {
//            for (int i = 0; i < 40; i++) {
//                cout << a0[i] << " ";
//            }
//        }
//        MPI_Finalize();
//}


//int main(int argc, char* argv[]) {
//    MPI_Init(&argc, &argv);
//    setlocale(LC_ALL, "RUS");
//    srand(time(NULL));
//    int world_size = 1, world_rank = 0;
//    MPI_Comm_size(MPI_COMM_WORLD, &world_size); //количество процессов
//    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); //номер процесса
//    vector<vector<double>> a0, a1;
//    a0 = Matrix().Generate(40, 40, false);
//    a1 = Matrix().Generate(40, 40, false);
//    for (int a = 0; a < 40; a++) {
//        MPI_Bcast(a1[a].data(), 40, MPI_DOUBLE, 0, MPI_COMM_WORLD);
//    }
//    for (int i = world_rank; i < 40; i += world_size)
//    {
//        for (int j = 0; j < 40; j++) {
//            if (world_rank == 1) {
//                a1[i][j] = 1;
//            }
//            if (world_rank == 0) {
//                a1[i][j] = 7;
//            }
//            else if (world_rank == 2) {
//                a1[i][j] = 2;
//            }
//            else if (world_rank == 3) {
//                a1[i][j] = 3;
//            }
//        }
//    }
//    for (int i = 0; i < 40; i++) {
//        MPI_Reduce(a1[i].data(), a0[i].data(), 40, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
//    }
//    if (world_rank == 0) {
//        Matrix().WriteMatrixFile(a0, "m1.txt");
//    }
//    MPI_Finalize();
//}






//int main(int argc, char* argv[]) {
//    setlocale(LC_ALL, "RUS");
//    cout << "Параллельное умножение: \n\n\n";
//    double sum = 0;
//    int actions_count = 0, start = 500, finish = 1000;
//
//    for (int i = start; i < finish; i += 100) {
//
//        //for (int j = 0; j < 5; j++) {
//            Matrix().CreateMatrix(i, i, "m1.txt");
//            //auto result = Matrix().SequentialMultiplicateTwoMatrix("m1.txt", "m1.txt", "m1.txt");
//            auto result = Matrix().ParallelMultiplicateTwoMatrix("m1.txt", "m1.txt", "m1.txt", 6, argc, argv);
//            sum += result.first;
//            actions_count = result.second;
//        //}
//        cout << i << " * " << i << ": " << double(sum / 5) << " seconds, " << actions_count << " actions\n";
//        sum = 0;
//        actions_count = 0;
//    }
//
//    cout << "\n\n\n";
//}





//mpiexec -n 4 ParallelLab1.exe




