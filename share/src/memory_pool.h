
#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include "types_def.h"

template <size_t data_size>
struct TMemoryNode
{
	char data[data_size];
	TMemoryNode* next;
	TMemoryNode() {
		clean_up();
	}

	void clean_up() {
		next = NULL;
	}
};

template <size_t data_size, size_t node_num>
class CMemoryAllocator
{
protected:
	typedef TMemoryNode<data_size> _TMyNode;
	struct TMyPool {
		_TMyNode nodes[node_num];
		TMyPool* next;
		TMyPool() {
			clean_up();
		}

		void clean_up() {
			next = NULL;
		}
	};

public:
	CMemoryAllocator() {
		m_pool = NULL;
		m_freeNode = NULL;
	}

	~CMemoryAllocator() {
		while (NULL != m_pool) {
			TMyPool* pool = m_pool;
			m_pool = m_pool->next;
			delete pool;
		}
	}

public:
	char* allocate() {
		if (NULL == m_freeNode) {
			add_block_memory();
		}
		_TMyNode* node = m_freeNode;
		m_freeNode = m_freeNode->next;
		node->next = NULL;
		return node->data;
	}

	void deallocate(char* p) {
		_TMyNode* node = (_TMyNode*)p;
		node->next = m_freeNode;
		m_freeNode = node;
	}

private:
	void add_block_memory() {
		TMyPool* pool = new TMyPool();
		pool->next = m_pool;
		m_pool = pool;
		for (int i = 0; i < node_num; ++i) {
			pool->nodes[i].next = (i > 0) ? (&pool->nodes[i - 1]) : NULL;
		}
		m_freeNode = &pool->nodes[node_num - 1];
	}

private:
	TMyPool* m_pool;
	_TMyNode* m_freeNode;
};

class CMemoryPool
{
public:
	CMemoryPool() {

	}

	~CMemoryPool() {

	}

	char* allocate(int n) {
		char* p = NULL;
		uint8 flag = 0xFF;
		int buffer_size = n + sizeof(flag);

		if (buffer_size <= 128) {
			p = m_node_128.allocate();
			flag = 1;
		}
		else if (buffer_size <= 128 * 2) {
			p = m_node_128x2.allocate();
			flag = 2;
		}
		else if (buffer_size <= 128 * 4) {
			p = m_node_128x4.allocate();
			flag = 4;
		}
		else if (buffer_size <= 128 * 8) {
			p = m_node_128x8.allocate();
			flag = 8;
		}
		else if (buffer_size <= 128 * 16) {
			p = m_node_128x16.allocate();
			flag = 16;
		}
		else if (buffer_size <= 128 * 32) {
			p = m_node_128x32.allocate();
			flag = 32;
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
			m_node_128.deallocate(p);
		}
		else if (flag == 2) {
			m_node_128x2.deallocate(p);
		}
		else if (flag == 4) {
			m_node_128x4.deallocate(p);
		}
		else if (flag == 8) {
			m_node_128x8.deallocate(p);
		}
		else if (flag == 16) {
			m_node_128x16.deallocate(p);
		}
		else if (flag == 32) {
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
	CMemoryAllocator<128, 128>		m_node_128;
	CMemoryAllocator<128 * 2, 64>	m_node_128x2;
	CMemoryAllocator<128 * 4, 32>	m_node_128x4;
	CMemoryAllocator<128 * 8, 16>	m_node_128x8;
	CMemoryAllocator<128 * 16, 16>	m_node_128x16;
	CMemoryAllocator<128 * 32, 16>	m_node_128x32;
};

template <class T, size_t max_count>
class CObjMemoryPool
{
public:
	CObjMemoryPool() {

	}

	~CObjMemoryPool() {

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
	CMemoryAllocator<sizeof(T), max_count> m_nodes;
};

#endif