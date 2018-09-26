#pragma once


#include "gruel_construct.h"
#include "gruel_alloc.h"
#include "gruel_uninitialized.h"


// ���ڶ�������������ΪĬ��������
namespace gruel {
	using alloc = _default_alloc_template<0>;


	/* ��װ��һ���ӿڣ�ʹ�����STL���ʵ���Ͼ���ת���� */
	// ��bytes��ת��Ϊ��Ӧ����T�Ĵ�С�����ӷ���
	template <typename T, typename Alloc>
	struct simple_alloc {
		static T *allocate(std::size_t n) {
			return static_cast<T *>(Alloc::allocate(n * sizeof(T)));
		}

		static T *allocate() {
			return static_cast<T *>(Alloc::allocate(sizeof(T)));
		}

		static void deallocate(T *p, std::size_t n) {
			if (n != 0)
				Alloc::deallocate(p, n * sizeof(T));
		}

		static void deallocate(T *p) {
			Alloc::deallocate(p, sizeof(T));
		}
	};
}
