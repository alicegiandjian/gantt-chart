//Alice Giandjian, CS302 - 1004, Assignment #11
/*
* DESCRIPTION: Design a set of C++11 classes to read, store, and process a large directed acyclic graph3 representing a Gantt Chart; 
tasks (edges) and milestones (vertices's).
* INPUT: The input files will be task lists. The first three lines will contain a project title, milestone count, and source node (in that order).
* OUTPUT: Graph Information, Dependency Statistics (In-Degree/Out-Degree), Topological Sort, Articulation Points, Critical Path, Slack Time,
and the optional printing of the graph. 
*/


#ifndef linkedQueueH
#define linkedQueueH

#define SIZE 32000

template <class myType>
struct queueNode {

    myType dataSet[SIZE];
    int front, back;
    queueNode<myType> *link;

};

template <class myType>
class linkedQueue { 

public:
linkedQueue();
~linkedQueue();
bool isEmptyQueue() const;
void initializeQueue();
void addItem(const myType& newItem);
myType front() const;
myType back() const;
void deleteItem();
int queueCount();
void printQueue();

private:
queueNode<myType> *queueFront; //pointer that points to the first node of the queue
queueNode<myType> *queueRear; //pointer that points to the last node of the queue
int count; //holds the amount of items in the custom data structure 

};

using namespace std;
#include <iostream>

//The linkedQueue() constructor should initialize the queue to an empty state (queueFront
//= NULL and queueRear = NULL)
template <class myType>
linkedQueue<myType>::linkedQueue()
{

    queueFront = NULL;
    queueRear = NULL;
    count = 0;

}
//The ~linkedQueue() destructor deletes the queue (including releasing all the
//allocated memory).
template <class myType>
linkedQueue<myType>::~linkedQueue()
{

queueNode<myType>* temp; //temporary value 

while(queueFront != NULL) {

    temp = queueFront -> link;
    delete queueFront;
    queueFront = temp;

}

}
//The isEmptyQueue() function should determine whether the queue is empty, returning
//true if the queue is empty and false if not.
template <class myType>
bool linkedQueue<myType>::isEmptyQueue() const
{

if(count == 0) { //if the count is 0 it should be empty

    return true;

} else {

    return false;

}

}
//The initializeQueue() function will create and initialize a new, empty queue.
template <class myType>
void linkedQueue<myType>::initializeQueue()
{

queueNode<myType> *temp; //temporary value

while(queueFront != NULL) { //deletes old queue

    temp = queueFront -> link;
    delete queueFront;
    queueFront = temp;

}

temp = new queueNode<myType>(); //adds the first new node

//initializes the new node in the queue
temp -> front = 0;
temp -> back = 0;
temp -> link = NULL;
queueFront = temp;
queueRear = temp;

count = 0; //resets the count

}
//The addItem(const Type& newItem) function will add the passed item to the back of the
//queue.
template <class myType>
void linkedQueue<myType>::addItem(const myType& newItem)
{

queueNode<myType> *temp; //temporary value

if( queueRear == NULL) { //initializes queue if empty

initializeQueue();

}

if(queueRear -> back >= SIZE) {//If there is no space left

    temp = new queueNode<myType>(); //makes new node
    temp -> link = NULL;
    temp -> front = 0;
    temp -> back = 0;

    queueRear -> link = temp;
    queueRear = temp;

}

queueRear -> dataSet[queueRear -> back] = newItem; //adds new item to back of array within the queue
queueRear -> back = queueRear -> back + 1; //increments back
count = count + 1; 

}
//The front() function will return the current front of the queue.
template <class myType>
myType linkedQueue<myType>::front() const
{

return queueFront -> dataSet[queueFront -> front];

}
//The back() function return the current back of the queue.
template <class myType>
myType linkedQueue<myType>::back() const
{

//needs -1 or else will go out of bounds because back points to something that doesnt exist
return queueRear -> dataSet[queueRear -> back - 1];

}
//The deleteItem() function will remove the front item from the queue (and return nothing).
//If the queue is empty, nothing should happen.
template <class myType>
void linkedQueue<myType>::deleteItem()
{

if(queueFront == NULL) { //If empty queue do nothing

return; 

}

queueFront -> front = queueFront -> front + 1;
count = count - 1;

if(queueFront -> front >= SIZE) { //deletes a node

    queueNode<myType>* temp = queueFront;
    queueFront = queueFront -> link;
    delete temp;

}

}
//The queueCount() function should return the current count of elements in the queue.
template <class myType>
int linkedQueue<myType>::queueCount()
{

return count; 

}
//The printQueue() function should print the current elements of queue.
template <class myType>
void linkedQueue<myType>::printQueue()
{

queueNode<myType>* temp = queueFront; //temporary value

int current = 0;
for(int i = 0; i < count; i++) {

    cout << temp -> dataSet[current] << " ";
    current++;

    if(current == temp -> back) { //resets current when getting to next node

        temp = temp -> link;
        current = 0;

    }

}

}

#endif
