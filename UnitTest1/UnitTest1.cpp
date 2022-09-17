#include "pch.h"
#include "CppUnitTest.h"
#include "../ParallelLab1/Source.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:

		void WriteMatrixToFile(const vector<vector<int>>& m1, const vector<vector<int>>& m2) {
			Matrix().WriteMatrixToFile(m1, "m1.txt");
			Matrix().WriteMatrixToFile(m2, "m2.txt");
		}
		
		void ProcessSequentialMultiplicating(const vector<vector<int>>& m1, const vector<vector<int>>& m2) {
			WriteMatrixToFile(m1, m2);
			Matrix().SequentialMultiplicateTwoMatrix("m1.txt", "m2.txt", "m3.txt");
		}

		void ProcessParallelMultiplicating(const vector<vector<int>>& m1, const vector<vector<int>>& m2, const int option) {
			WriteMatrixToFile(m1, m2);
			Matrix().ParallelMultiplicateTwoMatrixByOpenMp("m1.txt", "m2.txt", "m3.txt", option);
		}

		void Multiplicate(const vector<vector<int>>& m1, const vector<vector<int>>& m2, const vector<vector<int>>& result) {
			ProcessSequentialMultiplicating(m1, m2);
			Assert::IsTrue(Matrix().GetMatrixFromFile("m3.txt") == result);
			for (int i = 1; i < 6; i++) {
				ProcessParallelMultiplicating(m1, m2, i);
				Assert::IsTrue(Matrix().GetMatrixFromFile("m3.txt") == result);
			}
		}

		void ThrowException(const vector<vector<int>>& m1, const vector<vector<int>>& m2) {
			bool check = false;
			try {
				ProcessSequentialMultiplicating(m1, m2);
			}
			catch (const Exception& ex) {
				check = true;
			}
			Assert::IsTrue(check);
		}









		TEST_METHOD(CheckMultiplicate)
		{
			Multiplicate({ {3,3,3}, {3,3,3}, {3,3,3} }, { {9,9,9}, {9,9,9}, {9,9,9} }, { {81, 81, 81}, {81, 81, 81}, {81, 81, 81} });
			Multiplicate({ {2,2}, {2,2} }, { {3,3}, {3,3}, }, { {12, 12}, {12, 12} });
			Multiplicate({ {42,68,35,1,70}, {25,79,59,63,65}, {6,46,82,28,62} }, { {92, 96, 43}, {28, 37, 92}, {5, 3, 54}, {93, 83, 22}, {17, 19, 96} }, { {7226, 8066, 16694}, {11771, 11964, 19155}, {5908, 6026, 15486} });
			
		}

		TEST_METHOD(CheckThrowException) {
			ThrowException({ {1}, }, { {2,2}, {2,2} });
			ThrowException({ {1,1,1,1,1}, {1,1,1,1,1}, {1,1,1,1,1}, {1,1,1,1,1}, {1,1,1,1,1} }, { {1,1,1,1,1}, {1,1,1,1,1}, {1,1,1,1,1}, {1,1,1,1,1} });
		}
	};
}
