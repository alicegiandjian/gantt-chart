//Alice Giandjian, CS302 - 1004, Assignment #11
/*
* DESCRIPTION: Design a set of C++11 classes to read, store, and process a large directed acyclic graph3 representing a Gantt Chart; 
tasks (edges) and milestones (vertices's).
* INPUT: The input files will be task lists. The first three lines will contain a project title, milestone count, and source node (in that order).
* OUTPUT: Graph Information, Dependency Statistics (In-Degree/Out-Degree), Topological Sort, Articulation Points, Critical Path, Slack Time,
and the optional printing of the graph. 
*/

#include "ganttUtils.h"
#include "linkedQueue.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

/*
* FUNCTION: ganttUtils
* DESCRIPTION: Sets correct values 
* PASSED: N/A
* OUT: N/A
*/
ganttUtils::ganttUtils()
{

    topoNodes = NULL; //topoNodes array
    inDegree = NULL; //inDegree array
    outDegree = NULL; 

    topoCount = -1; 
    vertexCount = 0; //milestones
    edgeCount = 0; //tasks


    title = "";

}

/*
* FUNCTION: ~ganttUtils
* DESCRIPTION: Deletes any dynamically allocated memory
* PASSED: N/A
* OUT: N/A
*/
ganttUtils::~ganttUtils()  
{

    if(topoNodes != NULL){

        delete[] topoNodes;

    }

    if(indMilestones != NULL){

        delete[] indMilestones;

    }

    if(outDegree != NULL){


        delete[] outDegree;

    }

    if(inDegree != NULL){

        delete[] inDegree;

    }

    if(distances != NULL){

        delete[] distances;

    }

    if(aps != NULL){

        delete[] aps;

    }

    if(slackTimes != NULL){

        delete[] slackTimes;

    }

    if(crPath != NULL){

        delete[] crPath;

    }

    if(adjList != NULL){

        for(int i = 0; i < vertexCount; i++){

            listNode * next = adjList[i].front;
            listNode * temp;

            while(next != NULL){

                temp = next;
                next = temp -> next;
                delete temp;

            }

        }

        delete[] adjList;

    }

    
}

/*
* FUNCTION: getArguments
* DESCRIPTION: Accepts and validates the command line arguments
* PASSED: number of arguments, value of arguments, name of file, toPrint
* OUT: Command line errors
*/
bool ganttUtils::getArguments(int argc, char * argv[], string& fileName, bool& printOut)  
{

    stringstream in;

    bool fileFlag = false;
    bool printFlag = false;

    //if user inputs: ./projectInfo
    if(argc == 1){

        cout << "Usage: ./projectInfo -f <graph file> [-p]" << endl;
        return false;

    }

    //if user does not enter correct amount of args
    if(argc != 3 && argc != 4){

        cout << "Error, invalid command line options." << endl;
        return false;

    }

    
    for(int i = 1; i < argc; i++){

        if(string(argv[i]) == "-f"){

            if(!fileFlag){

                //in << string(argv[i+1]);
                //in >> fileName;
                fileName = string(argv[i+1]);
                ifstream test(fileName);
                //cout << "ping: " << fileName << endl;

                if(test.fail()){

                    cout << "Error, can not find project file." << endl;
                    return false;

                }

                fileFlag = true;
                in.clear();

            }

        }else if(argc == 4){ //check for the correct print specifier 

            string param = string(argv[3]);

            if(param.substr(0,2) == "-p" || param.substr(0,1) == "p" || param.substr(0,3) == "-pr"){

                if(param.length() != 2){

                    cout << "Error, invalid print specifier." << endl;
                    return false;

                }

                if(!printFlag){

                    printOut = true;
                    printFlag = true;

                }

            }

        }

    }

    if(!fileFlag){

        cout << "Error, invalid project file name specifier." << endl;
        return false;

    }

    return true;
    
}


