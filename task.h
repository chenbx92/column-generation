#pragma once
#include <string>

using namespace std;

class task
{
private:
	string _taskid;
	int _employeeNoNeeded;
	int _sequence;
public:
	void settaskid(string taskid) { _taskid = taskid; };
	void setemployeenoneeded(int employeenoneeded) { _employeeNoNeeded = employeenoneeded; };
	void setsequence(int sequence) { _sequence = sequence; };
	int gettasksequnece() {return _sequence;}
	string gettaskid() { return _taskid; };
	int getempolyeenoneeded() {return _employeeNoNeeded;}
};