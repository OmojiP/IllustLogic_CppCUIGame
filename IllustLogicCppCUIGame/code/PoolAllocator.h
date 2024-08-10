#pragma once
#include <vector>

// class Tを最大MAXSIZE個確保可能なPoolAllocatorを実装してください
template<class T, size_t MAXSIZE> class PoolAllocator
{
public:
	// コンストラクタ
	PoolAllocator() {
		// TODO: 必要に応じて実装してください

		// T * MAXSIZE分の領域を確保し、アドレスをfreeListに入れる
		for (size_t i = 0; i < MAXSIZE; ++i)
		{
			auto addr = (T*)malloc(sizeof(T));
			if (addr != nullptr) {
				//std::cout << addr << std::endl;
				freeList.push_back(addr);
			}
			else
			{
				// malloc失敗時の処理（例: 例外を投げる）
				throw std::bad_alloc();
			}
		}
	}

	// デストラクタ
	~PoolAllocator() {
		// TODO: 必要に応じて実装してください

		// freeListのアドレスを解放する
		for (auto& addr : freeList)
		{
			//std::cout << "メモリを解放 : " << addr << std::endl;
			free(addr);
		}


	}

	// 確保できない場合はnullptrを返す事。
	T* Alloc() {
		// TODO: 実装してください

		// 空の場合は確保できないのでnullptrを返す
		if (freeList.empty()) {
			return nullptr;
		}

		//freelistからアドレスを取り出し、freelistから消去する
		T* addr = freeList.back();
		freeList.pop_back();
		return addr;
	}

	// Free(nullptr)で誤動作しないようにする事。
	void Free(T* addr) {
		// TODO: 実装してください

		// nullptrなら何もしない
		if (addr == nullptr) { return; }

		//freelistに戻す
		freeList.push_back(addr);

	}

private:
	// TODO: 実装してください

	// 未使用のTを格納
	std::vector<T*> freeList;
};
