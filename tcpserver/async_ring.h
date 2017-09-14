#ifndef SSON_ASYNC_RING_H
#define SSON_ASYNC_RING_H
#include "ring.h"
#include <mutex>
#include <condition_variable>
namespace sson {

	template<typename T>
	class async_ring {
		ring<T> ring_;
		std::size_t insert_waiters_;
		std::size_t remove_waiters_;
		std::mutex mtx_;
		std::condition_variable insert_cond_;
		std::condition_variable remove_cond_;
	public:
		async_ring(std::size_t size, std::function<void(T&, T&)> func = [&](T& dest, T& src) { dest=src; })
			: ring_(size, func), insert_waiters_(0), remove_waiters_(0)
		{
		}

		~async_ring() {}

		async_ring(const async_ring&) = delete;
		async_ring& operator=(const async_ring&) = delete;

		bool push(T& item)
		{
			std::lock_guard<std::mutex> lock(mtx_);
			if (ring_.push(item) == false)
			{
				return false;
			}
			if (remove_waiters_ > 0)
			{
				remove_cond_.notify_one();
			}
			return true;
		}

		bool pop(T& item)
		{
			std::lock_guard<std::mutex> lock(mtx_);
			if (ring_.pop(item) == false)
			{
				return false;
			}
			if (insert_waiters_ > 0)
			{
				insert_cond_.notify_one();
			}
			return true;
		}

		void push_wait(T& item)
		{
			std::unique_lock<std::mutex> lock(mtx_);
			++insert_waiters_;
			while (ring_.push(item) == false)
			{
				insert_cond_.wait(lock);
			}
			--insert_waiters_;
			if (remove_waiters_ > 0)
			{
				remove_cond_.notify_one();
			}
		}

		void pop_wait(T& item)
		{
			std::unique_lock<std::mutex> lock(mtx_);
			++remove_waiters_;
			while (ring_.pop(item) == false)
			{
				remove_cond_.wait(lock);
			}
			--remove_waiters_;
			if (insert_waiters_ > 0)
			{
				insert_cond_.notify_one();
			}
		}

		std::size_t length()
		{
			return ring_.length();
		}

		std::size_t space()
		{
			std::lock_guard<std::mutex> lock(mtx_);
			return ring_.space();
		}
	};
}
#endif
