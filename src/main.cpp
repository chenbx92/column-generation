#include "ObjMgr.h"
#include "tasklist.h"
#include "taskassignmodel.h"
#include <iostream>

int main(int argc, char *argv[])
{ 
 try{
	ObjMgr::instance()->loadtask();
	ObjMgr::instance()->loademployee();
	tasklistbuilder *buildtasklist = new tasklistbuilder();
	buildtasklist->exec();
	//delete buildtasklist;测试一下，应该没有问题
	taskassignmodel assignmodel(ObjMgr::instance()->gettaskset(), ObjMgr::instance()->getemployeeset(), ObjMgr::instance()->gettasklists());
	int exemode;//1ΪIP,solve(),2ΪCG,solveCG()
	std::cin>>exemode;
	if (exemode == 1)
	{
		assignmodel.solve();
	}
	if (exemode == 2)
	{
		assignmodel.solveCG();
	}	
  }catch (GRBException& e){std::cout << e.getMessage() << "\n";}
 return 0;
}