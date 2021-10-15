#pragma once

#ifndef _TIMER_H
#define _TIMER_H

#define TIME_SCOPE(x) Timer timer(x)
#define TIME() TIME_SCOPE(__FUNCSIG__)

#include <chrono>
#include <iostream>

class Timer
{
public:
	Timer(const char* _func_sig)
	{
		m_func_sig = _func_sig;
		m_s_tp = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		Stop();
	}

private:

	void Stop()
	{
		m_e_tp = std::chrono::high_resolution_clock::now();
		auto dur = m_e_tp - m_s_tp;
		std::cout << m_func_sig << ":\n";
		std::cout << "\t" << dur.count() << " nano sec's" << '\n';
		std::cout << "\t" << std::chrono::duration_cast<std::chrono::microseconds>(dur).count() << " micro sec's" << '\n';
		std::cout << "\t" << std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() << " mili sec's" << '\n';
		std::cout << "\t" << std::chrono::duration_cast<std::chrono::seconds>(dur).count() << " sec's" << '\n';
	}

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> m_s_tp;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_e_tp;
	std::string m_func_sig;
};



#endif // !_TIMER_H
