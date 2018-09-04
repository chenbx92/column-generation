#pragma once
#include <vector>

class tasklist;
class task;

using namespace std;
class tasklistbuilder
{
public:
	void exec();
	void tasklistdepsearch(tasklist* temtasklist,task* taske, vector<task*> taskset, vector<tasklist*>* task_lists);
};

class tasklist
{
public:
	void addtask(task*taske) { _tasklist.push_back(taske); };
	void poptask() { _tasklist.pop_back(); };
	int gettasklistsize() const { return _tasklist.size(); };
	vector<task*> gettasksseq()const { return _tasklist; };
	int gettasklistid() { return tasklistid; };
	void settasklistid(int id) { tasklistid = id; };

private:
	vector<task*> _tasklist;
	int tasklistid;
};