#pragma once
#include <vector>

// class T���ő�MAXSIZE�m�ۉ\��PoolAllocator���������Ă�������
template<class T, size_t MAXSIZE> class PoolAllocator
{
public:
	// �R���X�g���N�^
	PoolAllocator() {
		// TODO: �K�v�ɉ����Ď������Ă�������

		// T * MAXSIZE���̗̈���m�ۂ��A�A�h���X��freeList�ɓ����
		for (size_t i = 0; i < MAXSIZE; ++i)
		{
			auto addr = (T*)malloc(sizeof(T));
			if (addr != nullptr) {
				//std::cout << addr << std::endl;
				freeList.push_back(addr);
			}
			else
			{
				// malloc���s���̏����i��: ��O�𓊂���j
				throw std::bad_alloc();
			}
		}
	}

	// �f�X�g���N�^
	~PoolAllocator() {
		// TODO: �K�v�ɉ����Ď������Ă�������

		// freeList�̃A�h���X���������
		for (auto& addr : freeList)
		{
			//std::cout << "����������� : " << addr << std::endl;
			free(addr);
		}


	}

	// �m�ۂł��Ȃ��ꍇ��nullptr��Ԃ����B
	T* Alloc() {
		// TODO: �������Ă�������

		// ��̏ꍇ�͊m�ۂł��Ȃ��̂�nullptr��Ԃ�
		if (freeList.empty()) {
			return nullptr;
		}

		//freelist����A�h���X�����o���Afreelist�����������
		T* addr = freeList.back();
		freeList.pop_back();
		return addr;
	}

	// Free(nullptr)�Ō듮�삵�Ȃ��悤�ɂ��鎖�B
	void Free(T* addr) {
		// TODO: �������Ă�������

		// nullptr�Ȃ牽�����Ȃ�
		if (addr == nullptr) { return; }

		//freelist�ɖ߂�
		freeList.push_back(addr);

	}

private:
	// TODO: �������Ă�������

	// ���g�p��T���i�[
	std::vector<T*> freeList;
};
