#pragma once
#include <vector>
#include <gurobi_c++.h>
#include <algorithm>

class task;
class employee;
class tasklist;
class tasklistemployee;

using namespace std;

class taskassignmodel
{
public:
	taskassignmodel(vector<task*> taskset, vector<employee*> employeeset, vector<tasklist*> task_lists) {
		_taskset = taskset;
		_employeeset = employeeset;
		_task_lists = task_lists;
	};
	static bool	sortcgcolumnbyreducedcost(const tasklistemployee* p1, const tasklistemployee* p2);
	void solve();
	void solveCG();
	void addemployeeconstrs(GRBModel& model);
	void addtaskconstrs(GRBModel& model);
	void addtasklistassigntoemployeevar(GRBModel& model);
	void addemployeeconstrscg(GRBModel& mainmodel);//每个员工最多只能做一个tasklist
	void addtaskconstrscg(GRBModel& mainmodel);//每个任务都需要被满足
	void addtasklistemployeevarcg(GRBModel& mainmodel);//加入tasklist指派给员工的0-1变量
	void buildtasklistemployee(GRBModel& model);

private:
	vector<task*> _taskset;
	vector<employee*> _employeeset;
	vector<tasklist*> _task_lists;
	vector<tasklistemployee*> chosencgcolumn;
	vector<tasklistemployee*> optionalcgcolumn;
};
class tasklistemployee
{
public:
	tasklistemployee(tasklist* taskliste, employee* employeed) {
		_tasklist = taskliste;
		_employee = employeed;
	};
	void calculatecost();
	void calculatereducedcost(GRBModel& mainmodel);
	void setcol(GRBColumn col) {_col = col;};
	tasklist* gettasklist() const { return _tasklist; };
	employee* getemployee() const { return _employee; };
	double getcost() const { return _cost; };
	GRBColumn getcol() const {return _col;};
	void setreducedcost(int reducedcostval) {_reducedcost = reducedcostval; };
	double getreducedcost() const {return _reducedcost;};
	//void settask(task* task) {_task = task;}
	//void setemployee(employee* employee) {_employee = employee;}
private:
	double _cost;
	double _reducedcost;
	GRBColumn _col;
	tasklist* _tasklist;
	employee* _employee;
};