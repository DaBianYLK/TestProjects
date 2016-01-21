#include <iostream>
#include <map>

using namespace std;

template<typename T>
void Output(T& value)
{
	value.Error();
}

void Output(string& value)
{
	cout << value.c_str() << endl;
}

void Output(int& value)
{
	cout << value << endl;
}

template<typename PairElem1, typename PairElem2>
void OutputPair(pair<PairElem1, PairElem2>& value)
{
	typename remove_const<PairElem1>::type temp = value.first;
	Output(temp);
	Output(value.second);
}

int main(void)
{
	map<string, int> mapStrInt;
	mapStrInt.insert(make_pair("Test", 1));

	OutputPair(*mapStrInt.begin());

	system("pause");

	return 0;
}