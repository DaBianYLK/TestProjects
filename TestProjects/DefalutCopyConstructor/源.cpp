/*
	ȷ��Ĭ�ϸ��ƹ��캯���Ծ�̬�����STL����ִ�е���ǳ�����������.

	���Խ���������
*/


#include <iostream>
#include <list>

using namespace std;

struct Object {
	int iArray[20];

	list<int> listInt;

	Object() {
		for (int i = 0; i < 20; ++i) {
			iArray[i] = i;
			listInt.push_back(i);
		}
	}

	~Object() {
		listInt.clear();
	}
};

void main() {
	Object* pA = new Object();

	Object b(*pA);

	cout << pA->iArray << '\t' << b.iArray << endl;

	list<int>::iterator itA = pA->listInt.begin();
	list<int>::iterator itB = b.listInt.begin();
	for (int i = 0; i < 20; ++i) {
		cout << &(*itA) << '\t' << &(*itB) << endl;
		++itA;
		++itB;
	}

	delete pA;

	int x[100000];
	for (int i = 0; i < 100000; ++i) {
		x[i] = i + 9999999;
	}
	
	list<int>::iterator it = b.listInt.begin();
	for (int i = 0; i < 20; ++i) {
		cout << b.iArray[i] << '\t' << *it << endl;
		++it;
	}

	system("pause");
}