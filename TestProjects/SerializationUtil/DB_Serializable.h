#pragma once

#include "DB_SerializationArchive.h"

namespace DaBianYLK {
	// ���л��ӿ�
	class Serializable {
	public:
		// �������еĿ����л������ԣ���Ա���������Լ���Щ�������л�ʱ��˳��
		virtual void Serialize(SerializationArchive& archive) = 0;		

	private:
		static int m_uVersion;

	
	private:
		
	};






























	// �Զ�����ע�ᣬ��ʹ��������ע�����
	template<typename Type>
	class AutoRegister {
		AutoRegister() {
			m_pTypeObject = new Type();

			// ע��Type����

			delete m_pTypeObject;
			m_pTypeObject = NULL;
		}

		static AutoRegister m_pRegister;
	};
}