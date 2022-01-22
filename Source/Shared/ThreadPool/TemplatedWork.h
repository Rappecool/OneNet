#pragma once
#include "Work.h"

namespace Threading
{

	template<class T>
	class TemplatedWork : public Work
	{
	public:
		TemplatedWork(const T& aFunction);
		~TemplatedWork() = default;
		void DoWork() override;

	private:
		T myWork;
	};

	template<class T>
	inline TemplatedWork<T>::TemplatedWork(const T& aFunction) : myWork(aFunction)
	{}

	template<class T>
	inline void TemplatedWork<T>::DoWork()
	{
		myWork();
		myIsDone = true;
	}

}