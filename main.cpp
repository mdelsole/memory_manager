#include <iostream>
#include <map>
#include <math.h>
#include <limits>
using namespace std;
#include "memory.h"


/*     Node/linkedList construction     */

//Set the first node in the linkedList to null
template<class T>
linkedList<T>::linkedList(): first(nullptr) {}

//Utility method to grab the first node
template<class T>
Node<T> *linkedList<T>::getFirst() {
    return first;
}

//Set the next node by setting its pointer
template<class T>
Node<T> *linkedList<T>::setFirst(Node<T> *first) {
    this->first = first;
    return this->first;
}

//Constructor for Node: fills the node with the inputted value and points to the next node (null cause its the end)
template <class T>
Node<T>::Node(T input):content(input), next(nullptr) {}

//Getter for node class
template <class T>
T Node<T>::getContent() { return content; }

//Setter for node class
template <class T> T Node<T>::setContent(T value) {
    content = value;
    return content;
}

//Get a pointer to the next node
template<class T>
Node<T> *Node<T>::getNextNode() {
    return next;
}

//Set the next node, for if we need to change order somewhere
template<class T>
Node<T> *Node<T>::setNextNode(Node<T> *node) {
    next = node;
    return next;
}

template<class T>
Node<T> *linkedList<T>::append(T content) {
    //If the linkedList is empty, put T value at the first spot, and increase the size
    if (first == nullptr) {
        first = new Node<T>(content);
        return first;
    }
    //If it's not empty, use a while loop to find the end where we will place the value
    Node<T>* temp = first;
    while (temp->getNextNode() != nullptr) {
        temp = temp->getNextNode();
    }
    //Once we've reached the end, add a new node and put that node's content as T value
    temp->setNextNode(new Node<T>(content));
    return temp->getNextNode();
}

//Constructor takes in which algorithm we're using and the number of pages
memoryManager::memoryManager(string algorithmInput, int pages) {
    algorithm = algorithmInput;
    //TODO: Make this say which algorithm
    cout<<"Ok, we're using the "<< algorithm<<"-case algorithm"<<endl;
    //Create our list of available chunks
    freeMemory = linkedList<chunk>();
    //Create our list of unavailable chunks, but of course it's empty right now
    usedMemory = linkedList<allocatedChunk>();
    //Set the length of our list to the inputted total number of memory pages
    freeMemory.append(chunk(0, pages - 1));

}

typedef pair<Node<chunk> *, int> MyPairType;
struct comparer {
    bool operator()(const MyPairType &left, const MyPairType &right) const {
        return left.second < right.second;
    }
};

