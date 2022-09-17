#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <omp.h>
using namespace std;

template<typename T>
void input_correctly_number(T& aa)
{
    while (!(cin >> aa) || cin.peek() != '\n')
    {
        cin.clear();
        cin.ignore(cin.rdbuf()->in_avail());
        cout << "������� �������� ��������. ��������� �������: ";
    }
}

void Limitations(int min, int max, int& value)
{
    while (value < min || value > max)
    {
        cout << "��������� ����� �� ������������� ����������. ��������� ����: ";
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

    template<typename T>
    void CorrectlyInput(istream& in, T& aa, const string& exeption_text)
    {
        if (!(in >> aa) || cin.peek() != '\n')
        {
            /*cin.clear();
            cin.ignore(cin.rdbuf()->in_avail());
            throw Exception(exeption_text);*/
        }
    }


    vector<vector<int>> GenerateMatrix(int row, int col, bool random_generate = false) {
        vector<vector<int>> v(row);
        for (int i = 0; i < row; i++) {
            v[i].resize(col);
            for (int j = 0; j < col; j++) {
                if (random_generate) v[i][j] = rand() % 100 + 1;
            }
        }
        return v;
    }



public:

    vector<vector<int>> GetMatrixFromFile(const string& path) {
        ifstream file;
        file.open(path, ios::in);
        if (!file.is_open()) throw Exception("������ ��� �������� ����� " + path);
        int row, col;
        string line;
        //CorrectlyInput(file, row, "� ����� " + path + " ������� ������������ ����� �����!\n");
        //CorrectlyInput(file, col, "� ����� " + path + " ������� ������������ ����� �����!\n");
        file >> row >> col;
        if (row <= 0) throw Exception("���������� ����� ������ ���� ������ 0!\n");
        if (col <= 0) throw Exception("���������� �������� ������ ���� ������ 0!\n");
        vector<vector<int>> result = GenerateMatrix(row, col);
        //getline(file, line, '\n');
        getline(file, line, '\n');
        for (int i = 0; i < result.size(); i++) {
            for (int j = 0; j < result[i].size(); j++) {
                if (j < result[i].size() - 1) getline(file, line, ' ');
                else getline(file, line, '\n');
                if (find_if_not(line.begin(), line.end(), ::isdigit) != line.end() || line.size() == 0)
                    throw Exception("������������ ������� � �������: " + line + "!\n");
                result[i][j] = stoi(line);
            }
        }
        return result;
    }

    void WriteMatrixToFile(const vector<vector<int>>& v, const string& path) {
        ofstream file;
        file.open(path, ios::out);
        if (!file.is_open()) throw Exception("������ ��� �������� �����!\n");
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

    void CreateMatrix(int row, int col, const string& path) {
        if (row <= 0) throw Exception("���������� ����� ������ ���� ������ 0!\n");
        if (col <= 0) throw Exception("���������� �������� ������ ���� ������ 0!\n");
        WriteMatrixToFile(GenerateMatrix(row, col, true), path);
    }


    pair<float, int> SequentialMultiplicateTwoMatrix(const string& path1, const string& path2, const string& result_path) {
        auto m1 = GetMatrixFromFile(path1);
        auto m2 = GetMatrixFromFile(path2);



        if (m1[0].size() != m2.size()) throw Exception("��� ������������ ���� ������ ���������� ������� ������ ������� ������ ���� ����� ���������� ����� ������ �������!");

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

        //cout << "����� ���������� ��������� ������: " << (double)time / CLOCKS_PER_SEC << " ������\n" << "���������� ����������� ��������: " << actions_count << "!\n";

        WriteMatrixToFile(result, result_path);

        return pair<float, int>((double)time / CLOCKS_PER_SEC, actions_count);
    }

    pair<float, int> ParallelMultiplicateTwoMatrixByOpenMp(const string& path1, const string& path2, const string& result_path, const int option = 1) {
        auto m1 = GetMatrixFromFile(path1);
        auto m2 = GetMatrixFromFile(path2);



        if (m1[0].size() != m2.size()) throw Exception("��� ������������ ���� ������ ���������� ������� ������ ������� ������ ���� ����� ���������� ����� ������ �������!");

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

        else throw Exception("��������� ����� ����������!");
        

        time = clock() - time;

        //WriteMatrixToFile(result, result_path);

        return pair<float, int>((double)time / CLOCKS_PER_SEC, actions_count);
    }
};



void CreateMatrix() {
    int row, col;
    string path;
    cout << "������� ���������� �����: ";
    input_correctly_number(row);
    cout << "������� ���������� �������: ";
    input_correctly_number(col);
    cout << "������� ���� ��� ������ ��������������� ������� � ����: ";
    cin >> path;
    Matrix().CreateMatrix(row, col, path);
}



//int main() {
//    int menu = 1;
//    setlocale(LC_ALL, "RUS");
//
//    do {
//        cout << "������������ ������ �1\n\n1 - ��������� ������ � ���������\n2 - ��������� ������ ���������������\n3 - ��������� ������ ����������� � ������� OpenMp\n0 - �����\n������� ��� �����: ";
//        input_correctly_number(menu);
//        Limitations(0, 3, menu);
//
//        try {
//
//            if (menu == 1) {
//                cout << "�������� ������ �������: \n\n";
//                CreateMatrix();
//                cout << "�������� ������ �������: \n\n";
//                CreateMatrix();
//            }
//
//            else if (menu != 0) {
//                string path1, path2, result_path;
//                cout << "������� ���� � ����� � ������ ��������: ";
//                cin >> path1;
//                cout << "������� ���� � ����� �� ������ ��������: ";
//                cin >> path2;
//                cout << "������� ���� � �����, ��� ����� ������� ��������� ��������: ";
//                cin >> result_path;
//                if (menu == 2) Matrix().SequentialMultiplicateTwoMatrix(path1, path2, result_path);
//            }
//
//        }
//        catch (const Exception& ex) {
//            cout << ex.What() << "\n";
//        }
//
//
//
//    } while (menu != 0);
//}



int main() {
    setlocale(LC_ALL, "RUS");
    cout << "���������������� ���������: \n\n\n";
    double sum = 0; 
    int actions_count = 0, start = 500, finish = 1000;

    for (int i = start; i < finish; i+=100) {

        for (int j = 0; j < 5; j++) {
            Matrix().CreateMatrix(i, i, "m1.txt");
            //auto result = Matrix().SequentialMultiplicateTwoMatrix("m1.txt", "m1.txt", "m1.txt");
            auto result = Matrix().ParallelMultiplicateTwoMatrixByOpenMp("m1.txt", "m1.txt", "m1.txt");
            sum += result.first;
            actions_count = result.second;
        }
        cout << i << " * " << i << ": " << double(sum / 5) << " seconds, " << actions_count << " actions\n";
        sum = 0;
        actions_count = 0;
    }

    cout << "\n\n\n";
}


