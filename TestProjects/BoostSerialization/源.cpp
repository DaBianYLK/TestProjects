// Author: ���һ���

#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <string>
#include <fstream>
#include <d3dx9.h>

using namespace std;

// ����ʽʵ��
class SerializableObject {
private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_NVP(m_nID);
		ar & BOOST_SERIALIZATION_NVP(m_strName);
	}

public:
	SerializableObject() :m_nID(0), m_strName("") {}
	SerializableObject(int nID, string strName) :m_nID(nID), m_strName(strName) {}
	~SerializableObject() {}

	inline int GetID() { return m_nID; }
	inline string GetName() { return m_strName; }

private:
	int m_nID;
	string m_strName;
};

// ����ʽʵ��
namespace boost {
	namespace serialization {
		template<class Archive>
		void serialize(Archive& ar, D3DXVECTOR2& vector, const unsigned int version) {
			ar & BOOST_SERIALIZATION_NVP(vector.x);
			ar & BOOST_SERIALIZATION_NVP(vector.y);
		}
	}
}

void main() {
	// ����һ���ļ������
	ofstream outFileStream("Test.txt");
	// ����һ������ĵ����ı����ͣ�
	boost::archive::text_oarchive outArchiveText(outFileStream);

	SerializableObject outObj(10, "Ten");
	D3DXVECTOR2 outVector(3, 3);
	outArchiveText << outObj << outVector;
	outFileStream.close();

	// ����һ���ļ�������
	ifstream inFileStream("Test.txt");
	// ����һ�������ĵ����ı����ͣ�
	boost::archive::text_iarchive inArchiveText(inFileStream);

	SerializableObject inObj;
	D3DXVECTOR2 inVector;
	inArchiveText >> inObj >> inVector;
	inFileStream.close();

	cout << "ID   = " << inObj.GetID()		<< endl;
	cout << "Name = " << inObj.GetName()	<< endl;
	cout << "x    = " << inVector.x			<< endl;
	cout << "y    = " << inVector.y			<< endl;

	system("pause");

	return;
}