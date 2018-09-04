#include "taskassignmodel.h"
#include "tasklist.h"
#include "employee.h"
#include "task.h"


void tasklistemployee::calculatecost()
{
	int coste = this->_employee->getunittaskcost()* this->_tasklist->gettasklistsize();
	if (this->_tasklist->gettasklistsize() > 3)
	{
		coste += 1000 * (this->_tasklist->gettasklistsize() - 3);
	}
	this->_cost = coste;
}

void tasklistemployee::calculatereducedcost(GRBModel& mainmodel)
{
	double dualcost = 0;
	GRBColumn Col = this->_col;
	for (int i=0;i<Col.size();i++)
	{
		GRBConstr cons=Col.getConstr(i);
		double dualval = cons.get(GRB_DoubleAttr_Pi);
		double coef = Col.getCoeff(i);
		dualcost += dualval * coef;
	}
	this->_reducedcost = this->_cost - dualcost;
}