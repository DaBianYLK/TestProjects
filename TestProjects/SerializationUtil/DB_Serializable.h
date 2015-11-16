#pragma once

#include "DB_SerializationArchive.h"

namespace DaBianYLK {
	// 序列化接口
	class Serializable {
	public:
		// 设置类中的可序列化的属性（成员变量），以及这些属性序列化时的顺序
		virtual void Serialize(SerializationArchive& archive) = 0;		

	private:
		static int m_uVersion;

	
	private:
		
	};






























	// 自动类型注册，对使用者隐藏注册过程
	template<typename Type>
	class AutoRegister {
		AutoRegister() {
			m_pTypeObject = new Type();

			// 注册Type类型

			delete m_pTypeObject;
			m_pTypeObject = NULL;
		}

		static AutoRegister m_pRegister;
	};
}