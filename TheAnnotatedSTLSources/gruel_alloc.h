#pragma once


#include <new>
#include <iostream>


namespace gruel {

	/* ��һ�������� */
	/* ���instû���� */
	template <int inst>
	class _malloc_alloc_template {
	public:
		// ����Ϊʲô����void *��ָ�룬����Ϊ������ָ���������ǿתΪ�������͵�ָ��
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


		/* ����ģ��C++�е�set_new_handler����Ϊ���ǲ�����operator newʵ�ֵ� */
		/* ��������β�÷��أ��Ƚ������Ķ� */
		static auto set_malloc_handler(void (*f)()) -> void(*)() {
			auto old = _malloc_alloc_oom_handler;
			_malloc_alloc_oom_handler = f;
			return old;
		}
	private:
		/* "oom": out of memory */
		static void *oom_malloc(std::size_t);
		static void *oom_realloc(void *, std::size_t);
		// _malloc_alloc_oom_handler��һ������ָ�룬�ú�������void
		static void(*_malloc_alloc_oom_handler)();	
	};


	/* ��ֵΪ�գ��д��Ͷ��趨 */
	template <int inst>
	void(*_malloc_alloc_template<inst>::_malloc_alloc_oom_handler)() = nullptr;


	template <int inst>
	void *_malloc_alloc_template<inst>::oom_malloc(std::size_t n) {
		void(*my_malloc_handler)();
		void *result;

		while (1) {
			// ����дΪ�˶��߳̿��ǣ�
			my_malloc_handler = _malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr)
				throw std::bad_alloc();
			// ���ô�������
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
	
	// ֱ�ӽ�inst�趨Ϊ0
	using malloc_alloc = _malloc_alloc_template<0>;


/* ----------------------------------------------------------------------------- */


	// 64λ�����£�8����Ȼ����
	const std::size_t _ALIGN = 8;
	const std::size_t _MAX_BYTES = 128;
	const std::size_t _NFREELISTS = _MAX_BYTES / _ALIGN;


	/* �ڶ��������� */
	/* ��ʵ�ֲ����Ƕ��̣߳�inst���� */
	// ��ʵ�ֹ������õ��˺ܶ�����ǿת��Ҫע��
	template <int inst>
	class _default_alloc_template {
	private:

		// ��bytes�ϵ���8�ı���
		/* ԭ��(ALIGN - 1)�����Ʊ�ʾ��0000 0111����~(ALIGN - 1)�����Ʊ�ʾ��1111 1000��
		��bytes��8�ı���ʱ������ı䣬��bytes����8�ı���ʱ����bytes = 8n + p, p < 8��
		����0000 0111�󣬱�Ϊbytes = 8(n + 1) + p��p < 8���ٺ�1111 1000����������3λ��
		�õ�8�ı��� */
		static std::size_t ROUND_UP(std::size_t bytes) {
			return ((bytes + (_ALIGN - 1)) & ~(_ALIGN - 1));
		}

		/* free_list�Ľڵ�ṹ */
		// �����union��СΪ4�ֽڣ���Ϊָ��ռ��4�ֽڣ�char����ռ��1�ֽڣ�ȡ��ֵ
		union obj {
			union obj *free_list_link;	// �Ӹ��ֶι�֮��obj����Ϊһ��ָ��ָ����һ��obj
			char client_data[1];		// �Ӹ��ֶι�֮��obj����Ϊһ��ָ��ָ��ʵ������
		};

		// 16��free_list
		static obj * volatile free_list[_NFREELISTS];

		// ȷ��ʹ�õ�n�ŵ�free_list��n��0��ʼ
		static std::size_t FREELIST_INDEX(std::size_t bytes) {
			return (((bytes + _ALIGN - 1) / _ALIGN) - 1);
		}

		// ����һ����СΪn�Ķ��󣬲����ܼ����СΪn���������鵽free list
		static void *refill(std::size_t n);

		// ����һ���ռ䣬������nobjs����СΪ"size"������
		// ����޷�����nobjs���ռ䣬��nobjs���ܻή��
		static char *chunk_alloc(std::size_t size, int &nobjs);

		static char *start_free;	// �ڴ�ص���ʼλ�ã�ֻ��chunk_alloc�б仯
		static char *end_free;		// �ڴ�صĽ���λ�ã�ֻ��chunk_alloc�б仯	
		static std::size_t heap_size;

	public:
		static void *allocate(std::size_t n) {

			obj * volatile *my_free_list;	// ��obj ** my_free_list��volatile���ڶ��߳�
			/* volatile�����ñ��������Ըñ����ķ��ʴ�������Ż�������˵ÿ�η��ʸñ�������
			���ڴ����ٴζ�ȡһ�飬���߳��б���������Ѹñ�������Ĵ����У����⵼�¶���̷߳���
			�ı�����һ�� */
			obj *result;

			// ����128bytes�͵��õ�һ��������
			if (n > static_cast<std::size_t>(_MAX_BYTES))
				return malloc_alloc::allocate(n);

			// Ѱ��16��free list���ʵ���һ��
			my_free_list = free_list + FREELIST_INDEX(n);
			result = *my_free_list;

			if (result == nullptr) {
				void *r = refill(ROUND_UP(n));
				return r;
			}

			// ����free list����result����ȥ
			*my_free_list = result->free_list_link;
			return result;
		}

		static void deallocate(void *p, std::size_t n) {
			obj *q = (obj *)(p);	// ǿת
			obj * volatile *my_free_list;

			if (n > (std::size_t)(_MAX_BYTES)) {
				malloc_alloc::deallocate(p, n);
				return;
			}

			// Ѱ�Ҷ�Ӧ��free list
			my_free_list = free_list + FREELIST_INDEX(n);
			q->free_list_link = *my_free_list;
			*my_free_list = q;
		}

		// ����ʵ��
		static void *reallocate(void *p, std::size_t old_sz, std::size_t new_sz);

	
	};

	/* static data member�Ķ���ͳ�ֵ�趨 */
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
		// ע�����ﴫ���nobjs�����ã�ʵ��nobjs���ܻ��޸�
		char *chunk = chunk_alloc(n, nobjs);
		obj * volatile *my_free_list;
		obj *result;

		// ���ֻ���һ�����飬��ֱ�ӷ��ظ������ߣ�free list�ޱ仯
		// ���Է�����char *ǿתΪvoid *������û��ǿתΪobj *��ԭ������Ϊֻ��һ�����飬û�б�Ҫǿת��
		if (nobjs == 1) return chunk;

		// �ҵ���Ҫ�����free list
		my_free_list = free_list + FREELIST_INDEX(n);

		// ��һ�����鷵�ظ�������ʹ��
		// ����ǿתΪobj *��
		// ����Ϊʲô��ǿת��ǿת�˾����ã����ᶪʧ������unionǿת�˻���ù��캯���𣿣����������ʣ����ǲ������ô��
		// ����֪ʶ���Σ���Ц�ˡ�
		/* ����֪����ȷ���Ļ����ϣ��κ�ָ������Ĵ�С��һ���ģ����綼�����ֽ� */
		result = (obj *)(chunk);

		obj *current_obj, *next_obj;
		// ʵ����Ӧ����chunk + n * sizeof(char)
		// ָ��+���֣� ��Ҫ������������Ȩ��Ϊsizeof��ָ��ֻȥ��һ��*��
		*my_free_list = next_obj = (obj *)(chunk + n);

		// �������õĽڵ㴮����
		for (int i = 1; ; ++i) {
			current_obj = next_obj;
			// ע�������ת��ƫ��
			// Ҳ����������ʵ��next_obj + n * sizeof(char)��sizeof(char)Ϊ1����ȷ��λ��
			// û��ǿת�Ļ�����next_obj + n * sizeof(obj)��sizeof(obj)Ϊ4�������λ��
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


	// ע��nobjs�Ǵ�����
	// ���ﷵ�ص���char *����Ϊmalloc���ص���char *
	template <int inst>
	char *_default_alloc_template<inst>::chunk_alloc(std::size_t size, int &nobjs) {
		char *result;
		std::size_t total_bytes = size * nobjs;			// ����bytes��
		std::size_t bytes_left = end_free - start_free;	// �ڴ��ʣ��bytes��


		if (bytes_left > total_bytes) {
			// ʣ��ˮ������
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else if (bytes_left >= size) {
			// ʣ��ˮ����������ȫ���㣬�������ṩһ�������ϵ�����
			nobjs = bytes_left / size;
			total_bytes = size * nobjs;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else {
			// ʣ��ˮ����һ�����鶼���㲻��
			// �����µ�heap�ռ䣬Ϊ����ռ������������Ǹ��Ǹ�����
			std::size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);

			// ���ڴ�صĲ�����ͷ�е��ֵ������ʣ��ռ�����Ӧ��free list
			if (bytes_left > 0) {
				obj * volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((obj *)start_free)->free_list_link = *my_free_list;
				*my_free_list = (obj *)start_free;
			}

			// ����heap�ռ䣬�����ڴ��
			start_free = (char *)std::malloc(bytes_to_get);
			if (start_free == nullptr) {
				// heap�ռ䲻�㣬����ʧ��
				// �����Ƿ���û��ʹ�������鹻���free list
				obj * volatile *my_free_list, *p;
				for (int i = size; i <= _MAX_BYTES; i += _ALIGN) {
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != nullptr) {
						// ���ȷʵ���㹻��δ�����飬�����free_list�ͳ�δ������
						// ������˼���ͷŵ�ǰ��һ��������������ڴ�أ������µ���chunk_alloc��������
						// Ҫע�����ڵ��ڴ����ȫ�ǿյģ����Կ���ֱ��ָ��start_free��end_free
						*my_free_list = p->free_list_link;
						start_free = (char *)p;
						end_free = start_free + i;
						// �ݹ�����Լ�����nobjs
						return chunk_alloc(size, nobjs);
					}
				}
				// �������ʧ�ܣ�������û�ڴ��ã�����õ�һ������������Ϊ��һ����������oom
				end_free = nullptr;
				start_free = (char *)malloc_alloc::allocate(bytes_to_get);
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			// �ݹ�����
			return chunk_alloc(size, nobjs);
		}
	}

}
