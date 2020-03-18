//Alice Giandjian, CS302 - 1004, Assignment #11
/*
* DESCRIPTION: Design a set of C++11 classes to read, store, and process a large directed acyclic graph3 representing a Gantt Chart; 
tasks (edges) and milestones (vertices's).
* INPUT: The input files will be task lists. The first three lines will contain a project title, milestone count, and source node (in that order).
* OUTPUT: Graph Information, Dependency Statistics (In-Degree/Out-Degree), Topological Sort, Articulation Points, Critical Path, Slack Time,
and the optional printing of the graph. 
*/

#ifndef ganttUtilsH
#define ganttUtilsH

#include <string>

using namespace std;

struct listNode {

    int dest;
    int weight;
    listNode * next;

};

struct listHead {

    listNode * front;

};


class ganttUtils {

    public:
        ganttUtils(); 
        ~ganttUtils(); 
        bool getArguments(int, char * [], string&, bool&); 
        bool readGraph(const string);
        bool isValidProject(); 
        void findGraphInformation(); 
        void findKeyMilestone();
        void findNodePoint();
        void findDependencyStats();
        void findIndepedentMilestones();
        void findSlackTimes();
        void findAPs();
        void topoSort();
        void criticalPath();
        void findSlackTime();
        void addEdge(int, int,int);
        void printGraphInformation();
        bool printGraph();
        void printDependencyStats();
        void printTopoSort();
        void printAPs();
        void printCriticalPath();
        void printSlackTimes();
        int getTaskCount();

    private:
        int vertexCount; //milestones
        int edgeCount; //tasks
        int sourceMilestone; //first node
        double ratio; //task/milestone ratio
        double density; //to calculate density of project
        int * inDegree; //to calculate inDegrees
        int * outDegree; //to calculate outDegrees
        bool * aps; //to find APs
        int nodePoint; //to find nodePoints
        int nodePointDegree; //hold degrees for nodePoints
        int keyMilestone; 
        int keyMilestoneDegree;
        int * indMilestones;
        int * distances;
        int duration;
        int * crPath;
        int crPathSize;
        int indSize;
        string title; //graph title
        int topoCount; //count of topo nodes
        int * topoNodes; //array for topo nodes
        listHead * adjList; //adjacency list
        int maxIn;
        int inMaxCount;
        int minIn;
        int inMinCount;
        int maxOut;
        int outMaxCount;
        int minOut;
        int outMinCount;
        int * slackTimes;
        int slackTimeCount;
        bool isCycle(int, bool[], bool[]); //to check if graph goes in cycle
        void findAPsHelper(int, bool[], int[], int[], int[], bool[]); //helper recurssive function

};

#endif