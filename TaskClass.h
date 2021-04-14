//=============
// TaskClass.h
//=============

#pragma once


//=======
// Using
//=======

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <functional>


//======
// Task
//======

class Task: public Object
{
public:
	// Common
	volatile BOOL Cancel;
	VOID Wait();

protected:
	// Con-/Destructors
	Task();

	// Common
	Handle<Task> hThis;
};


//============
// Task Typed
//============

template <class _owner_t, class... _args_t>
class TaskTyped: public Task
{
public:
	// Task-Proc
	typedef VOID (_owner_t::*TaskProc)(_args_t...);

	// Con-/Destructors
	TaskTyped(_owner_t* Owner, TaskProc Procedure, UINT Stack=2048, UINT Priority=5):
		hOwner(Owner),
		pProcedure(Procedure),
		uPriority(Priority),
		uStack(Stack)
		{}

	VOID Run(_args_t... Arguments)
		{
		if(hThis)
			return;
		hThis=this;
		cProcedure=[this, Arguments...](){ (hOwner->*pProcedure)(Arguments...); };
		xTaskCreate(DoTask, nullptr, uStack, this, uPriority, nullptr);
		}

private:
	// Common
	static VOID DoTask(VOID* Param)
		{
		Handle<TaskTyped> task=(TaskTyped*)Param;
		task->cProcedure();
		task->hThis=nullptr;
		}
	std::function<VOID()> cProcedure;
	Handle<_owner_t> hOwner;
	TaskProc pProcedure;
	UINT uPriority;
	UINT uStack;
};
