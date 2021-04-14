//==============
// TaskHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "Handle.h"
#include "TaskClass.h"


//========
// Common
//========

template <class _owner_t, class... _args_t> Handle<Task> CreateTask(_owner_t* Owner, VOID (_owner_t::*Procedure)(), _args_t... Arguments)
{
auto task=new TaskTyped<_owner_t>(Owner, Procedure);
task->Run(Arguments...);
return task;
}
