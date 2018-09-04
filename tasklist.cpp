#include "tasklist.h"
#include <vector>
#include "ObjMgr.h"
#include "task.h"
#include <fstream>
#include <sstream>
#include "taskassignmodel.h"


class task;
class employee;

using namespace std;

void tasklistbuilder::exec()
{
	vector<task*> taskset = ObjMgr::instance()->gettaskset();
	vector<employee*> employeeset = ObjMgr::instance()->getemployeeset();
	vector<tasklist*>* task_lists=new vector<tasklist*>();
	for (int i = 0; i < taskset.size(); i++)
	{
		task* taske = taskset[i];
		tasklist* temtasklist = new tasklist();
		tasklistdepsearch(temtasklist,taske, taskset, task_lists);
		delete temtasklist;
	}
	//dump task_lists
	{
		ofstream of("tasklists.dump");
		for (vector<tasklist*>::const_iterator iter = (*task_lists).begin(); iter != (*task_lists).end(); iter++)
		{
			tasklist* taskliste = *iter;
			vector<task*> tasks = taskliste->gettasksseq();
			of << taskliste->gettasklistid()<<" ";
			for (size_t i=0;i<tasks.size();i++)
				{
					of<<tasks[i]->gettaskid()<<" ";
				}
			of << endl;
		}
		of.close();
	}
	ObjMgr::instance()->loadtasklists(task_lists);
	delete task_lists;
}
void tasklistbuilder::tasklistdepsearch(tasklist* temtasklist,task* taske, vector<task*> taskset, vector<tasklist*>* task_lists)
{
	temtasklist->addtask(taske);
	if (temtasklist->gettasklistsize() > 5)
	{
		temtasklist->poptask();
		return;
	}
	if (temtasklist->gettasklistsize() > 1)
	{
		tasklist* tasklistsaved = new tasklist(*temtasklist);
		int id = task_lists->size();
		tasklistsaved->settasklistid(id);
		task_lists->push_back(tasklistsaved);
	}
	vector<task*> temtasklisttasks = temtasklist->gettasksseq();
	task* lasttask =temtasklisttasks[temtasklist->gettasklistsize() - 1];
	for (vector<task*> ::const_iterator iter = taskset.begin(); iter != taskset.end(); iter++)
	{
		task* tasknew = *iter;
		if (lasttask->gettasksequnece() < tasknew->gettasksequnece())
		{
			tasklistdepsearch(temtasklist, tasknew, taskset, task_lists);
		}
	}
	temtasklist->poptask();
}