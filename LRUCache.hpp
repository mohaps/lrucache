/* 
 *
 * LRUCache - a templated LRU cache class that allows specification of
 * key, value and optionally the map container type (defaults to std::map)
 * By using the std::map and a linked list of keys it allows O(1) 
 * insert, delete and refresh operations
 
 * Copyright (c) 2012-22 SAURAV MOHAPATRA <mohaps@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef _MOHAPS_LRU_CACHE_HPP_
#define _MOHAPS_LRU_CACHE_HPP_

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <map>
#include <iostream>
#include <exception>

namespace lru
{
	// a double linked list node
	template<class Key, class Value>
	struct Node
	{
		Node* prev;
		Node* next;
		Key key;
		Value value;
		Node(const Key& keyObj, const Value& valueObj) : prev(0), next(0), key(keyObj), value(valueObj){}
		virtual ~Node(){ cleanup(); }
		void cleanup()
		{ 
			if(next){ delete next; } 
			next = 0; 
			prev = 0; 
		}
		void unlink()
		{
			if(next){ next->prev = prev; }
			if(prev){ prev->next = next; }
			next = 0;
			prev = 0;
		}
		template<class Visitor>
		void walk(Visitor& visitorFunc)
		{
			visitorFunc(*this);
			if(this->next)
			{
				this->next->walk<Visitor>(visitorFunc);
			}
		}
	};
	
	
	// a doubly linked list class
	template<class Key, class Value>
	struct List
	{
		Node<Key, Value>* head;
		Node<Key, Value>* tail;
		size_t size;
		List() : head(0), tail(0), size(0){}
		virtual ~List(){ clear(); }
		void clear()
		{
			if(head){ delete head; }
			head = 0;
			tail = 0;
			size = 0;
		}
		Node<Key, Value>* pop()
		{
			if(!head){ return 0; }
			else
			{
				Node<Key, Value>* newHead = head->next;
				head->unlink();
				Node<Key, Value>* oldHead = head;
				head = newHead;
				size--;
				if(size == 0){ tail = 0; }
				return oldHead;
			} 
		}
		Node<Key, Value>* remove(Node<Key, Value>* node)
		{
			if(node == head){ head = node->next; }
			if(node == tail){ tail = node->prev; }
			node->unlink();
			size--;
			return node;
		}
		void push(Node<Key, Value>* node)
		{
			node->unlink(); 
			if(!head){ head = node; }
			else if (head == tail)
			{
				head->next = node;
				node->prev = head;
			}
			else
			{
				tail->next = node;
				node->prev = tail;
			}
			tail = node; 
			size++;
		}
	};
	
	// the LRU Cache class
	template<class Key, class Value, class MapType = std::map<Key, Node<Key,Value>* > >
	class Cache
	{
	public:
		class KeyNotFound : public std::exception
		{
		public:
			const char* what() const throw() { return "KeyNotFound"; }
		};
		// -- methods
		Cache(size_t maxSize=64, size_t elasticity=10) : m_maxSize(maxSize), m_elasticity(elasticity){}
		virtual ~Cache(){}
		void clear(){ m_cache.clear(); m_keys.clear(); }
		void insert(const Key& key, const Value& value)		
		{
			typename MapType::iterator iter = m_cache.find(key);
			if(iter != m_cache.end())
			{
				m_keys.remove(iter->second);
				m_keys.push(iter->second);
			}
			else
			{
				Node<Key, Value>* n = new Node<Key, Value>(key, value);
				m_cache[key] = n;
				//std::cout<<"insering ("<<key<<")=("<<value<<")"<<std::endl;
				m_keys.push(n);
				prune();
			}
			
		}

		const Value& get(const Key& key)
		{
			//std::cout<<"get("<<key<<")"<<std::endl;
			typename MapType::iterator iter = m_cache.find(key);
			if(iter == m_cache.end())
			{
				throw KeyNotFound();
			}
			m_keys.remove(iter->second);
			m_keys.push(iter->second);
			return iter->second->value;
			
		}
		void remove(const Key& key)
		{
			typename MapType::iterator iter = m_cache.find(key);
			if(iter != m_cache.end())
			{
				m_keys.remove(iter->second);
				m_cache.erase(iter);
			}
		}
		bool contains(const Key& key)
		{
			return m_cache.find(key) != m_cache.end();
		}
		static void printVisitor(const Node<Key, Value>& node)
		{
			std::cout<<"{"<<node.key<<":"<<node.value<<"}"<<std::endl;
		}
		
		void dumpDebug(std::ostream& os) const
		{
			std::cout<<"Cache Size : "<<m_cache.size()<<" (max:"<<m_maxSize<<") (elasticity: "<<m_elasticity<<")"<<std::endl;
			if(m_keys.head)
			{
				m_keys.head->walk(printVisitor);
			}
			
		}
	protected:
		size_t prune()
		{
			if (m_maxSize> 0 && m_cache.size() >= (m_maxSize + m_elasticity))
			{
				size_t count = 0;
				while(m_cache.size() > m_maxSize)
				{
					Node<Key, Value>* n = m_keys.pop();
					m_cache.erase(n->key);
					delete n;
					count++;
				}
				return count;
			}
			else
			{
				return 0;
			}
		}
	private:
		// -- handy typedefs
		MapType m_cache;
		List<Key, Value> m_keys;
		size_t m_maxSize;
		size_t m_elasticity;
	private:
		Cache(const Cache&);
		const Cache& operator = (const Cache&);
		
	};
	
}
#endif