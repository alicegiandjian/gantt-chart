//  CS 302 - Assignment #11
//Alice Giandjian, CS302 - 1004, Assignment #11
/*
* DESCRIPTION: Design a set of C++11 classes to read, store, and process a large directed acyclic graph3 representing a Gantt Chart; 
tasks (edges) and milestones (vertices's).
* INPUT: The input files will be task lists. The first three lines will contain a project title, milestone count, and source node (in that order).
* OUTPUT: Graph Information, Dependency Statistics (In-Degree/Out-Degree), Topological Sort, Articulation Points, Critical Path, Slack Time,
and the optional printing of the graph. 
*/
//  Find project gantt information.

#include <cstdlib>
#include <iostream>

using namespace std;

#include "ganttUtils.h"

int main(int argc, char *argv[])
{

// *****************************************************************
//  Headers...

	const int	LIMIT = 10;
	const char	*bold = "\033[1m";
	const char	*unbold = "\033[0m";
	string		stars = "";
	string		tasksFile = "";
	string		dotFile = "";
	bool		printFlag = false;
	ganttUtils	myProject;

	stars.append(65, '*');

// ------------------------------------------------------------------
//  Perform initial checks.
//	command line arguments
//	read project tasks file
//	basic validation of tasks file

	if (!myProject.getArguments(argc, argv, tasksFile, printFlag))
		exit(1);

	if (!myProject.readGraph(tasksFile)) {
		cout << "Error, can not find project file." << endl;
		exit(1);
	}

	if(!myProject.isValidProject()) {
		cout << "Invalid task list." << endl;
		exit(1);
	}

// ------------------------------------------------------------------
//  Display headers, verify enough tasks...

	cout << stars << endl << bold << "CS 302 - Assignment #11" << endl;
	cout << "Gantt Analysis Project" << unbold << endl;
	cout << endl;

	if (myProject.getTaskCount() < LIMIT) {
		cout << "Error, too few tasks." << endl;
		exit(1);
	}

// ------------------------------------------------------------------
//  Find project information
//	Note, some of these function can be threaded...

// ----------
//  To initiate a thread for an object function:
//	thread thd1(&<objectName>::<functionName>);
//  Note, as usual, must join to ensure completed:
//	thd1.join()

	myProject.findGraphInformation();
	myProject.findKeyMilestone();
	myProject.findNodePoint();
	myProject.findIndepedentMilestones();
	myProject.findAPs();
	myProject.topoSort();
	myProject.criticalPath();
	myProject.findSlackTimes();
	myProject.findDependencyStats();

// -----
//  Display project information.
//	Note, all calculations done, just display results.

	myProject.printGraphInformation();

	if (printFlag)
		myProject.printGraph();

	myProject.printDependencyStats();
	myProject.printTopoSort();
	myProject.printAPs();
	myProject.printCriticalPath();
	myProject.printSlackTimes();

// *****************************************************************
//  All done.

	cout << stars << endl;
	cout << "Game Over, thank you for playing." << endl;

	return 0;
}

