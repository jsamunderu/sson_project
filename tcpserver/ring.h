#ifndef SSON_RING_H
#define SSON_RING_H
#include <memory>
#include <cstddef>

namespace sson
{
	template<typename T>
	class ring
	{
		std::unique_ptr<T[]> buffer_;
		std::function<void(T&, T&)> func_;
		std::size_t size_;
		std::size_t mask_;
		std::size_t begin_;
		std::size_t end_;
		std::size_t counter_;
		std::size_t next_pwr_of_2(std::size_t num)
		{
			if (num < 0)
			{
				return 0;
			}
			--num;
			num |= num >> 1;
			num |= num >> 2;
			num |= num >> 4;
			num |= num >> 8;
			num |= num >> 16;
			return num + 1;
		}

	public:
		ring(std::size_t size, std::function<void(T&, T&)> func = [&](T& dest, T& src) { dest=src; })
			: func_(func), begin_(0), end_(0), counter_(0)
		{
			size_ = next_pwr_of_2(size);
			mask_ = size_ - 1;
			buffer_.reset(new T[size_]);
		}

		~ring()
		{
		}

		ring(const ring& r)
		{
			for (std::size_t i; i < size_; i++)
			{
				func_(buffer_[i], r.at(i));
			}
		}

		ring& operator=(const ring& r)
		{
			if (this == &r)
			{
				return;
			}
			for (std::size_t i; i < size_; i++)
			{
				func_(buffer_[i], r.at(i));
			}
			return this;
		}

		T& at(std::size_t pos)
		{
			if (pos >(size_ - 1))
			{
				pos = begin_;
			}
			return buffer_[pos];
		}

		bool push(T& item)
		{
			if (counter_ == size_)
			{
				return false;
			}
			func_(buffer_[end_], item);
			end_ = (end_ + 1) & mask_;
			++counter_;
			return true;
		}

		bool pop(T& item)
		{
			if (counter_ == 0)
			{
				return false;
			}
			func_(item, buffer_[begin_]);
			begin_ = (begin_ + 1) & mask_;
			--counter_;
			return true;
		}

		std::size_t length()
		{
			return size_;
		}

		std::size_t space()
		{
			return size_ - counter_;
		}
	};
}
#endif