/*
* FUNCTION: readGraph
* DESCRIPTION: Reads in the information from the inputFile 
* PASSED: inputFile
* OUT: N/A
*/
bool ganttUtils::readGraph(const string inputFile)  
{

    ifstream in(inputFile);

    //Check if file input works

    if(in.fail()){

        cout << "readGraph: the file failed to read in. (big sad)" << endl;
        return false;

    }

    string fileTitle = ""; //the file title
    string milestones = ""; //the milestone count
    string milestoneSource = ""; //told source
    int sourceVertex = 0; //first num
    int destVertex = 0; //second num
    int weightVertex = 0; //third num

    getline(in, fileTitle); //store

    if(fileTitle.substr(0,6) == "title:"){

        fileTitle.erase(0,6);

    }

    title = fileTitle; //store in title(global)
    //cout << title << endl; //for debugging

    getline(in, milestones,' ');
    getline(in,milestones);
    vertexCount = stoi(milestones);
    //cout << endl << "vertexCount: " << vertexCount << endl;

    //Create new graph
    adjList = new listHead[vertexCount];

    for(int i = 0; i < vertexCount; i++){

        adjList[i].front = NULL;

    }

    getline(in, milestoneSource, ' ');
    getline(in, milestoneSource);
    sourceMilestone = stoi(milestoneSource);
    //cout << "sourceMilestone: " << sourceMilestone << endl;

    while(!in.fail()){

        in >> sourceVertex;
        //cout << "sourceVertex: " << sourceVertex << endl;
        in >> destVertex;
        //cout << "destVertex: " << destVertex << endl;
        in >> weightVertex;
        //cout << "weightVertex: " << weightVertex << endl;

        if(in.eof()){

            break;

        }

        addEdge(sourceVertex, destVertex, weightVertex);

    }

    return true;
}

/*
* FUNCTION: isValidProject
* DESCRIPTION: Checks to see if the graph is a DAG (A Depth First Search traversal can be used to detect 
a cycle in a graph).  
* PASSED: N/A
* OUT: N/A
*/
bool ganttUtils::isValidProject() 
{

    //Allocate memory for visited and marked
    bool * visited = new bool [vertexCount];
    bool * marked = new bool [vertexCount];

    //Set everything to false
    for(int i = 0; i < vertexCount; i++){

        visited[i] = false;
        marked[i] = false;

    }

    //Check if it cycles
    for(int i = 0; i < vertexCount; i++){

        if(!visited[i]){ //if it is not visited check if it cycles

            if(isCycle(i, visited, marked)){

                return false; //everything sucks its not...valid

            }

        }

    }

    //delete dynamically allocated memory
    delete[] visited;
    delete[] marked;

    return true; //everythings fine ITS VALID

} 

/*
* FUNCTION: isCycle
* DESCRIPTION: Should check to see if there is a cycle in the graph, used in isValidProject. If there 
is a cycle in the graph it means the project is invalid and should return the error from the main. 
* PASSED: vertex, if the vertex was visited, if the vertex was marked
* OUT: N/A
*/
bool ganttUtils::isCycle(int vertex, bool visited[], bool marked[])  
{

    if(!visited[vertex]){ //if the vertex is not visited yet

        //set it to visited and marked
        visited[vertex] = true;
        marked[vertex] = true;

        listNode * ptr = adjList[vertex].front;

        while(ptr != NULL){

            if(marked[ptr -> dest]){ //check if the destination is marked

                return true; //if that destination is marked it means that it has cycled

            }else if(isCycle(ptr -> dest, visited, marked)){ //continues down these edges 

                return true; 

            }

            ptr = ptr -> next; //itterate thru

        }

    }

    marked[vertex] = false;
    return false; //has not cycled

}

