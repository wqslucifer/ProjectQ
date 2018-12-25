#include "coreFrame.h"


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
				colName_Str_Int.insert(pair<string, int>(colName, colIndex));
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

VAR IceFrame::iloc(int row, int col)
{
	return iceData[row][col];
}

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


void testfunc(variant<int, string> input) {
	std::visit(coutVisitor(), input);
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

	string temp = "\"adf,\"\"alllll\"\"\",3214,,13";
	string cell;
	while (trainset.getOneCell(temp, cell, ',')) {
		cout << cell << endl;
	}
#endif // DEBUGREGEX
	trainset.loadCSV(csvPath);
	trainset.display();
	system("pause");
}