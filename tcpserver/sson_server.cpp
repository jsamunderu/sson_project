#include "tcpserver.h"

#include <memory>

#ifdef __linux__ 
#include <csignal>
#elif _WIN32
#include <windows.h>
#include "stdafx.h"
#endif

std::unique_ptr<sson::tcpserver> server;

#ifdef __linux__ 
void handler_routine(int signum)
{
	if (server != nullptr)
	{
		server->stop();
	}
}
void wait_for_termination()
{
	struct sigaction action;
	action.sa_handler = handler_routine;
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
}
#elif _WIN32
BOOL WINAPI handler_routine(_In_ DWORD dw_ctrl_type)
{
	switch (dw_ctrl_type)
	{
	case CTRL_C_EVENT:
		if (server != nullptr)
		{
			server->stop();
		}
		return TRUE;
	default:
		return FALSE;
	}
}
void wait_for_termination()
{
	SetConsoleCtrlHandler(handler_routine, TRUE);
}
#endif

int main()
{
	wait_for_termination();
	server.reset(new sson::tcpserver(8181, 2));
		// = std::make_unique<sson::tcpserver>(8181, 2);
	server->start();
	return EXIT_SUCCESS;
}
