#pragma once
#include <vector>

using namespace std;

class task;
class employee;
class tasklist;

class ObjMgr
{
public:
	static ObjMgr* instance();
protected:
	static ObjMgr *_instance;
public:
	void loadtask();
	void loademployee();
	void loadtasklists(vector<tasklist*>* task_lists) { _tasklists.assign((*task_lists).begin(),(*task_lists).end()); };
	vector<task*> gettaskset() const {
		return _taskset;
	};
	vector<employee*> getemployeeset() const {
		return _employeeset;
	};
	vector<tasklist*> gettasklists() const { return _tasklists; };
private:
	vector<task*> _taskset;
	vector<employee*> _employeeset;
	vector<tasklist*> _tasklists;
};