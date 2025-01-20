#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <stop_token>
#include <string>
#include <syncstream>
#include <thread>

using namespace std::chrono_literals;

std::atomic<bool> stopped(false);

class BgThreadDispatcher
{
private:
	using Task = std::function<void()>;
	using osync = std::osyncstream;

	std::jthread thr;
	std::queue<Task> queue;

	std::condition_variable queueCondVar;
	std::condition_variable waitCondVar;

	std::mutex queueMutex;
	std::mutex waitMutex;

	std::stop_source stopSource;
	std::stop_token sToken;

public:
	BgThreadDispatcher(const BgThreadDispatcher&) = delete;
	BgThreadDispatcher& operator=(const BgThreadDispatcher&) = delete;

	BgThreadDispatcher() : thr() {}

	~BgThreadDispatcher()
	{
		stopSource.request_stop();
	}

	// если создать поток в конструкторе могут не успеть проинициализироваться поля объекта т.к. поток стартует сразу и не ждет
	void StartThread()
	{
		std::stop_token stopToken = stopSource.get_token();
		this->sToken = stopToken;

		thr = std::jthread{
			&BgThreadDispatcher::ThreadHandler,
			this,
			stopToken
		};
	}

	void ThreadHandler(std::stop_token stopToken)
	{
		std::stop_callback stopCallback(
			stopToken,
			[this]() {
				queueCondVar.notify_all();
			});

		while (true)
		{
			std::unique_lock<std::mutex> lock{ queueMutex };
			queueCondVar.wait(lock, [this] () {
				return !queue.empty() || sToken.stop_requested();
			});

			if(stopToken.stop_requested() || stopped.load()) {
				break;
			}

			auto task = queue.front();
			queue.pop();
			queueMutex.unlock();

			waitCondVar.notify_one();

			task();
		}
	}

	void Dispatch(Task task)
	{
		if (stopped.load())
			return;

		std::lock_guard<std::mutex> lock{ queueMutex };
		queue.push(task);
		queueCondVar.notify_one();
	}

	void Wait()
	{
		if (stopped.load())
			return;

		std::unique_lock<std::mutex> lock{ waitMutex };
		waitCondVar.wait(lock, [this] () {
			return queue.empty();
		});
	}

	void Stop()
	{
		stopped.store(true);
	}
};

int main()
{
	using osync = std::osyncstream;

	BgThreadDispatcher dispatcher;
	dispatcher.StartThread();

//	dispatcher.Dispatch([]{ osync(std::cout) << "1"; }); // Выведет 1 в фоновом потоке
//	dispatcher.Wait(); // Дождётся завершения фоновых задач
//	osync(std::cout) << "!"; // Выведет ! в основном потоке
//  +
//	dispatcher.Dispatch([]{ osync(std::cout) << "1"; }); // выведет 1 в фоновом потоке
//	dispatcher.Dispatch([]{ osync(std::cout) << "2"; }); // Выведет 2 в фоновом потоке
//	osync(std::cout) << "!"; // Выведет ! в основном потоке
//	dispatcher.Wait(); // Дождётся печати 1 и 2
//	dispatcher.Dispatch([]{ osync(std::cout) << "3"; }); // Выведет 3 в фоновом потоке
//	dispatcher.Wait(); // Дождётся вывода 3
//  +
//	dispatcher.Dispatch([]{ osync(std::cout) << "1"; }); // Выводим 1 в фоновом потоке
//	osync(std::cout) << "!"; // Выводим ! в основном потоке
//	// Вывод 1 не гарантируется, так как мы не вызвали Wait перед вызовом деструктора.
//  +
//	dispatcher.Dispatch([] { osync(std::cout) << "1"; }); // Выведет 1 в фоновом потоке
//	osync(std::cout) << "!"; // Выведет ! в основном потоке
//	// За 1 секунду фоновый поток успеет вывести 1
//	std::this_thread::sleep_for(std::chrono::seconds(1));
//  +
//	dispatcher.Wait(); // Фоновых задач нет, метод сразу вернёт управление
//	std::cout << "!";  // Выведет ! в главном потоке.
//  +
	dispatcher.Dispatch([] { osync(std::cout) << "1"; }); // Выведет 1 в фоновом потоке
	// 1 секунды, скорее всего, хватит, чтобы успела вывестись 1.
	std::this_thread::sleep_for(std::chrono::seconds(1));
	dispatcher.Stop(); // Останавливаем выполнение фоновых задач
	dispatcher.Dispatch([] { osync(std::cout) << "2"; }); // 2 выведена не будет, так как раньше был вызван Stop
	dispatcher.Wait(); // Дожидаемся завершения фоновых задач
	osync(std::cout) << "!"; // Выведет ! в главном потоке.
//  +
}