//Add the programs in
void memoryManager::programAdder(string name, int size) {
    //TODO: Get this working
    if (size<= 0 || typeid(size).name() != typeid(int).name()){
        cout<<"Not a valid size"<<endl;
        return;
    }

    //Get the beginning of the current chunk
    Node<allocatedChunk> *currentlyUsed = usedMemory.getFirst();
    //Create a map of all the free slots, with the respective address stored in that slot's index
    map<Node<chunk>*, int> freeChunks;
    //Point to the address of the currently free space, so we can store stuff at that address
    Node<chunk> *current = freeMemory.getFirst();
    //The amount of memory is the size/4 rounded up to the nearest integer, because each page is 4 kb
    int numPages = ceil(size / 4.0);

    //Make sure the program is not running already
    while (currentlyUsed != nullptr) {
        if (currentlyUsed->getContent().programName == name) {
            cout<<name << " is already running"<<endl;
            return;
        } else {
            currentlyUsed = currentlyUsed->getNextNode();
        }
    }

    //See how much space is available by calculating how much is taken up
    while (current != nullptr) {
        int freeMemSize = current->getContent().pageNumEnd - current->getContent().pageNumStart + 1;
        if (freeMemSize >= (numPages)) {
            freeChunks[current] = freeMemSize;
        }
        current = current->getNextNode();
    }

    //Make sure to stop if we run out of memory
    if (freeChunks.size() == 0){
        cout<<"Sorry, there ain't no memory left"<<endl;
        return;
    }

    Node<allocatedChunk> *newNode = nullptr;
    Node<chunk> *selectedNode = nullptr;

    if (this->algorithm == "Best") {
        pair<Node<chunk> *, int> min;
        min = *min_element(freeChunks.begin(), freeChunks.end(), comparer());
        selectedNode = min.first;
    }
        //Else it's the worst-case
    else{
        pair<Node<chunk> *, int> max;
        max = *max_element(freeChunks.begin(), freeChunks.end(), comparer());
        selectedNode = max.first;
    }

    //Set the selected chunk to the contents of the selected node
    chunk selectedChunk = selectedNode->getContent();
    //Put the chunk's data in the node
    selectedNode->setContent(chunk(selectedChunk.pageNumStart + numPages, selectedChunk.pageNumEnd));
    int chunkStart = selectedChunk.pageNumStart;
    int chunkEnd = selectedChunk.pageNumStart + numPages - 1;
    //Create a new allocatedChunk to tell our program that a chunk has been filled
    newNode = new Node<allocatedChunk>(allocatedChunk(chunkStart, chunkEnd, name));

    currentlyUsed = usedMemory.getFirst();
    Node<allocatedChunk> *lastUsed = usedMemory.getFirst();

    //If it's empty, we'll put it at the head
    if (currentlyUsed != nullptr && currentlyUsed->getContent().pageNumStart > newNode->getContent().pageNumStart) {
        newNode->setNextNode(usedMemory.getFirst());
        usedMemory.setFirst(newNode);
        cout<<"Program "<< name << " has been added. Pages used: "<< numPages<<endl;
        return;
    }

    // If it's not empty, we're going to put it once you've passed the starting page of the allocated memory
    while (currentlyUsed != nullptr) {
        if (newNode->getContent().pageNumStart < currentlyUsed->getContent().pageNumStart) {
            lastUsed->setNextNode(newNode);
            newNode->setNextNode(currentlyUsed);
            cout<<"Program "<< name << " has been added. Pages used: "<< numPages<<endl;
            return;
        } else {
            lastUsed = currentlyUsed;
            currentlyUsed = currentlyUsed->getNextNode();
        }
    }

    // Append it to the end if nothing else worked
    usedMemory.append(newNode->getContent());
    cout<<"Program "<< name << " has been added. Pages used: "<< numPages<<endl;
}

void memoryManager::defragment() {
    //Fragments are the free memory (contiguous set of pages according to wikipedia)
    Node<chunk> *current = freeMemory.getFirst();
    Node<chunk> *last = nullptr;
    //Go through all the current nodes
    while (current != nullptr) {
        if (last == nullptr) {
            last = current;
            current = current->getNextNode();
        } else {
            //Basically connect adjacent nodes based on the condition that pageNumEnd + 1 = pageNumStart
            int endOfLast = last->getContent().pageNumEnd;
            int startOfCurrent = current->getContent().pageNumStart;
            if (startOfCurrent == endOfLast + 1) {
                chunk expanded(last->getContent().pageNumStart, current->getContent().pageNumEnd);
                last->setContent(expanded);
                last->setNextNode(current->getNextNode());
                return;
            } else {
                last = current;
                current = current->getNextNode();
            }
        }
    }
}

