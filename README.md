LRUCache
========

a header only C++ LRU Cache template class that allows you to define key, value and optionally the Map type. uses a double linked list and a std::map style container to provide fast insert, delete and update

No dependencies other than the C++ standard library.


Usage Example
---------------
```cpp
#include "LRUCache.hpp"
namespace lru
{
	typedef std::string String;
	void test()
	{
		lru::Cache<String, String> cache(3,0);
		cache.insert("hello", "world");
		cache.insert("foo", "bar");
		cache.dumpDebug(std::cout<<"--> After 2 inserts"<<std::endl);
		
		std::cout<<"checking refresh : "<<cache.get("hello")<<std::endl;
		cache.dumpDebug(std::cout<<"--> After refeshing oldest key"<<std::endl);
		
		cache.insert("hello1", "world1");
		cache.insert("foo1", "bar1");
		cache.dumpDebug(std::cout<<"--> After adding two more"<<std::endl);
	}
}

int main(int argc, char **argv)
{
	lru::test();
	return 0;
}
```


License
-------

BSD License


Comments/Crits
---------------

Please contact author at mohaps@gmail.com
