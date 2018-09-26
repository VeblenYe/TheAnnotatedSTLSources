#pragma once


#include <new>
#include <iostream>


namespace gruel {

	/* 第一级配置器 */
	/* 这个inst没有用 */
	template <int inst>
	class _malloc_alloc_template {
	public:
		// 至于为什么返回void *的指针，是因为该类型指针可以随意强转为其他类型的指针
		static void *allocate(std::size_t n) {
			void *result = std::malloc(n);
			if (result == nullptr)
				result = oom_malloc(n);
			return result;
		}

		static void deallocate(void *p, std::size_t) {
			std::free(p);
		}

		static void *reallocate(void *p, std::size_t /* old_sz */, std::size_t new_sz) {
			void *result = std::realloc(p, new_sz);
			if (result == nullptr)
				result = oom_realloc(p, new_sz);
			return result;
		}


		/* 用来模拟C++中的set_new_handler，因为我们不是用operator new实现的 */
		/* 这里用了尾置返回，比较容易阅读 */
		static auto set_malloc_handler(void (*f)()) -> void(*)() {
			auto old = _malloc_alloc_oom_handler;
			_malloc_alloc_oom_handler = f;
			return old;
		}
	private:
		/* "oom": out of memory */
		static void *oom_malloc(std::size_t);
		static void *oom_realloc(void *, std::size_t);
		// _malloc_alloc_oom_handler是一个函数指针，该函数返回void
		static void(*_malloc_alloc_oom_handler)();	
	};


	/* 初值为空，有待客端设定 */
	template <int inst>
	void(*_malloc_alloc_template<inst>::_malloc_alloc_oom_handler)() = nullptr;


	template <int inst>
	void *_malloc_alloc_template<inst>::oom_malloc(std::size_t n) {
		void(*my_malloc_handler)();
		void *result;

		while (1) {
			// 这样写为了多线程考虑？
			my_malloc_handler = _malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr)
				throw std::bad_alloc();
			// 调用处理例程
			(*my_malloc_handler)();
			result = std::malloc(n);
			if (result)
				return result;
		}
	}

	
	template <int inst>
	void *_malloc_alloc_template<inst>::oom_realloc(void *p, std::size_t n) {
		void(*my_malloc_handler)();
		void *result;

		while (1) {
			my_malloc_handler = _malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr)
				throw std::bad_alloc();
			(*my_malloc_handler)();
			result = std::realloc(p, n);
			if (result)
				return result;
		}
	}
	
	// 直接将inst设定为0
	using malloc_alloc = _malloc_alloc_template<0>;