void memoryManager::killProgram(string name) {
    int freedPageStart;
    int freedPageEnd;
    bool found = false;
    Node<allocatedChunk> *currentlyUsed = usedMemory.getFirst();
    Node<allocatedChunk> *lastUsed = nullptr;

    //We're going to deallocate the memory associated with program matching the name that was inputted
    while (currentlyUsed != nullptr && !found) {
        allocatedChunk thisChunk = currentlyUsed->getContent();
        if (thisChunk.programName == name) {
            freedPageStart = thisChunk.pageNumStart;
            freedPageEnd = thisChunk.pageNumEnd;
            if (lastUsed != nullptr) {
                lastUsed->setNextNode(currentlyUsed->getNextNode());
            } else {
                usedMemory.setFirst(currentlyUsed->getNextNode());
            }
            found = true;
        } else {
            lastUsed = currentlyUsed;
            currentlyUsed = currentlyUsed->getNextNode();
        }
    }

    //If it's not found, we don't have to anything! Yay!
    if (found != true) {
        cout<<"Couldn't find "<<name<<", sorry try again"<<endl;
        return;
    }

    chunk freed(currentlyUsed->getContent().pageNumStart, currentlyUsed->getContent().pageNumEnd);
    auto *newNode = new Node<chunk>(freed);

    if (freeMemory.getFirst() == nullptr) {
        freeMemory.setFirst(newNode);
        cout<<name<< " deleted. It had children so I hope you're happy. "
            <<(freedPageEnd - freedPageStart + 1)<< " page(s) reclaimed."<< endl;
        return;
    }

    Node<chunk> *currentlyFree = freeMemory.getFirst();
    Node<chunk> *lastFree = nullptr;
    //Go over currently free memory
    while (currentlyFree != nullptr) {
        //We need to find the starting page of the freed memory
        int current_pageNumStart = currentlyFree->getContent().pageNumStart;
        //Once we've found it, we'll create a new node that points to the first node
        if (current_pageNumStart > freed.pageNumStart) {
            newNode->setNextNode(currentlyFree);
            if (lastFree == nullptr) {
                freeMemory.setFirst(newNode);
            } else {
                lastFree->setNextNode(newNode);
            }
            //Defragment as necessary and give confirmation
            defragment();
            cout<<name<< " deleted. It had children so I hope you're happy. "
                <<(freedPageEnd - freedPageStart + 1)<< " page(s) reclaimed."<< endl;
            return;
        } else {
            lastFree = currentlyFree;
            currentlyFree = currentlyFree->getNextNode();
        }
    }
}

//Supposed to print out number of frags? The assignment was really unclear but I hope this is what it's supposed to do
//What even is a frag? It's contiguous set of pages
void memoryManager::printFrag() {
    int frags = 0;
    //We use our free memory to get the num of frags, so naturally we'll start at the first node
    Node<chunk> *current = freeMemory.getFirst();
    //Number of frags is the number of nodes in our free memory, so let's count them up
    while (current != nullptr) {
        frags++;
        current = current->getNextNode();
    }
    cout<<"There are "<<frags<< " fragment(s)."<<endl;
}

void memoryManager::printMem() {
    //Create a map of our pages used, the page num as the key and the program name as the value
    map<int, string> pagesUsed;
    //Get the head of the used memory as our currently allocated chunk
    Node<allocatedChunk> *current = usedMemory.getFirst();

    //Get all the program names that are currently running and their locations
    while (current != nullptr) {
        allocatedChunk info = current->getContent();
        for (int i = info.pageNumStart; i <= info.pageNumEnd; i++) {
            pagesUsed[i] = info.programName;
        }
        current = current->getNextNode();
    }
    //In each page print either the program name or free depending on what's there
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            int curr_page = i * 8 + j;
            if (pagesUsed.find(curr_page) != pagesUsed.end()) {
                cout<<pagesUsed[curr_page]<< " ";
            } else {
                cout<<"Free ";
            }
        }
        cout<<endl;
    }
    cout<<endl;
}

int main() {
    cout << "Welcome to the program" << endl;
    A:
    cout << "Would you like to do Best-case or Worst-case algorithm? Type 'Best' or 'Worst (no quotes): " << endl;
    string algorithm;
    cin >> algorithm;
    if (algorithm != "Best" and algorithm != "Worst"){
        cout<<"Not a valid choice"<<endl;
        goto A;
    }
    //create memory manager object
    memoryManager memMang(algorithm, 32);
    int choice, size;
    string name;
    while (choice != 5) {
        cout << "1. Add program\n" << "2. Kill program\n" << "3. Fragmentation\n" << "4. Print memory\n" << "5. Exit\n" << endl;
        cin >> choice;
        if (choice == 1) {
            cout << "Program name: ";
            cin >> name;
            cout<<"Program size (in kb): ";
            cin >> size;
            memMang.programAdder(name,size);
        } else if (choice == 2) {
            cout<<"Program name: ";
            cin >> name;
            memMang.killProgram(name);
        } else if (choice == 3) {
            memMang.printFrag();
        } else if (choice == 4) {
            memMang.printMem();
        } else if (choice == 5) {
            return 0;
        } else {
            cout << "That's not a valid choice. Try again please"<<endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    return 0;
}