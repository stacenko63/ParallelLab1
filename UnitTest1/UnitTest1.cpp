#include "pch.h"
#include "CppUnitTest.h"
#include "../ParallelLab1/Source.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:
		
		void ProcessMultiplicating(const vector<vector<int>>& m1, const vector<vector<int>>& m2) {
			Matrix().WriteMatrixToFile(m1, "m1.txt");
			Matrix().WriteMatrixToFile(m2, "m2.txt");
			Matrix().SequentialMultiplicateTwoMatrix("m1.txt", "m2.txt", "m3.txt");
		}

		void Multiplicate(const vector<vector<int>>& m1, const vector<vector<int>>& m2, const vector<vector<int>>& result) {
			ProcessMultiplicating(m1, m2);
			vector<vector<int>> m3 = Matrix().GetMatrixFromFile("m3.txt");
			Assert::IsTrue(m3 == result);
		}

		void ThrowException(const vector<vector<int>>& m1, const vector<vector<int>>& m2) {
			bool check = false;
			try {
				ProcessMultiplicating(m1, m2);
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
