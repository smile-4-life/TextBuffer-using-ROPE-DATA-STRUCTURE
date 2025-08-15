
#ifndef ROPE_TEXTBUFFER_H
#define ROPE_TEXTBUFFER_H

#include "main.h"

/*
    Student can define other list data structures here
*/

template<typename T>
    class NodeDDL {
    public:
        NodeDDL* prev;
        NodeDDL* next;
        T data;

        NodeDDL(T d);;
    };
    
template <typename T>
class DoublyLinkedList {    
    // TODO: may provide some attributes
private:
    NodeDDL<T>* head;
    NodeDDL<T>* tail;
    int count;

public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    int size() const;
    void insertAtTail(T data);
    void deleteAt(int index);

    string toString(string (*convert2str)(T&) = 0) const;

    //get
    NodeDDL<T>* getHead() const;
    NodeDDL<T>* getTail() const;

    //set
    void setHead(NodeDDL<T>* node);
    void setTail(NodeDDL<T>* node);
    void removeTail();
};

/**
 * Rope (AVL-based, fixed leaf chunk size = 8)
 */
class Rope {
public:
    static const int CHUNK_SIZE = 8;
private:
    class Node {
    public:
        enum BalanceFactor { LH = 1, EH = 0, RH = -1 };
        friend class Rope;

    private:
        Node* left;
        Node* right;
        string data;
        int weight;
        int height;
        BalanceFactor balance;

        Node();
        explicit Node(const string& s);
        bool isLeaf() const;
    
    public:

        // Getters for private members
        Node* getLeft() const { return left; }
        Node* getRight() const { return right; }
        const string& getData() const { return data; }
        int getWeight() const { return weight; }
        int getHeight() const { return height; }
        BalanceFactor getBalance() const { return balance;} 
        
        //setter
        void setLeft(Node* l) { left = l; }
        void setRight(Node* r) { right = r; }
        void setData(const string& d) { data = d; }
        void setWeight(int w) { weight = w; }
        void setHeight(int h) { height = h; }
        void setBalance(BalanceFactor b) { balance = b; }
    };

    Node* root;

    int height(Node* node) const;
    int getTotalLength(Node* node) const;
    void update(Node* node);
    Node* rotateLeft(Node* x);
    Node* rotateRight(Node* y);
    Node* rebalance(Node* node);
    void split(Node* node, int index, Node*& outLeft, Node*& outRight);
    Node* concatNodes(Node* left, Node* right);
    char charAt(Node* node, int index) const;
    string toString(Node* node) const;
    void destroy(Node*& node);

    //helper funcitions:
    void collectSubstring(Node* node, int start, int len, std::string& out) const;

public:
    Rope();
    ~Rope();

    int length() const;
    bool empty() const;
    char charAt(int index) const;
    string substring(int start, int length) const;
    void insert(int index, const string& s);
    void deleteRange(int start, int length);
    string toString() const;

#ifdef TESTING
    friend class TestHelper;
#endif
};

class RopeTextBuffer {
public:
    class HistoryManager; 

private:
    Rope rope;
    int cursorPos;
    HistoryManager* history;

public:
    RopeTextBuffer();
    ~RopeTextBuffer();

    void insert(const string& s);
    void deleteRange(int length);
    void replace(int length, const string& s);
    void moveCursorTo(int index);
    void moveCursorLeft();
    void moveCursorRight();
    int  getCursorPos() const;
    string getContent() const;
    int findFirst(char c) const;
    int* findAll(char c) const;
    void undo();
    void redo();
    void printHistory() const;
    void clear();
#ifdef TESTING
    friend class TestHelper;
#endif
};


class RopeTextBuffer::HistoryManager {
public:
    struct Action {
        string actionName;
        int cursorBefore;
        int cursorAfter;
        string data;
    };

    // TODO: may provide some attributes
    DoublyLinkedList<Action> historyActions;
    DoublyLinkedList<Action> undoActions;

public:
    HistoryManager();
    ~HistoryManager();
    void addAction(const Action& a);
    bool canUndo() const;
    bool canRedo() const;
    void printHistory() const;

#ifdef TESTING
    friend class TestHelper;
#endif
};

#endif // ROPE_TEXTBUFFER_H

