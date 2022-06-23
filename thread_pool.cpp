#include "thread_pool.h"



ThreadPool::ThreadPool(int32_t thread_count):thread_count_(thread_count)
{
	start(thread_count);
}


ThreadPool::~ThreadPool()
{
	stop();
}

ThreadPool::Task ThreadPool::getTask()
{
	std::lock_guard<std::mutex> _(mutex_);
	if(task_queue_.empty())
		return NULL;

	auto task = task_queue_.front();
	task_queue_.pop_front();
	return task;
}

void ThreadPool::proc()
{
	while (!exit_)
	{
		{
			std::unique_lock<std::mutex> lock(condition_mutex_);
			cv_.wait(lock, [&] {
				return exit_ || !task_queue_.empty(); });
		}

		if (exit_)
		{
			break;
		}

		auto task = getTask();
		if (task)
		{
			task();
		}
	}
}

void ThreadPool::start(int32_t thread_count)
{
	if (threads_.empty()) {
		threads_.create_threads(std::bind(&ThreadPool::proc, this), thread_count);
	}
}

void ThreadPool::stop()
{
	{
		std::lock_guard<std::mutex> _(mutex_);
		if (exit_) {
			return;
		}
		exit_ = true;
		cv_.notify_all();
	}
	if (!threads_.empty()) {
		threads_.join();
	}
}

void ThreadPool::post(Task task)
{
	std::lock_guard<std::mutex> _(mutex_);
	task_queue_.emplace_back(task);
	cv_.notify_one();
}