/* ----------------------------------------------------------------------------- */


	// 64位环境下，8是自然对齐
	const std::size_t _ALIGN = 8;
	const std::size_t _MAX_BYTES = 128;
	const std::size_t _NFREELISTS = _MAX_BYTES / _ALIGN;


	/* 第二级配置器 */
	/* 此实现不考虑多线程，inst无用 */
	// 该实现过程中用到了很多类型强转，要注意
	template <int inst>
	class _default_alloc_template {
	private:

		// 将bytes上调至8的倍数
		/* 原理：(ALIGN - 1)二进制表示是0000 0111，则~(ALIGN - 1)二进制表示是1111 1000，
		当bytes是8的倍数时，不会改变，当bytes不是8的倍数时，设bytes = 8n + p, p < 8，
		加上0000 0111后，变为bytes = 8(n + 1) + p，p < 8，再和1111 1000与后消除最后3位，
		得到8的倍数 */
		static std::size_t ROUND_UP(std::size_t bytes) {
			return ((bytes + (_ALIGN - 1)) & ~(_ALIGN - 1));
		}

		/* free_list的节点结构 */
		// 这里的union大小为4字节，因为指针占用4字节，char数组占用1字节，取大值
		union obj {
			union obj *free_list_link;	// 从该字段观之，obj可视为一个指针指向另一个obj
			char client_data[1];		// 从该字段观之，obj可视为一个指针指向实际区块
		};

		// 16个free_list
		static obj * volatile free_list[_NFREELISTS];

		// 确定使用第n号的free_list，n从0开始
		static std::size_t FREELIST_INDEX(std::size_t bytes) {
			return (((bytes + _ALIGN - 1) / _ALIGN) - 1);
		}

		// 返回一个大小为n的对象，并可能加入大小为n的其它区块到free list
		static void *refill(std::size_t n);

		// 配置一大块空间，可容纳nobjs个大小为"size"的区块
		// 如果无法配置nobjs个空间，则nobjs可能会降低
		static char *chunk_alloc(std::size_t size, int &nobjs);

		static char *start_free;	// 内存池的起始位置，只在chunk_alloc中变化
		static char *end_free;		// 内存池的结束位置，只在chunk_alloc中变化	
		static std::size_t heap_size;

	public:
		static void *allocate(std::size_t n) {

			obj * volatile *my_free_list;	// 即obj ** my_free_list，volatile用于多线程
			/* volatile用于让编译器不对该变量的访问代码进行优化，就是说每次访问该变量都会
			从内存中再次读取一遍，多线程中编译器不会把该变量放入寄存器中，以免导致多个线程访问
			的变量不一致 */
			obj *result;

			// 大于128bytes就调用第一级配置器
			if (n > static_cast<std::size_t>(_MAX_BYTES))
				return malloc_alloc::allocate(n);

			// 寻找16个free list中适当的一个
			my_free_list = free_list + FREELIST_INDEX(n);
			result = *my_free_list;

			if (result == nullptr) {
				void *r = refill(ROUND_UP(n));
				return r;
			}

			// 调整free list，将result交出去
			*my_free_list = result->free_list_link;
			return result;
		}

		static void deallocate(void *p, std::size_t n) {
			obj *q = (obj *)(p);	// 强转
			obj * volatile *my_free_list;

			if (n > (std::size_t)(_MAX_BYTES)) {
				malloc_alloc::deallocate(p, n);
				return;
			}

			// 寻找对应的free list
			my_free_list = free_list + FREELIST_INDEX(n);
			q->free_list_link = *my_free_list;
			*my_free_list = q;
		}

		// 不用实现
		static void *reallocate(void *p, std::size_t old_sz, std::size_t new_sz);

	
	};

	/* static data member的定义和初值设定 */
	template <int inst>
	char *_default_alloc_template<inst>::start_free = nullptr;

	template <int inst>
	char *_default_alloc_template<inst>::end_free = nullptr;

	template <int inst>
	std::size_t _default_alloc_template<inst>::heap_size = 0;

	template <int inst>
	typename _default_alloc_template<inst>::obj * volatile
		_default_alloc_template<inst>::free_list[_NFREELISTS] =
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };


	template <int inst>
	void *_default_alloc_template<inst>::refill(std::size_t n) {
		int nobjs = 20;
		// 注意这里传入的nobjs是引用，实际nobjs可能会修改
		char *chunk = chunk_alloc(n, nobjs);
		obj * volatile *my_free_list;
		obj *result;

		// 如果只获得一个区块，则直接返回给调用者，free list无变化
		// 可以发现是char *强转为void *，这里没有强转为obj *的原因是因为只有一个区块，没有必要强转？
		if (nobjs == 1) return chunk;

		// 找到需要扩充的free list
		my_free_list = free_list + FREELIST_INDEX(n);

		// 第一个区块返回给调用者使用
		// 这里强转为obj *了
		// 这里为什么能强转？强转了就能用？不会丢失数据吗？union强转了会调用构造函数吗？（不是这样问，但是不清楚怎么表达）
		// 基础知识不牢，见笑了。
		/* 可以知道在确定的机子上，任何指针变量的大小是一定的，比如都是四字节 */
		result = (obj *)(chunk);

		obj *current_obj, *next_obj;
		// 实际上应该是chunk + n * sizeof(char)
		// 指针+数字： 需要调整，调整的权重为sizeof（指针只去掉一个*）
		*my_free_list = next_obj = (obj *)(chunk + n);

		// 将新配置的节点串起来
		for (int i = 1; ; ++i) {
			current_obj = next_obj;
			// 注意这里的转型偏移
			// 也就是这里其实是next_obj + n * sizeof(char)，sizeof(char)为1，正确的位置
			// 没有强转的话就是next_obj + n * sizeof(obj)，sizeof(obj)为4，错误的位置
			next_obj = (obj *)((char *)next_obj + n);
			if (nobjs - 1 == i) {
				current_obj->free_list_link = nullptr;
				break;
			}
			else
				current_obj->free_list_link = next_obj;
		}

		return result;
	}


	// 注意nobjs是传引用
	// 这里返回的是char *，因为malloc返回的是char *
	template <int inst>
	char *_default_alloc_template<inst>::chunk_alloc(std::size_t size, int &nobjs) {
		char *result;
		std::size_t total_bytes = size * nobjs;			// 所需bytes数
		std::size_t bytes_left = end_free - start_free;	// 内存池剩余bytes数


		if (bytes_left > total_bytes) {
			// 剩余水量充足
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else if (bytes_left >= size) {
			// 剩余水量不足以完全满足，但可以提供一个及以上的区块
			nobjs = bytes_left / size;
			total_bytes = size * nobjs;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else {
			// 剩余水量连一个区块都满足不了
			// 配置新的heap空间，为所需空间的两倍，最后那个是附加量
			std::size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);

			// 让内存池的残余零头有点价值，即将剩余空间编入对应的free list
			if (bytes_left > 0) {
				obj * volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((obj *)start_free)->free_list_link = *my_free_list;
				*my_free_list = (obj *)start_free;
			}

			// 配置heap空间，补充内存池
			start_free = (char *)std::malloc(bytes_to_get);
			if (start_free == nullptr) {
				// heap空间不足，配置失败
				// 查找是否有没有使用且区块够大的free list
				obj * volatile *my_free_list, *p;
				for (int i = size; i <= _MAX_BYTES; i += _ALIGN) {
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != nullptr) {
						// 如果确实有足够大未用区块，则调整free_list释出未用区块
						// 这句的意思是释放当前的一个可用区块加入内存池，再重新调用chunk_alloc分配区块
						// 要注意现在的内存池完全是空的，所以可以直接指定start_free，end_free
						*my_free_list = p->free_list_link;
						start_free = (char *)p;
						end_free = start_free + i;
						// 递归调用自己修正nobjs
						return chunk_alloc(size, nobjs);
					}
				}
				// 如果配置失败，到处都没内存用，则调用第一级配置器，因为第一级配置器有oom
				end_free = nullptr;
				start_free = (char *)malloc_alloc::allocate(bytes_to_get);
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			// 递归修正
			return chunk_alloc(size, nobjs);
		}
	}

}
