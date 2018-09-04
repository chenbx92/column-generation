#include "taskassignmodel.h"
#include "employee.h"
#include "task.h"
#include "tasklist.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


using namespace std;

void taskassignmodel::solve()
{
	GRBEnv* env = new GRBEnv();
	GRBModel model = GRBModel(*env);
	addemployeeconstrs(model);//每个员工最多只能做一个tasklist
	addtaskconstrs(model);//每个任务都需要被满足
	addtasklistassigntoemployeevar(model);//加入tasklist指派给员工的0-1变量
	model.set(GRB_IntAttr_ModelSense,1);
	model.write("taskassignmodel.lp");
	model.optimize();

	//print result
	if (model.get(GRB_IntAttr_Status) == 2)
	{
		ofstream of("assignresult.output");
		for (size_t i = 0; i < _employeeset.size(); i++)
		{
			employee* employeed = _employeeset[i];
			for (size_t j = 0; j < _task_lists.size(); j++)
			{
				tasklist* taskliste = _task_lists[j];
				vector<task*> tasks = taskliste->gettasksseq();
			    ostringstream ofstring;
			    ofstring<<"tasklist"<<taskliste->gettasklistid();
			    string tasklistidstr=ofstring.str();
			
				if (model.getVarByName(employeed->getemployeename()+tasklistidstr).get(GRB_DoubleAttr_X)>0.0001)
				{                                                                                        
					of << employeed->getemployeename()<< model.getVarByName(employeed->getemployeename() + tasklistidstr).get(GRB_DoubleAttr_X)
						<< "withtasklist" << taskliste->gettasklistid();
					for (size_t k = 0; k < tasks.size(); k++)
					{
						of << tasks[k]->gettaskid();
					}
					of << endl;
				}
			}
		}
		of.close();
	}
	else
		cout << "no optimal solution";
}
void taskassignmodel::addemployeeconstrs(GRBModel& model)
{
	for (size_t i = 0; i < _employeeset.size(); i++)
	{
		employee* employeed = _employeeset[i];
		string constrname = employeed->getemployeename();
		GRBLinExpr expr = 0;
		int rhs = 1;
		std::cout << "add constr: [" <<  constrname <<"]\n";
		model.addConstr(expr, GRB_LESS_EQUAL, rhs, constrname);
	}
	model.update();
}