/*
* FUNCTION: findGraphInformation
* DESCRIPTION: finds task/milestone & project tasks density & 
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::findGraphInformation()  
{

    ratio = double(edgeCount)/double(vertexCount); //finds task/milestone ratio

    density = (2.0 * double(edgeCount)/(double(vertexCount) * (double(vertexCount) - 1.0))); //finds project task density

    
}

/*
* FUNCTION: findKeyMilestone
* DESCRIPTION: finds the key milestone, which is the latest milestone with the maximum in-degree.
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::findKeyMilestone() 
{

    listNode * ptr;
    int latestInDegree = 0;

    inDegree = new int[vertexCount]; //allocate dynamic memory

    //Initializes inDegree array
    for(int i = 0; i < vertexCount; i++){

        inDegree[i] = 0;

    }

    //Populating the inDegree array while looking for the key milestone
    for(int i = 0; i < vertexCount; i++){

        ptr = adjList[i].front;

        while(ptr != NULL){

            inDegree[ptr -> dest] += 1; //for each edge adds inDegree for each relvant vertex

            //if next vertex has more inDegrees replace as new key milestone
            if(inDegree[ptr -> dest] > inDegree[latestInDegree]){ 

                latestInDegree = ptr -> dest;

            }

            ptr = ptr -> next; //check next vertex

        }

    }

    keyMilestone = latestInDegree;
    keyMilestoneDegree = inDegree[keyMilestone];
    
}


/*
* FUNCTION: findNodePoint
* DESCRIPTION: finds the node point, which is the first milestone with the maximum out-degree.
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::findNodePoint() 
{

    int latestOutDegree = 0;
    listNode * ptr;
    outDegree = new int[vertexCount]; //Allocate dynamic memory 

    
    for(int i = 0; i < vertexCount; i++){

        //Initialize outDegree 
        ptr = adjList[i].front;
        outDegree[i] = 0;

        while( ptr != NULL){

            outDegree[i] += 1; //adds all the outDegrees

            //if next vertex has more outDegrees replace as new nodePoint
            if(outDegree[i] > outDegree[latestOutDegree]){

                latestOutDegree = i;

            }

            ptr = ptr -> next; //check next vertex

        }

    }

    nodePoint = latestOutDegree;
    nodePointDegree = outDegree[nodePoint];

    
}

/*
* FUNCTION: findDependencyStats
* DESCRIPTION:The dependency statistics for the in-degrees include value of the highest in-degree, 
value of the lowest in-degree (excluding the source node) and the count of vertices with the highest 
in-degree and lowest in-degree. The dependency statistics for the out-degrees include value of the highest 
out-degree (excluding the final node), value of the lowest out-degree and the count of vertices with the highest 
in- degree and lowest in-degree.
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::findDependencyStats() 
{

    maxIn = -1; //current most indegrees
    inMaxCount = 0; 

    minIn = vertexCount; //current least indegrees
    inMinCount = 0;

    maxOut = -1; //current most outdegrees
    outMaxCount = 0;

    minOut = vertexCount; //current least outdegrees
    outMinCount = 0;

    int in;
    int out; 

    for(int i = 0; i < vertexCount; i++){ 

        in = inDegree[i]; //current nodes inDegrees
        out = outDegree[i]; //current nodes outDegrees

        //keeps track of how many vertexs have the most inDegrees
        if(in > maxIn){ //found new most indegrees

            maxIn = in; //replaces old new most indegree with new new most indegree 
            inMaxCount = 1;

        } else if (in == maxIn){ //found similar to most indegrees 

            inMaxCount += 1;

        }


        if(i != sourceMilestone){ //special case to ignore the source node
            
            //keeps track of how many vertex have the least amount of inDegrees
            if(in < minIn){ //found new least indegrees

                minIn = in; //replaces old new least indegree with new new least indegree
                inMinCount = 1;

            } else if(in == minIn) { //found similar to least indegrees

                inMinCount += 1;

            }

        }

        //keeps track of how many vertexs have the most amount of outDegrees
        if(out > maxOut){ //found new max outDegree

            maxOut = out; //replaces old new max outdegree with new new max outdegree
            outMaxCount = 1;

        } else if (out == maxOut){ //found similar to max indegree

            outMaxCount += 1;

        }

        if(i != crPath[0]){ //special case to ignore final node

            //keeps track of how many vertexs have the least amount of outDegrees
            if(out < minOut){ //found new min out degree

                minOut = out; //replcaes old new min out degree with new new min outdegree
                outMinCount = 1;

            } else if(out == minOut) { //found similar min outdegree

                outMinCount += 1;

            }

        }

    }

    
}

/*
* FUNCTION: printDependencyStats
* DESCRIPTION: This function prints all of the information found from the findDependencyStats function. 
* PASSED: N/A
* OUT: highest inDegree/outDegree, lowest inDegree/outDegree, count of highest inDegree/outDegree, count of lowest inDegree/outDegree
*/
void ganttUtils::printDependencyStats() 
{

    cout << "------------------------------------------------------------" << endl;
    cout << "Dependency Statistics (in-degree):" << endl;
    cout << "   Highest In-Degree: " << maxIn << endl;
    cout << "   Lowest In-Degree: " << minIn << endl;
    cout << "   Count of Highest Degree: " << inMaxCount << endl;
    cout << "   Count of Lowest Degree: " << inMinCount << endl;
    cout << endl;
    cout << "Dependency Statistics (out-degree):" << endl;
    cout << "   Highest Out-Degree: " << maxOut << endl;
    cout << "   Lowest Out-Degree: " << minOut <<  endl;
    cout << "   Count of Highest Degree: " << outMaxCount << endl;
    cout << "   Count of Lowest Degree: " << outMinCount << endl;
    cout << endl;
}

