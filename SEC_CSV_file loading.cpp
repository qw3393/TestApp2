// SEC_CSV_file loading.cpp - Read csv file and save into 2D array variable.


#include <chrono>		//time_point, steady_clock
#include <iostream>
#include <fstream>		//ifstream
#include <string>
#include <vector>		//vector
#include <sstream>      // std::stringstream, std::stringbuf

using namespace std;


int main() {
	string line, str;
	string word, temp;

	int Line_number;
	int Value_number;
	float temp_num;

	vector<string> row;				// 1D vector
	vector<vector<string>> vect;	// 2D vector


	chrono::time_point<chrono::steady_clock> start = chrono::steady_clock::now();

	ifstream myfile("filtered_Area1.csv");

	if (myfile.is_open())
	{
		Line_number = 0;
		while (getline(myfile, line))
		{
			++Line_number;
			str = line;

			stringstream s(str);

			Value_number = 0;
			row.clear();

			// read every column data of a row and
			// store it in a string variable, 'word'
			while (getline(s, word, ',')) {
				++Value_number;
				// add all the column data
				// of a row to a vector
				row.push_back(word);
			}
				// Pushing back above 1D vector (row)
				// to create the 2D vector (vect)
				vect.push_back(row);
		}

	myfile.close();

		std::chrono::time_point<std::chrono::steady_clock> stop = std::chrono::steady_clock::now();
		std::chrono::duration<double, std::milli> elapsed = stop - start;
		std::cout << "Elapsed: " << elapsed.count() << " ms\n" << std::endl;

		// CSV file loading and put into 2D array (vect) test
		for (int i = 0; i <= 5; ++i)
		{
			for (int j = 0; j <= 5; ++j)
			{
				temp_num = stof(vect[i][j]);
				cout << temp_num << '\n';
			}
		}

		cout << "Total number of line in the file = " << Line_number << '\n';;
		cout << "Total number of values per line = " << Value_number << '\n';;
	}

	else cout << "Unable to open file";
	return 0;
}


