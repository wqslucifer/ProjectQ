#include "coreFrame.h"

void IceFrame::loadCSV(string filename)
{
	fstream file;
	try
	{
		file.open(filename.c_str(), std::ios::in);
		if (!file.is_open()) {
			cout << "file not open" << endl;
			return;
		}
		rows = 0;
		cols = 0;
		// get csv file column names
		string tempLine;
		if (getline(file, tempLine)) {
			istringstream sin(tempLine);
			string colName;
			int colIndex = 0;
			while (getline(sin, colName, ','))
			{
				columns.push_back(colName);
				colName_Str_Int.insert(pair<string, int>(colName, colIndex));
				++cols;
			}
		}
		else {
			throw "CSV_READ_ERROR";
		}
		this->colTypes.assign(columns.size(), dtype_empty);

		// load data
		string cellBuff;
		string tempLine;
		while (getline(file, tempLine)) {
			istringstream sin(tempLine);
			vector<VAR> oneRow;
			int col = 0;
			while (getline(sin, cellBuff, delim))
			{
				VAR cell;
				if (regex_match(cellBuff, sm, numeric_int) && dtype_int >= colTypes[col]) {
					colTypes[col] = dtype_int;
					cell = stoi(cellBuff);
				}
				else if (regex_match(cellBuff, sm, numeric) && dtype_double >= colTypes[col]) {
					colTypes[col] = dtype_double;
					cell = stod(cellBuff);
				}
				else if (dtype_string >= colTypes[col]) {
					colTypes[col] = dtype_string;
					cell = cellBuff;
				}
				else {
					cell = string("NAN");
				}
				oneRow.push_back(cell);
				++col;
			}
			for (auto c = col; c < columns.size(); c++)
				oneRow.push_back(string(""));
			iceData.push_back(oneRow);
			++rows;
		}
		isEmpty = cols + rows > 0 ? false : true;
	}
	catch (const std::exception& e)
	{
		cout << "unexpected error: " << e.what() << endl;
	}
}

void IceFrame::getSize()
{
	this->size = new int[2];
	this->size[0] = this->rows;
	this->size[1] = this->cols;
}

template<typename... Args>
bool all(Args... args) { return (... && args); }

template<typename... Args>
bool any(Args... args) { return (... || args); }

template<typename... Ts>
void printAll(Ts&&... mXs)
{
	(cout << ... << mXs) << endl;
}

template<typename TF, typename... Ts>
void forArgs(TF&& mFn, Ts&&... mXs)
{
	(mFn(mXs), ...);
}


struct TypeCheckVisitor {
	int operator()(const int n) const {
		return dtype_int;
	}
	int operator()(const double b) const {
		return dtype_double;
	}
	int operator()(const string& s) const {
		return dtype_string;
	}
};


int main() {
	variant<int, double, string> var;
	string temp = "1234563546351";
	cout << std::stoi(temp) << endl;
	system("pause");
}