/*
* FUNCTION: findIndependentMilestones
* DESCRIPTION: Independent milestones are all the milestones with an out-degree of 0. With an out-degree of 0, 
the final completion date is not directly dependent on these tasks.
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::findIndepedentMilestones() 
{

    indMilestones = new int[vertexCount]; //Allocate indMilestones
    indSize = 0;
    
    for(int i = 0; i < vertexCount; i++){ 

        if(outDegree[i] == 0){ //if the vertex has no out degrees = independent milestone

            indMilestones[indSize] = i;
            indSize += 1;

        }

    }
}

/*
* FUNCTION: printAPs
* DESCRIPTION: prints out all of the APs found from findAPs with the help of APShelper
* PASSED: N/A
* OUT: prints out all APs
*/
void ganttUtils::printAPs() 
{

    cout << "------------------------------------------------------------" << endl;
    cout << "Articulation Points:" << endl;

    //cout << sourceMilestone;

    for(int i = 0; i < vertexCount; i++){

        if(aps[i]){

            cout << " " << i;

        }

    }

    cout << endl << endl;

    
}

/*
* FUNCTION: printAPs
* DESCRIPTION: In a graph, a vertex is called an articulation point if removing it (and edges through it) disconnects the graph. 
It can be thought of as a potential vulnerability (e.g., key point of failure). 
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::findAPs() 
{

    aps = new bool[vertexCount]; //Create bool aps[] array and initialize to false
    bool * visited = new bool [vertexCount]; //Create bool visited[] array and initialize to false.
    int * parent = new int [vertexCount]; //Create integer parent[] array and initialize to -1
    int * low = new int [vertexCount]; //Create integer low[] array and initialize to 0
    int * discovered = new int [vertexCount]; //Create integer discovered[] array and initialize to 0

    for(int i = 0; i < vertexCount; i++){

        aps[i] = false;
        visited[i] = false;
        parent[i] = -1;
        low[i] = 0;
        discovered[i] = 0;

    }

    for(int i = 0; i < vertexCount; i++){

        if(!visited[i]){ //For each vertex v, if v is not visited

            findAPsHelper(i, visited, discovered, low, parent, aps); //findAPsHelper(v, visited, discovered, low, parent, aps)

        }

    }

    delete[] visited;
    delete[] parent;
    delete[] low;
    delete[] discovered;
    
}

/*
* FUNCTION: findAPsHelper
* DESCRIPTION: In a graph, a vertex is called an articulation point if removing it (and edges through it) disconnects the graph. 
It can be thought of as a potential vulnerability (e.g., key point of failure). 
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::findAPsHelper(int vertex, bool visited[], int discovered[], int low[], int parent[], bool aps[])  
{

    //initialize children to 0 and time to 0
    int children = 0;
    int time = 0;

    //mark current vertex, v, as visited
    visited[vertex] = true;

    //initialize discovery order
    discovered[vertex] = ++time; //disc[v] = low[v] = ++time;
    low[vertex] = ++time;

    listNode * u = adjList[vertex].front;

    //for every adjacent u of v
    while(u != NULL){ 

        if(!visited[u -> dest]){ //if u is not visited

            children++; //children++
            parent[u -> dest] = vertex; //set parent[u] = v

            findAPsHelper(u -> dest, visited, discovered, low, parent, aps); //findAPsHelper(u, visited, discovered, low, parent, aps)
            low[vertex] = min(low[vertex], low[u -> dest]); //low[v] = min(low[v], low[u])

            if(parent[vertex] == -1 && children > 1){ //if parent[v] == -1 and children > 1

                aps[vertex] = true; //aps[v] = true

            }

            if(parent[vertex] != -1 && low[u -> dest] >= discovered[vertex]){ //if parent[v] != -1 and low[u] ≥ disc[v]

                aps[vertex] = true; //aps[v] = true

            }

        } else if(vertex != parent[vertex]){ //else if (v != parent[v])

            low[vertex] = min(low[vertex], discovered[u -> dest]); ////low[v] = min(low[v], disc[u])

        }

        u = u -> next; //next neighbor 

    }

    
}

/*
* FUNCTION: topoSort
* DESCRIPTION: Creates a topological sort
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::topoSort() 
{

    if(adjList == NULL){

        cout << "topoSort: Error, no graph data." << endl;
        return;

    }

    int * fakeDegree = new int[vertexCount];
    topoNodes = new int[vertexCount];
    topoCount = 0;

    linkedQueue<int> qNode;

    //create in-degree array and initialize all elements to 0
    for(int i = 0; i < vertexCount; i++){

        fakeDegree[i] = inDegree[i];
        if(inDegree[i] == 0){

            qNode.addItem(i);

        }

    }

    listNode * tempNode;

    //create topo[] nodes array and initialize all elements
    while(!qNode.isEmptyQueue()){ //While queue is not empty

        topoNodes[topoCount] = qNode.front();
        topoCount = topoCount + 1;

        //Compute in-degree for each vertex in graph
        tempNode = adjList[qNode.front()].front;

        while(tempNode != NULL){

            fakeDegree[tempNode -> dest]--; //decrease in-degree by 1 for all of it neighbors

            if(fakeDegree[tempNode -> dest] == 0){ //if in-degree of a neighbor is reduced to 0, add to queue.

                qNode.addItem(tempNode -> dest); //if in-degree of a neighbor is reduced to 0, add to queue.

            }

            tempNode = tempNode -> next;

        }

        qNode.deleteItem(); //remove a vertex from queue

    }

    delete[] fakeDegree;
    
}

/*
* FUNCTION: printTopoSort
* DESCRIPTION: Prints out the topological sort
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::printTopoSort() 
{

    cout << "------------------------------------------------------------" << endl;
    cout << "Topological Sort: ";
    cout << endl;

    for(int i = 0; i < topoCount; i++){

        cout << " " << topoNodes[i]; 

    }

    cout << endl << endl << endl;
    
}

/*
* FUNCTION: criticalPath
* DESCRIPTION: The following algorithm is used to find the critical path in a directed acyclic graph. 
For each vertex, must find the minimum possible starting time of each activity.
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::criticalPath() 
{

    linkedQueue<int> qNode;
    int * fakeDegrees = new int[vertexCount];

    distances = new int[vertexCount];

    for(int i = 0; i < vertexCount; i++){

        if(inDegree[i] == 0){ //enqueue all vertices with an in-degree of 0

            qNode.addItem(i);

        }

        fakeDegrees[i] = inDegree[i]; //deep copy into fake degrees

        distances[i] = 0; //initialize all elements to 0 
    }


    while(!qNode.isEmptyQueue()){

        int qFront = qNode.front();
        listNode * lFront = adjList[qFront].front;

        while(lFront != NULL){ 

            //if distances[lFront -> dest] is <= to distances[qFront] + lFront -> weight
            //then replace the value because it is bigger
            //lFront = where we are in the list
            //qFront = where we are in the queue (distance)
            distances[lFront -> dest] = max(distances[lFront -> dest], distances[qFront] + lFront -> weight);

            fakeDegrees[lFront -> dest] -= 1; //this indegree has been processed
            if(fakeDegrees[lFront -> dest] == 0){ //queue if no indegrees left

                qNode.addItem(lFront -> dest);

            }

            lFront = lFront -> next; //move on the next neighbor 

        }

        qNode.deleteItem(); //to advance the queue

    }


    duration = 0;
    int finalMile = sourceMilestone;

    for(int i = 0; i < vertexCount; i++){

        if(duration < distances[i]){ //finds longest path

            duration = distances[i];
            finalMile = i;

        }

    }


    //finds the critical path
    //finds the way back to the source node 
    //reconstructs backwards

    listNode * tempNode;
    crPath = new int [vertexCount];
    crPathSize = 0; 

    crPath[crPathSize] = finalMile;
    crPathSize = crPathSize + 1;

    while(finalMile != sourceMilestone){

        for(int i = 0; i < vertexCount; i++){ //check all nodes for original distance

            tempNode = adjList[i].front;

            while(tempNode != NULL){ //check all outdegrees for that node

                if(tempNode -> dest == finalMile){ //if outdegree leads to the current end 

                    if(distances[finalMile] == distances[i] + tempNode -> weight){ //if this distance matches degrees distance

                        crPath[crPathSize] = i; //found the previous in the path
                        crPathSize = crPathSize + 1; //add to crPath
                        finalMile = i; //new final milestone
                        //break;

                    }

                }

                tempNode = tempNode -> next; //check the rest of the out-degrees (edges)

            }

        }

    }

    delete[] fakeDegrees;

    
}

/*
* FUNCTION: printCriticalPath
* DESCRIPTION: The following function is used to print the numbers of the critical path
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::printCriticalPath() 
{

    cout << "------------------------------------------------------------" << endl;
    cout << "Critical Path:" << endl;
    cout << "   Source Node: " << sourceMilestone << endl;
    cout << "   Final Task: " << crPath[0] << endl;
    cout << "   Total Duration: " << duration << endl;
    cout << endl;
    cout << "Critical Path: " << endl;
    for(int i = crPathSize - 1; i >= 0; i--){ //has to be done backwards because insert nodes O(1) :^) aka insert in front WHATS MAKES IT GO SUPPA FAST

        cout << " " << crPath[i];

    }
    cout << endl << endl;
    
}

/*
* FUNCTION: findSlackTimes
* DESCRIPTION: Slack time is the time that a task in a project network can be delayed without delaying subsequent tasks or the overall project.
This would apply only to tasks that were not on the critical path. The following algorithm will compute the amount of time between the start 
of the next task and the end time of the current task.
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::findSlackTimes() 
{
    
    slackTimes = new int[vertexCount];
    listNode * temp;

    //Create and initialize slackTimes[] to -1
    for(int i = 0; i < vertexCount; i++){

        slackTimes[i] = -1;

    }

    //Set slackTimes[i] to 0 for each i on the critical path
    for(int i = 0; i < crPathSize; i++){

        slackTimes[crPath[i]] = 0;

    }

    slackTimeCount = vertexCount - crPathSize;

    //For each (u, v) not on the critical path
    //slackTimes[u] = distance[v] - (distance[u] + time(u, v))

    int switchTime = 0;

    for(int u = 0; u < vertexCount; u++){

        if(slackTimes[u] == -1){

            temp = adjList[u].front;

            while(temp != NULL){

                slackTimes[u] = distances[temp -> dest] - (distances[u] + temp -> weight);

                temp = temp -> next;

            }

            switchTime++;

        }

        if(switchTime == slackTimeCount){

            break;

        }

    }
    
}

/*
* FUNCTION: printSlackTimes
* DESCRIPTION: print out slackTime numbers 
* PASSED: N/A
* OUT: N/A
*/
void ganttUtils::printSlackTimes() 
{

    cout << "------------------------------------------------------------" << endl;
    cout << "Slack Times (task-slacktime):" << endl;

    int flag = 0;

    for(int i = 0; i < vertexCount; i++){

        if(slackTimes[i] != 0 && slackTimes[i] != -1){

            cout << " " << i << "-" << slackTimes[i];
            flag++;
            if(flag == slackTimeCount){

                break;

            }

        }

    }

    cout << endl << endl;
    
}