void taskassignmodel::addtaskconstrs(GRBModel& model)
{
	for (size_t i = 0; i < _taskset.size(); i++)
	{
		task* taske = _taskset[i];
		string constrname = taske->gettaskid();
		GRBLinExpr expr = 0;
		int rhs = taske->getempolyeenoneeded();
		std::cout << "add constr: [" <<  constrname <<"]\n";
		model.addConstr(expr, GRB_EQUAL, rhs, constrname);
		model.update();

		//slack variable
		GRBColumn col;
		col.addTerm(1.0, model.getConstrByName(constrname));
		int cost = 10000;
		string varname = taske->gettaskid() + "slack";
		model.addVar(0, 1.0, cost, GRB_CONTINUOUS, col, varname);
	}
	model.update();
}
void taskassignmodel::addtasklistassigntoemployeevar(GRBModel& model)
{
	for (size_t i = 0; i < _employeeset.size(); i++)
	{
		employee* employeed = _employeeset[i];
		for (size_t j = 0; j < _task_lists.size(); j++)
		{
			tasklist* taskliste= _task_lists[j];
			vector<task*> tasks = taskliste->gettasksseq();
			ostringstream ofstring;
			ofstring<<"tasklist"<<taskliste->gettasklistid();
			string tasklistidstr=ofstring.str();
			GRBColumn col;
			//addterm for employee constrs
			std::string constr_name = employeed->getemployeename();
			GRBConstr constr = model.getConstrByName(constr_name);
			col.addTerm(1.0, constr);
			for (size_t m = 0; m < _taskset.size(); m++)
			{
				task* taske= _taskset[m];
				vector<task*>::const_iterator iter = find(tasks.begin(), tasks.end(), taske);
				if (iter != tasks.end())
				{
					col.addTerm(1.0, model.getConstrByName(taske->gettaskid()));
				}				
			}
			int cost =(employeed->getunittaskcost())* tasks.size();
			if (tasks.size() > 3)
			{
				cost += 1000*(tasks.size()-3);
			}
			model.addVar(0.0, 1.0, cost, GRB_BINARY, col, employeed->getemployeename()+tasklistidstr);
			
		}
	}
	model.update();
}
void taskassignmodel::solveCG()
{
	//mainrelaxproblem
	GRBEnv* env = new GRBEnv();
	GRBModel mainmodel = GRBModel(*env);
	addemployeeconstrscg(mainmodel);//每个员工最多只能做一个tasklist
	addtaskconstrscg(mainmodel);//每个任务都需要被满足
	buildtasklistemployee(mainmodel);

	//选择初始cg变量
	int i = 0;
	vector<task*> coveredtask;
	while (coveredtask.size() < _taskset.size())
	{
		tasklistemployee* temtasklistemployee = optionalcgcolumn[i];
		tasklist* temtasklist = temtasklistemployee->gettasklist();
		vector<task*> taskset = temtasklist->gettasksseq();
		for (size_t j = 0; j < taskset.size(); j++)
		{
			vector<task*>::const_iterator iter= find(coveredtask.begin(), coveredtask.end(), taskset[j]);
			if (iter==coveredtask.end())
			{ 
				coveredtask.push_back(taskset[j]);
				vector<tasklistemployee * >::const_iterator iter2 = find(chosencgcolumn.begin(), chosencgcolumn.end(), temtasklistemployee);
				if (iter2 == chosencgcolumn.end())
				{
					chosencgcolumn.push_back(temtasklistemployee);
				}
			}
		}
		i++;
	}
	addtasklistemployeevarcg(mainmodel);//加入tasklist指派给员工的0-1变量
	mainmodel.set(GRB_IntAttr_ModelSense, 1);
	mainmodel.write("taskassignmodelCGmain.lp");
	mainmodel.optimize();

	if (mainmodel.get(GRB_IntAttr_Status) == 2)	
	{
		for (size_t j = 0; j < optionalcgcolumn.size(); j++)
		{
			vector<tasklistemployee*>::const_iterator iter3 = find(chosencgcolumn.begin(), chosencgcolumn.end(), optionalcgcolumn[j]);
			if (iter3==chosencgcolumn.end())
			{
				optionalcgcolumn[j]->calculatereducedcost(mainmodel);
			}
			else
			{
				optionalcgcolumn[j]->setreducedcost(10000);
			}
		}
	}
	else
	{
		cout << "no optimal solution during adding columns";
	};
	sort(optionalcgcolumn.begin(), optionalcgcolumn.end(), sortcgcolumnbyreducedcost);


	while(optionalcgcolumn[0]->getreducedcost() < 0)//添加column进入主问题，update mainmodel
	{
		employee* employeed = optionalcgcolumn[0]->getemployee();
		tasklist* taskliste = optionalcgcolumn[0]->gettasklist();
		vector<task*> tasks = taskliste->gettasksseq();
		ostringstream ofstring;
		ofstring << "tasklist" << taskliste->gettasklistid();
		string tasklistidstr = ofstring.str();
		mainmodel.addVar(0.0, 1.0, optionalcgcolumn[0]->getcost(), GRB_CONTINUOUS, optionalcgcolumn[0]->getcol(), employeed->getemployeename()  + tasklistidstr);
		mainmodel.update();
		mainmodel.write("taskassignmodelCG.lp");
		mainmodel.optimize();
		chosencgcolumn.push_back(optionalcgcolumn[0]);

		if (mainmodel.get(GRB_IntAttr_Status) == 2)	//列生成循环
												//计算reducedcost
		{
			for (size_t j = 0; j < optionalcgcolumn.size(); j++)
			{
				vector<tasklistemployee*>::const_iterator iter3 = find(chosencgcolumn.begin(), chosencgcolumn.end(), optionalcgcolumn[j]);
				if (iter3 == chosencgcolumn.end())
				{
					optionalcgcolumn[j]->calculatereducedcost(mainmodel);
				}
				else
				{
					optionalcgcolumn[j]->setreducedcost(10000);
				}
			}
		}
		else
		{
			cout << "no optimal solution during adding columns";
		};
		sort(optionalcgcolumn.begin(), optionalcgcolumn.end(), sortcgcolumnbyreducedcost);
	}

		//IP
	for (size_t i = 0; i < chosencgcolumn.size(); i++)
	{
		employee* employeed = chosencgcolumn[i]->getemployee();
		tasklist* taskliste = chosencgcolumn[i]->gettasklist();
		vector<task*> tasks = taskliste->gettasksseq();
		ostringstream ofstring;
		ofstring << "tasklist" << taskliste->gettasklistid();
		string tasklistidstr = ofstring.str();
		GRBVar var=mainmodel.getVarByName(employeed->getemployeename() + tasklistidstr);
		var.set(GRB_CharAttr_VType,'B');
	}

	mainmodel.update();
	mainmodel.write("taskassignmodelCGi.lp");
	mainmodel.optimize();
			
	//print result
	if (mainmodel.get(GRB_IntAttr_Status) == 2)
	{
		ofstream of("assignresultcg.output");
		for (size_t i = 0; i < chosencgcolumn.size(); i++)
		{
			employee* employeed = chosencgcolumn[i]->getemployee();
			tasklist* taskliste = chosencgcolumn[i]->gettasklist();
			vector<task*> tasks = taskliste->gettasksseq();
			ostringstream ofstring;
			ofstring << "tasklist" << taskliste->gettasklistid();
			string tasklistidstr = ofstring.str();

			if (mainmodel.getVarByName(employeed->getemployeename() + tasklistidstr).get(GRB_DoubleAttr_X)>0.0001)
			{
				of << employeed->getemployeename() <<mainmodel.getVarByName(employeed->getemployeename() + tasklistidstr).get(GRB_DoubleAttr_X)
					<< "withtasklist" << taskliste->gettasklistid();
				for (size_t k = 0; k < tasks.size(); k++)
				{
					of << tasks[k]->gettaskid();
				}
				of << endl;
			}
		
		}
		of.close();
	}
	else
	{cout << "no optimal solution";}
}
void taskassignmodel::buildtasklistemployee(GRBModel& mainmodel)
{
	for (size_t i = 0; i < _employeeset.size(); i++)
	{
		employee* employeed = _employeeset[i];
		for (size_t j = 0; j < _task_lists.size(); j++)
		{
			tasklist* taskliste = _task_lists[j];
			tasklistemployee* newtasklistemployee = new tasklistemployee(taskliste,employeed);
			newtasklistemployee->calculatecost();
			vector<task*> tasks = taskliste->gettasksseq();
			GRBColumn col;
			//addterm for employee constrs
			std::string constr_name = employeed->getemployeename();
			GRBConstr constr = mainmodel.getConstrByName(constr_name);
			col.addTerm(1.0, constr);
			for (size_t m = 0; m < _taskset.size(); m++)
			{
				task* taske = _taskset[m];
				vector<task*>::const_iterator iter = find(tasks.begin(), tasks.end(), taske);
				if (iter != tasks.end())
				{
					col.addTerm(1.0, mainmodel.getConstrByName(taske->gettaskid()));
				}
			}
			newtasklistemployee->setcol(col);
			optionalcgcolumn.push_back(newtasklistemployee);
		}
	}
}
void taskassignmodel::addemployeeconstrscg(GRBModel& mainmodel)
{
	for (size_t i = 0; i < _employeeset.size(); i++)
	{
		employee* employeed = _employeeset[i];
		string constrname = employeed->getemployeename();
		GRBLinExpr expr = 0;
		int rhs = 1;
		std::cout << "add constr: [" << constrname << "]\n";
		mainmodel.addConstr(expr, GRB_LESS_EQUAL, rhs, constrname);
	}
	mainmodel.update();
}
void taskassignmodel::addtaskconstrscg(GRBModel& mainmodel)
{
	for (size_t i = 0; i < _taskset.size(); i++)
	{
		task* taske = _taskset[i];
		string constrname = taske->gettaskid();
		GRBLinExpr expr = 0;
		int rhs = taske->getempolyeenoneeded();
		std::cout << "add constr: [" << constrname << "]\n";
		mainmodel.addConstr(expr, GRB_EQUAL, rhs, constrname);
		mainmodel.update();

		//slack variable
		GRBColumn col;
		col.addTerm(1.0, mainmodel.getConstrByName(constrname));
		int cost = 10000;
		string varname = taske->gettaskid() + "slack";
		mainmodel.addVar(0, 1.0, cost, GRB_CONTINUOUS, col, varname);
	}
	mainmodel.update();
}
void taskassignmodel::addtasklistemployeevarcg(GRBModel& mainmodel)
{
	for (size_t i = 0; i < chosencgcolumn.size(); i++)
	{
		employee* employeed = chosencgcolumn[i]->getemployee();
		tasklist* taskliste = chosencgcolumn[i]->gettasklist();
		vector<task*> tasks = taskliste->gettasksseq();
		ostringstream ofstring;
		ofstring << "tasklist" << taskliste->gettasklistid();
		string tasklistidstr = ofstring.str();
		double cost = chosencgcolumn[i]->getcost();
		mainmodel.addVar(0.0, 1.0, cost, GRB_CONTINUOUS, chosencgcolumn[i]->getcol(),employeed->getemployeename() + tasklistidstr);
	}
	mainmodel.update();
}
bool taskassignmodel::sortcgcolumnbyreducedcost(const tasklistemployee* p1, const tasklistemployee* p2)
{
	return p1->getreducedcost() < p2->getreducedcost();
}
