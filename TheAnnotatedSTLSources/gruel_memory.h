#pragma once


#include "gruel_construct.h"
#include "gruel_alloc.h"
#include "gruel_uninitialized.h"


// 将第二级配置器定义为默认配置器
namespace gruel {
	using alloc = _default_alloc_template<0>;


	/* 包装的一个接口，使其符合STL规格，实际上均是转调用 */
	// 将bytes数转换为相应类型T的大小，更加方便
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
