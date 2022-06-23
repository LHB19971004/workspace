#pragma once
#include "std.h"

class ThreadPool
{
public:
	using Task = std::function<void()>;
	ThreadPool(int32_t thread_count);
	~ThreadPool();
	void post(Task task);
private:
	void start(int32_t thread_count);
	void proc();
	void stop();
	Task getTask();
private:
	boost::asio::detail::thread_group threads_;
	bool exit_ = false;
	std::mutex mutex_;
	std::mutex condition_mutex_;
	std::condition_variable cv_;
	list<Task> task_queue_;
	int32_t thread_count_;
};

