#pragma once
#include "Variable.h"
#include <queue>
#include <mutex>

class Object
{
public:
	VariableType getType() const { return Type; }
	Object() : Type(VariableType::Undefined), RefCount(0) {};
	virtual ~Object() {}

public:
	VariableType Type;
	int RefCount;
};

template <class T>
class Allocator
{
public:
	Allocator() {
		Make();
	}

	template <typename ...Args>
	T* Make(const Args&... args) {
		std::unique_lock lk(AllocLock);
		if (FreeList.empty()) {
			auto str = new T(args...);
			PointerList.push_back(str);
			return str;
		}
		else {
			auto idx = FreeList.front();
			FreeList.pop();
			auto& obj = PointerList[idx];
			obj->RefCount = 0;
			/*constexpr bool hasRealloc = requires(T& t) {
				t.Realloc(args...);
			};
			if constexpr (hasRealloc) {
				obj->Realloc(args...);
			}*/
			delete obj;
			obj = new T(args...);

			return obj;
		}
	}

	T* GetDefault() {
		return PointerList[0];
	}

	void Free() {
		for (int i = 1; i < PointerList.size(); i++) {
			if (PointerList[i]->RefCount == 0) {
				PointerList[i]->RefCount = -1;
				FreeList.push(i);
			}
		}
	}
private:

	std::mutex AllocLock;
	std::vector<T*> PointerList;
	std::queue<size_t> FreeList;
};
