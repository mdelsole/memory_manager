//
// Created by Michael DelSole on 3/31/18.
//

#ifndef MEMORY_MANAGER_MEMORY_H
#define MEMORY_MANAGER_MEMORY_H

#include <string>
using namespace std;

//Create a chunk structure. We'll store a start and an end page (contiguous block of memory), recording what pages the
//chunk takes up in the memory
struct chunk {
    int pageNumStart, pageNumEnd;
    chunk(int start, int end) {
        pageNumStart = start;
        pageNumEnd = end;
    }
};

//allocatedChunk is a chunk structure that has been filled by a program. It keeps track of the program's name, so we
//can know what it's allocated to
struct allocatedChunk : chunk {
    string programName;

    //since allocatedChunks are created from chunks, we construct the aC from a chunk
    allocatedChunk(int start, int end, string name): chunk(pageNumStart, pageNumEnd) {
        pageNumStart = start;
        pageNumEnd = end;
        programName = name;
    }
};

// Represents a node in a linked list.
template <class T>
class Node {
private:
    //The content, and the pointer to the next node, private beacuse security for programs
    T content;
    Node<T> *next;
public:
    Node(T content);
    //Getters and setters for this node's content and the pointer to the next node, for when we need to mess with them
    T setContent(T content);
    Node<T> *setNextNode(Node<T> *node);
    T getContent();
    Node<T> *getNextNode();
};

//linkedList in c++ is complicated. Needs to be specifically defined
template <class T>
class linkedList {
private:
    //First entry in the list; our starting point
    Node<T> *first;
public:
    linkedList();
    //Getters and Setters for the first node, cause the first is like the engine of a train
    Node<T> *setFirst(Node<T> *first);
    Node<T> *getFirst();
    //Put a new node containing the "value" at the end of the list
    Node<T> *append(T content);
};

class memoryManager {
private:
    string algorithm;
    //Create a linkedList of the free chunks
    linkedList<chunk> freeMemory;
    //Create a linkedList of the used chunks
    linkedList<allocatedChunk> usedMemory;

public:
    memoryManager(string algorithm, int pages);
    void programAdder(string name, int size);
    void killProgram(string programName);
    void defragment();
    void printFrag();
    void printMem();
};

#endif //MEMORY_MANAGER_MEMORY_H
