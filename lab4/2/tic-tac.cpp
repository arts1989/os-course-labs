#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stop_token>
#include <string>
#include <syncstream>
#include <thread>

using namespace std::chrono_literals;
using namespace std::literals;
using Clock = std::chrono::system_clock;

std::string CurrentTimeToString()
{
	auto t = Clock::to_time_t(Clock::now());
	struct tm buf;
	localtime_s(&buf, &t);
	std::ostringstream os;
	os << std::setw(2) << std::setfill('0') << buf.tm_hour << ":" << buf.tm_min << ":" << buf.tm_sec;

	return std::move(os).str();
}

int main()
{
	std::stop_source stopSource;
	std::stop_token stopToken = stopSource.get_token();

	std::jthread ticThr{ [](std::stop_token stopToken) {
		while (!stopToken.stop_requested())
		{
			std::osyncstream(std::cout) << CurrentTimeToString() << " Tic" << std::endl;
			std::this_thread::sleep_for(2000ms);
		}

		std::osyncstream(std::cout) << "Tic stopped" << std::endl;
	} };

	std::jthread tacThr{ [](std::stop_token stopToken) {
		while (!stopToken.stop_requested())
		{
			std::osyncstream(std::cout) << CurrentTimeToString() << " Tac" << std::endl;
			std::this_thread::sleep_for(3000ms);
		}

		std::osyncstream(std::cout) << "Tac stopped" << std::endl;
	} };

	std::string command;
	std::cin >> command;

	while (std::cin.fail())
	{
		stopSource.request_stop();
	}
}