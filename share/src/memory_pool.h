
#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include "types_def.h"

template <size_t data_size>
struct memory_node
{
	char data[data_size];
	memory_node* next;
	memory_node() {
		clean_up();
	}

	void clean_up() {
		next = NULL;
	}
};

template <size_t data_size, size_t node_num>
class memory_allocator
{
protected:
	typedef memory_node<data_size> my_node;
	struct my_pool {
		my_node nodes[node_num];
		my_pool* next;
		my_pool() {
			clean_up();
		}

		void clean_up() {
			next = NULL;
		}
	};

public:
	memory_allocator() {
		m_pool = NULL;
		m_free_node = NULL;
	}

	~memory_allocator() {
		while (NULL != m_pool) {
			my_pool* pool = m_pool;
			m_pool = m_pool->next;
			delete pool;
		}
	}

public:
	char* allocate() {
		if (NULL == m_free_node) {
			add_block_memory();
		}
		my_node* node = m_free_node;
		m_free_node = m_free_node->next;
		node->next = NULL;
		return node->data;
	}

	void deallocate(char* p) {
		my_node* node = (my_node*)p;
		node->next = m_free_node;
		m_free_node = node;
	}

private:
	void add_block_memory() {
		my_pool* pool = new my_pool();
		pool->next = m_pool;
		m_pool = pool;
		for (int i = 0; i < node_num; ++i) {
			pool->nodes[i].next = (i > 0) ? (&pool->nodes[i - 1]) : NULL;
		}
		m_free_node = &pool->nodes[node_num - 1];
	}

private:
	my_pool* m_pool;
	my_node* m_free_node;
};

class memory_pool
{
public:
	memory_pool() {

	}

	~memory_pool() {

	}

	char* allocate(int n) {
		char* p = NULL;
		uint8 flag = 0xFF;
		int buffer_size = n + sizeof(flag);

		if (buffer_size <= 32) {
			p = m_node_32.allocate();
			flag = 1;
		}
		else if (buffer_size <= 64) {
			p = m_node_64.allocate();
			flag = 2;
		}
		else if (buffer_size <= 128) {
			p = m_node_128.allocate();
			flag = 3;
		}
		else if (buffer_size <= 128 * 2) {
			p = m_node_128x2.allocate();
			flag = 4;
		}
		else if (buffer_size <= 128 * 4) {
			p = m_node_128x4.allocate();
			flag = 5;
		}
		else if (buffer_size <= 128 * 8) {
			p = m_node_128x8.allocate();
			flag = 6;
		}
		else if (buffer_size <= 128 * 16) {
			p = m_node_128x16.allocate();
			flag = 7;
		}
		else if (buffer_size <= 128 * 32) {
			p = m_node_128x32.allocate();
			flag = 8;
		}
		else {
			p = new char[buffer_size];
			flag = 0xFF;
		}

		if (NULL != p) {
			*(uint8*)p = flag;
			p += sizeof(flag);
		}
		
		return p;
			
	}

	void deallocate(char* p) {
		p -= sizeof(uint8);
		uint8 flag = *(uint8*)p;
		if (flag == 1) {
			m_node_32.deallocate(p);
		}
		else if (flag == 2) {
			m_node_64.deallocate(p);
		}
		else if (flag == 3) {
			m_node_128.deallocate(p);
		}
		else if (flag == 4) {
			m_node_128x2.deallocate(p);
		}
		else if (flag == 5) {
			m_node_128x4.deallocate(p);
		}
		else if (flag == 6) {
			m_node_128x8.deallocate(p);
		}
		else if (flag == 7) {
			m_node_128x16.deallocate(p);
		}
		else if (flag == 8) {
			m_node_128x32.deallocate(p);
		}
		else {
			if (NULL != p) {
				delete[] p;
			}
			p = NULL;
		}
	}

private:
	memory_allocator<32, 512>		m_node_32;
	memory_allocator<64, 256>		m_node_64;
	memory_allocator<128, 128>		m_node_128;
	memory_allocator<128 * 2, 64>	m_node_128x2;
	memory_allocator<128 * 4, 32>	m_node_128x4;
	memory_allocator<128 * 8, 16>	m_node_128x8;
	memory_allocator<128 * 16, 16>	m_node_128x16;
	memory_allocator<128 * 32, 16>	m_node_128x32;
};

template <class T, size_t max_count>
class obj_memory_pool
{
public:
	obj_memory_pool() {

	}

	~obj_memory_pool() {

	}

public:
	T* allocate() {
		char* node = m_nodes.allocate();
		return new (node)T;
	}


	void deallocate(T* p) {
		p->~T();
		m_nodes.deallocate((char*)p);
	}
private:
	memory_allocator<sizeof(T), max_count> m_nodes;
};

#endif