/*
* FUNCTION: addEdge
* DESCRIPTION: Used in readGraph to input each valid Edge from the input file
* PASSED: source vertex, destination vertex, edge weight
* OUT: N/A
*/

void ganttUtils::addEdge(int source, int destination, int weight) 
{

    if(source < 0 || source >= vertexCount || destination < 0 || destination >= vertexCount){

        cout << "addEdge: Error, invalid vertex." << endl;
        return;

    }

    if(source == destination){

        cout << "addEdge: Error, source vertex and destination vertex can not be the same." << endl;
        return;

    }

    //Allocate new node
    listNode * newNode; 
    newNode = new listNode;

    //Assign the correct values
    newNode -> weight = weight;
    newNode -> dest = destination;
    newNode -> next = NULL;

    if(adjList[source].front == NULL){ //if there is nothing, insert first node

        adjList[source].front = newNode;
        edgeCount = edgeCount + 1;
        return;

    } else { //updates link 

        newNode -> next = adjList[source].front; //last in not really the front front
        adjList[source].front = newNode; 
        edgeCount = edgeCount + 1;
        return;

    }

    
}

/*
* FUNCTION: printGraphInformation
* DESCRIPTION: prints out all the graph information
* IN: N/A
* OUT: title, milestone count, source milestone, task/milestone ratio, project task density, key milestone, node point, independent milestones
*/
void ganttUtils::printGraphInformation() 
{
    cout << "------------------------------------------------------------" << endl;
    cout << "Graph Information" << endl;
    cout << "   Project title: " << title << endl;
    cout << "   Milestone Count: " << vertexCount << endl;
    cout << "   Task Count: " << edgeCount << endl;
    cout << "   Source Milestone: " << sourceMilestone << endl;
    cout << "   Tasks/Milestones Ratio: " << fixed << setprecision(6) << ratio << endl;
    cout << "   Project Tasks Density: " << fixed << setprecision(6) << density << endl;
    cout << endl;
    cout << "   Key Milestone: " << keyMilestone << ", in-degree: " << keyMilestoneDegree << " tasks" << endl;
    cout << "   Node Point: " << nodePoint << ", out-degree: " << nodePointDegree << " tasks" << endl;
    cout << "   Independent Milestones" << endl;

    for(int i = 0; i < indSize; i++){

        cout << " " << indMilestones[i];

    }

    cout << endl << endl;

}

