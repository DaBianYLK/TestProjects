#pragma once

#include "DB_Serializable.h"

namespace DaBianYLK {
	class SerializationArchive {
	public:
		SerializationArchive();
		virtual ~SerializationArchive();

		//virtual SerializationArchive& operator& (Serializable& serializableObject) = 0;

		// �Ի������ͽ�������
		// ����Ϊÿ��������������һ�ζ���ʹ��ģ�庯����ԭ����ģ�庯����������Ϊ�麯��
		// Ϊ�˼��ⲿ���õĽӿڣ����Բ��ö�̬�����صķ�ʽʵ

		virtual SerializationArchive& operator & (signed char& sChar) = 0;
		virtual SerializationArchive& operator & (unsigned char& uChar) = 0;
		virtual SerializationArchive& operator & (short& sInt16) = 0;
		virtual SerializationArchive& operator & (unsigned short& uInt16) = 0;
		virtual SerializationArchive& operator & (int& sInt32) = 0;
		virtual SerializationArchive& operator & (unsigned& uInt32) = 0;
		virtual SerializationArchive& operator & (long long& sInt64) = 0;
		virtual SerializationArchive& operator & (unsigned long long& sInt64) = 0;

	private:

	};
}