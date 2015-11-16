#pragma once

#include "DB_Serializable.h"

namespace DaBianYLK {
	class SerializationArchive {
	public:
		SerializationArchive();
		virtual ~SerializationArchive();

		//virtual SerializationArchive& operator& (Serializable& serializableObject) = 0;

		// 对基本类型进行重载
		// 这里为每个基本类型重载一次而不使用模板函数的原因是模板函数不能声明为虚函数
		// 为了简化外部调用的接口，所以采用多态和重载的方式实

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