/*
* FUNCTION: printGraph
* DESCRIPTION: prints out the graph in the correct horrific format
* IN: N/A
* OUT: (all correctly formated) 
*/
bool ganttUtils::printGraph() 
{

    listNode * temp;

    //Print all the titles and labels 
    cout << "------------------------------------------------------------" << endl;
    cout << "Graph Adjacency List:" << endl;
    cout << "   Title: " << title << endl << endl;
    cout << "Vertex" << "    " << "vrt /weight | vrt /weight | vrt /weight | ..." << endl;
    cout << "------    ----------------------------------------------" << endl;

    //Print out the adjList

    for(int i = 0; i < vertexCount; i++){

        cout << setw(6) << i << " ->";

        temp = adjList[i].front;

        if(temp == NULL){

            cout << "    " << "None" << endl;
            continue;

        }

        while(temp != NULL){

            cout << setw(5) << temp -> dest << "/" << setw(6) << temp -> weight << " |";

            temp = temp -> next;

        }

        cout << " " << endl;

    }

    cout << endl;

    temp = NULL;

    return true;
    
}

/*
* FUNCTION: getTaskCount
* DESCRIPTION: returns the edge count
* IN: N/A
* OUT: N/A
*/

int ganttUtils::getTaskCount() 
{

    return edgeCount;
    
}
