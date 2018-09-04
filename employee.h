#pragma once
#include <string>

using namespace std;

class employee
{
private:
	string _employeename;
	int _unittaskcost;
public:
	void setemployeename(string name) { _employeename = name; };
	void setunittaskcost(int cost) { _unittaskcost = cost; };
	string getemployeename() { return _employeename;};
	int getunittaskcost(){return _unittaskcost;};
};
