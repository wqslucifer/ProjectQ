#include "coreFrame.h"


// csv
void IceFrame::loadCSV(string filename, char delim = ',')
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
			while (getline(sin, colName, delim))
			{
				columns.push_back(colName);
				colName_Str_Int.insert(pair<string, int>(colName, colIndex++));
				++cols;
			}
		}
		else {
			throw "CSV_READ_ERROR";
		}
		this->colTypes.assign(columns.size(), dtype_empty);

		// load data
		while (getline(file, tempLine)) {
			vector<VAR> oneRow;
			int col = 0;
			string cellBuff;
			while (getOneCell(tempLine, cellBuff, delim))
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
				else if (cellBuff!="" && dtype_string >= colTypes[col]) {
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

bool IceFrame::getOneCell(string &oneLine, string &cell, char delim=',')
{
	/*
		read one cell from a line of csv
	*/
	int cellLen = 0;
	bool ifBlock = false;
	char cellEnd = delim;
	cell.clear();
	if (oneLine.size() == 0)
		return false;
	char c;
	while (cellLen < oneLine.size())
	{
		c = oneLine[cellLen];
		if (c == '\"') {
			if (!ifBlock)
				ifBlock = true;
			if (oneLine[cellLen + 1] == ',') {
				++cellLen;
				ifBlock = false;
				break;
			}
			else if (oneLine[cellLen + 1] == '\"') {
				cell += '"';
				cellLen += 2;
			}
			else {
				++cellLen;
			}
		}
		else if (c == ',' && !ifBlock) {
			break;
		}
		else{
			cell += c;
			++cellLen;
		}
	}
	if (cellLen < oneLine.size()) {
		oneLine = oneLine.substr(cellLen + 1);
	}
	else {
		oneLine = oneLine.substr(cellLen);
	}
	return true;

}

//utls
void IceFrame::getSize()
{
	this->size = new int[2];
	this->size[0] = this->rows;
	this->size[1] = this->cols;
}

void IceFrame::display()
{
	for (int row = 0; row < rows; row++)
	{
		for (int col = 0; col < cols; col++)
		{
			std::visit(coutVisitor(), this->iceData[row][col]);
			cout << "\t";
		}
		cout << endl;
	}
}

// iloc
VAR IceFrame::iloc(int row, int col)
{
	return iceData[row][col];
}

vector<vector<VAR>> IceFrame::iloc(vector<int> &rows, vector<int> &cols)
{
	vector<vector<VAR>> ret;
	for (auto row : rows) {
		vector<VAR> newRow;
		for (auto col : cols) {
			newRow.push_back(this->iceData[row][col]);
		}
		ret.push_back(newRow);
	}
	return ret;
}

vector<VAR> IceFrame::iloc(vector<int> &rows, int col)
{
	vector<VAR> ret;
	for (auto row : rows) {
		ret.push_back(this->iceData[row][col]);
	}
	return ret;
}

vector<VAR> IceFrame::iloc(int row, vector<int> &cols)
{
	vector<VAR> ret;
	for (auto col : cols) {
		ret.push_back(this->iceData[row][col]);
	}
	return ret;
}

// loc
VAR IceFrame::loc(int row, string col)
{
	int i_col = colName_Str_Int[col];
	return iceData[row][i_col];
}

vector<VAR> IceFrame::loc(int row, vector<string> cols)
{
	vector<VAR> ret;
	vector<int> i_cols;
	for_each(cols.begin(), cols.end(), [&](string c) {i_cols.push_back(this->colName_Str_Int[c]); });
	for (auto col : i_cols)
		ret.push_back(this->iceData[row][col]);
	return ret;
}

vector<VAR> IceFrame::loc(vector<int> rows, string col)
{
	vector<VAR> ret;
	int i_col = colName_Str_Int[col];
	for (auto row : rows)
		ret.push_back(this->iceData[row][i_col]);
	return ret;
}

vector<vector<VAR>> IceFrame::loc(vector<int> rows, vector<string> cols)
{
	vector<int> i_cols;
	for_each(cols.begin(), cols.end(), [&](string c) {i_cols.push_back(this->colName_Str_Int[c]); });
	vector<vector<VAR>> ret;
	for (auto row : rows) {
		vector<VAR> newRow;
		for (auto col : i_cols) {
			newRow.push_back(this->iceData[row][col]);
		}
		ret.push_back(newRow);
	}
	return ret;
}

// bool
bool IceFrame::all(int axis = 0) // axis: 0 column, 1 row
{
	try
	{
		if (axis == 0) // column
		{
			if (cols > 0) {
			}
			else {
			}
		}
		else if (axis > 0) // row
		{
		}
		else {
			throw("axis input error");
		}
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}

bool IceFrame::any(int axis=0) // axis: 0 column, 1 row
{
	try
	{
		if (axis == 0) // column
		{
		}
		else if (axis > 0) // row
		{
		}
		else {
			throw("axis input error");
		}
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}

template<typename... Args>
bool checkAll(Args... args) { return (... && args); }

template<typename... Args>
bool checkAny(Args... args) { return (... || args); }

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



#ifdef DEBUG
void IceFrame::testRegex(string num)
{
	cout << num << " is int:" << regex_match(num, sm, numeric_int) << endl;
	cout << num << " is number:" << regex_match(num, sm, numeric) << endl;
}
#endif // DEBUG


void testfunc(vector<int> input) {
	for (auto i : input)
		cout << i << endl;
}


int main() {
	string csvPath = "E:/project/ProjectQ/test/TitanicDataset/train.csv";
	IceFrame trainset;
#ifdef DEBUGREGEX
	trainset.testRegex("1000");
	trainset.testRegex("0065");
	trainset.testRegex("16354.5684");
	trainset.testRegex("05");
	trainset.testRegex("0");
	trainset.testRegex("0.000");
	trainset.testRegex("+.5615");
	trainset.testRegex("+0.5615");
	trainset.testRegex("-0.5615");
	// get one cell test
	string temp = "\"adf,\"\"alllll\"\"\",3214,,13";
	string cell;
	while (trainset.getOneCell(temp, cell, ',')) {
		cout << cell << endl;
	}

	// iceFrame iloc test
	vector<int> rows = { 1,3,5,7 };
	auto ret = trainset.iloc(rows, 0);
	for (auto i : ret) {
		std::visit(coutVisitor(), i);
	}

#endif // DEBUGREGEX
	trainset.loadCSV(csvPath);
	//trainset.display();

	vector<string> cols = {"PassengerId","Name"};
	auto ret = trainset.loc(0, cols);
	for (auto i : ret) {
		std::visit(coutVisitor(), i);
	}

	system("pause");
}