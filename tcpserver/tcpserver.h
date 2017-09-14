#ifndef SSON_TCPSERVER_H
#define SSON_TCPSERVER_H
#include <memory>
#include <fstream>
#include <functional>
#include <atomic>
#include <thread>
#include <boost/asio.hpp>
#include "async_ring.h"
#include "sson.h"

namespace sson
{
	class tcpserver
	{
		struct thread_pool
		{
			struct job_runner
			{
				job_runner(std::function<void(boost::asio::ip::tcp::socket& socket)> func, std::atomic<bool>& running,
					async_ring<std::unique_ptr<job_runner> >& pool);
				void work();
				void set_work(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
				void finish();
				void signal();
			private:
				std::atomic<bool>& running_;
				async_ring<std::unique_ptr<tcpserver::thread_pool::job_runner> >& pool_;
				std::function<void(boost::asio::ip::tcp::socket& socket)> func_;
				bool sleep_;
				std::thread thrd_;
				std::condition_variable cond_;
				std::mutex mtx_;
				std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
			};
			thread_pool(std::size_t size, std::function<void(boost::asio::ip::tcp::socket& socket)> func);
			~thread_pool();
			thread_pool(const thread_pool&) = delete;
			thread_pool& operator=(const thread_pool&) = delete;
			void shutdown();
			bool run(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
		private:
			async_ring<std::unique_ptr<job_runner> > pool_;
			std::atomic<bool> running_;
		};

		int port_;
		std::atomic<bool> running_;
		thread_pool thrd_pool_;
		boost::asio::io_service ios_;
		std::unique_ptr<boost::asio::ip::tcp::acceptor>	acceptor_;
		object obj_;

		void run();
		void handle(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
		void process(boost::asio::ip::tcp::socket& socket);
	public:
		tcpserver(int port, std::size_t sessions);
		~tcpserver();

		tcpserver(const tcpserver& server) = delete;
		tcpserver& operator=(const tcpserver& server) = delete;

		void start();
		void stop();
	};
}
#endif
