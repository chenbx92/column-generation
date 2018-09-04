#include "ObjMgr.h"
#include "fstream"
#include "sstream"
#include "iostream"
#include "task.h"
#include "employee.h"
using namespace std;

ObjMgr* ObjMgr::_instance = 0;
ObjMgr* ObjMgr::instance()
{
	if (!_instance)
		_instance = new ObjMgr();
	return _instance;
}
void ObjMgr::loademployee()
{
	string aline;
	ifstream aStream("employee.input");
	if (!aStream)
	{
		std::cerr<< "Error: could noet open file employee.input";
		throw(-1);
	}
	while(aStream.good())
	{
		string employeename;
		int unittaskcost;
		getline(aStream, aline);
		if (aline.empty())
		{
			continue;
		}
		istringstream pausealine(aline);
		pausealine >> employeename;
		pausealine >> unittaskcost;
		employee* tememployee = new employee();
		tememployee->setemployeename(employeename);
		tememployee->setunittaskcost(unittaskcost);
		_employeeset.push_back(tememployee);
	}
	aStream.close();
}
void ObjMgr::loadtask()
{
	string aline;
	ifstream aStream("task.input");
	if (!aStream)
	{
		std::cerr << "Error: could noet open file employee.input";
		throw(-1);
	}
	while (aStream.good())
	{
		string taskid;
		int employeenoneeded;
		int sequence;
		getline(aStream, aline);
		if (aline.empty())
		{
			continue;
		}
		istringstream pausealine(aline);
		pausealine >> taskid;
		pausealine >> employeenoneeded;
		pausealine >> sequence;
		task* taske = new task();
		taske->settaskid(taskid);
		taske->setemployeenoneeded(employeenoneeded);
		taske->setsequence(sequence);
		_taskset.push_back(taske);
	}
	aStream.close();
}
