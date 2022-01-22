#pragma once
#include <atomic>

namespace Threading
{
	class Work
	{
	public:
		Work() = default;
		~Work() = default;

		virtual void DoWork() = 0;
		inline bool IsDone() const;

	protected:
		std::atomic<bool> myIsDone;
	};

	bool Work::IsDone() const
	{
		return myIsDone;
	}
}