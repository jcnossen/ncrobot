
#include "StdIncl.h"

#include "ThreadManager.h"

ThreadManager::~ThreadManager()
{
	WaitForAllDone();

	if (todoMutex)
		SDL_DestroyMutex(todoMutex);
}

ThreadManager::ThreadManager()
{
	todoMutex = SDL_CreateMutex();
}


void ThreadManager::AddWorkItem( void (*func)(void*state), void* state )
{
	WorkItem w;
	w.func = func;
	w.state = state;

	SDL_LockMutex(todoMutex);
	todo.push_back(w);
	SDL_UnlockMutex(todoMutex);
}

int ThreadManager::ThreadMain(void* data)
{
	State* state = (State*)data;
	ThreadManager* m = state->owner;

	WorkItem item;
	while(true) {
		SDL_LockMutex(m->todoMutex);
		bool allDone = m->todo.empty();
		if (!allDone) {
			item = m->todo.front();
			m->todo.pop_front();
		}
		SDL_UnlockMutex(m->todoMutex);

		if (allDone) {
			// TODO: Wait for signal to continue?
//			SDL_CondWait(state->waitCond, state->waitMutex);
			break;
		}

		// Execute the work item
		item.func(item.state);
	}

	state->finished = true;

	return 0;
}

// Start all the threads. Make sure there is work to do before running this
void ThreadManager::Start(int numThreads)
{
	threads.resize(numThreads);

	for(int i=0;i<numThreads;i++) {
		threads[i].owner = this;
		threads[i].thread = SDL_CreateThread(ThreadMain, &threads[i]);
	}
}

void ThreadManager::Stop()
{
}

bool ThreadManager::AllDone()
{
	for(int i=0;i<threads.size();i++)
		if (!threads[i].finished)
			return false;
	return true;
}

void ThreadManager::WaitForAllDone()
{
	end=true;

	while(!AllDone())
		SDL_Delay(50);
}

