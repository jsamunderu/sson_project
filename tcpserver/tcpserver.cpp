#include <iostream>
#include "tcpserver.h"

namespace sson {

tcpserver::thread_pool::job_runner::job_runner(std::function<void(boost::asio::ip::tcp::socket& socket)> func,
			std::atomic<bool>& running, async_ring<std::unique_ptr<tcpserver::thread_pool::job_runner> >& pool)
	: running_(running), pool_(pool), func_(func), sleep_(true),
	thrd_(&tcpserver::thread_pool::job_runner::work, this)
{
}

void tcpserver::thread_pool::job_runner::work()
{
	while (running_.load(std::memory_order_acquire) == true)
	{
		{
			std::unique_lock<std::mutex> lock(mtx_);
			while (sleep_ == true)
			{
				cond_.wait(lock);
			}
		}

		if (socket_.get())
		{
			func_(*socket_.get());
			socket_->close();
			socket_.reset();
			{
				std::unique_lock<std::mutex> lock(mtx_);
				sleep_ = running_.load(std::memory_order_acquire);
			}
			std::unique_ptr<tcpserver::thread_pool::job_runner> me(this);
			pool_.push_wait(me);
		}
	}
}

void tcpserver::thread_pool::job_runner::set_work(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
	this->socket_ = socket;
}

void tcpserver::thread_pool::job_runner::finish()
{
	thrd_.join();
}

void tcpserver::thread_pool::job_runner::signal()
{
	std::lock_guard<std::mutex> lock(mtx_);
	sleep_ = false;
	cond_.notify_one();
}

tcpserver::thread_pool::thread_pool(std::size_t size, std::function<void(boost::asio::ip::tcp::socket& socket)> func)
	: pool_(size, [&](std::unique_ptr<tcpserver::thread_pool::job_runner>& dest,
		std::unique_ptr<tcpserver::thread_pool::job_runner>& src) {
		dest = std::move(src);
	})
{
	running_.store(true, std::memory_order_release);
	std::unique_ptr<tcpserver::thread_pool::job_runner> thrd;
	for (std::size_t i = 0; i < pool_.length(); i++)
	{
		thrd.reset(new tcpserver::thread_pool::job_runner(func, running_, pool_));
			// = std::make_unique<tcpserver::thread_pool::job_runner>(func, running_, pool_);
		pool_.push_wait(thrd);
	}
}

tcpserver::thread_pool::~thread_pool()
{
}

void tcpserver::thread_pool::shutdown()
{
	running_.store(false, std::memory_order_release);
	for (std::size_t i = 0; i < pool_.length(); i++)
	{
		std::unique_ptr<tcpserver::thread_pool::job_runner> thrd;
		pool_.pop_wait(thrd);
		thrd->signal();
		thrd->finish();
	}
}

bool tcpserver::thread_pool::run(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
	std::unique_ptr<tcpserver::thread_pool::job_runner> thrd;
	bool result = pool_.pop(thrd);
	if (result == false)
	{
		return false;
	}
	thrd->set_work(socket);
	thrd->signal();
	thrd.release(); // let it fly, when done it will put itself back into the pool
	return true;
}

tcpserver::tcpserver(int port, std::size_t sessions)
	: port_(port), thrd_pool_(sessions, std::bind(&tcpserver::process, this, std::placeholders::_1))
{
	running_.store(false, std::memory_order_release);
	std::stringstream ss;
	ss << "{ parent:object; { child:object; } }";
	input_serializer is(obj_);
	ss >> is;
}

tcpserver::~tcpserver()
{
	stop();
}

void tcpserver::start()
{
	if (running_.load(std::memory_order_acquire) == true)
	{
		return;
	}
	tcpserver::run();
}

void tcpserver::stop()
{
	if (running_.load(std::memory_order_acquire) == true)
	{
		running_.store(false, std::memory_order_release);

		ios_.stop();

		thrd_pool_.shutdown();
	}
}

void tcpserver::run()
{
	try
	{
		acceptor_.reset(new boost::asio::ip::tcp::acceptor(ios_,
			boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_)));
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return;
	}

	running_.store(true, std::memory_order_release);

	while (running_.load(std::memory_order_acquire) == true)
	{
		try
		{
			ios_.reset();

			std::shared_ptr<boost::asio::ip::tcp::socket> socket = std::make_shared<boost::asio::ip::tcp::socket>(ios_);
			boost::system::error_code ec;
			acceptor_->async_accept(*socket.get(), [&](const boost::system::error_code& error)->void {
				ec = error;
				if (!ec)
				{
					handle(socket);
				}
			});

			ios_.run_one();

			if (ec)
			{
				std::cerr << ec.message() << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			ios_.reset();
			acceptor_.reset(new boost::asio::ip::tcp::acceptor(ios_,
				boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_)));
		}
	}
}

void tcpserver::handle(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
	// terminate connection if there is no session
	if (thrd_pool_.run(socket) == false)
	{
		boost::system::error_code error;
		boost::asio::write(*socket.get(), boost::asio::buffer("Server Busy! Try again Later."),
			boost::asio::transfer_all(), error);
		if (error)
		{
			std::cerr << error.message() << std::endl;
		}
		socket->close();
	}
}

void tcpserver::process(boost::asio::ip::tcp::socket& socket)
{
	std::stringstream packet;
	packet << sson::output_serializer(obj_);
	boost::system::error_code error;
	boost::asio::write(socket, boost::asio::buffer(packet.str()), boost::asio::transfer_all(), error);
	if (error)
	{
		std::cerr << error.message() << std::endl;
	}
	socket.close();
}

}
