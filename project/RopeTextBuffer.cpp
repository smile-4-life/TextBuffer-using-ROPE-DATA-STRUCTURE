#include "RopeTextBuffer.h"

using namespace std;

// =========================
// DoublyLinkedList::Node
// =========================
template <typename T>
NodeDDL<T>::NodeDDL(T d) : data(d), prev(nullptr), next(nullptr) {}


// =========================
// DoublyLinkedList
// =========================
template <typename T>
DoublyLinkedList<T>::DoublyLinkedList() {
    this -> head = nullptr;
    this -> tail = nullptr;
    this -> count = 0;
}

template <typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
    while (this-> head != nullptr) {
        NodeDDL<T>* nextNode = this -> head -> next;
        delete this-> head;
        this -> head = nextNode;
    }
    this-> tail = nullptr;
    this-> count = 0;
}

template <typename T>
int DoublyLinkedList<T>::size() const {
    return this-> count;
}

//set
template <typename T>
void DoublyLinkedList<T>::setHead(NodeDDL<T>* node) {
     this->head = node;
}

template <typename T>
void DoublyLinkedList<T>::setTail(NodeDDL<T>* node) {
    this->tail = node;
}


template <typename T>
void DoublyLinkedList<T>::removeTail() {
    if (this->count == 0) return;
    deleteAt(this->count-1);
}

//get
template <typename T>
NodeDDL<T>* DoublyLinkedList<T>::getHead() const {
    return this->head;
}

template <typename T>
NodeDDL<T>* DoublyLinkedList<T>::getTail() const {
    return this->tail;
}

//insert
template <typename T>
void DoublyLinkedList<T>::insertAtTail(T data) {
    NodeDDL<T>* newNode = new NodeDDL<T>(data);
    newNode-> prev = this-> tail;
    newNode-> next = nullptr;
    if(this-> tail == nullptr) {
        this-> head = newNode;
    }
    else {
        this-> tail-> next = newNode;
    }
    this-> tail = newNode;
    this-> count += 1;
}

//delete
template <typename T>
void DoublyLinkedList<T>::deleteAt(int index) {
    if ((index < 0) || (index >= this-> count)) {
        throw out_of_range("Index is invalid!");
    }
    NodeDDL<T>* current;
    if (index == 0){
        //if (this-> head == nullptr) return; //index >= count thi` catch o? tre^n ro`i
        //else {
        current = this-> head;
        this-> head = this-> head-> next;
        if (this-> head == nullptr) this-> tail = nullptr;
        else this-> head-> prev = nullptr;
    }
    else if(index == this-> count - 1){
        current = this-> tail;
        this-> tail = this-> tail-> prev;
        this-> tail-> next = nullptr;       //crash neu tail la nullptr, nhung da duoc catch khi indec == 0
    }
    else {
        current = this-> head;
        for (int i = 0; i < index; ++i) {
            current = current-> next;
        }
        current-> prev-> next = current-> next;
        current-> next-> prev = current-> prev;
    }
    delete current;
    this-> count -= 1;
}


// =========================
// Rope::Node
// =========================

Rope::Node::Node() 
    : left(nullptr), right(nullptr), data(""), weight(0),          height(0), balance(EH) {}

Rope::Node::Node(const string& s)
    : left(nullptr), right(nullptr), data(s) , weight(s.length()), height(1), balance(EH) {}

bool Rope::Node::isLeaf() const {
    return left == nullptr && right == nullptr;
}

// =========================
// Rope
// =========================

int Rope::height(Node*node) const {
    return node ? node-> height : 0; //if have node, return its height
}

int Rope::getTotalLength(Node* node) const {
    if (!node) return 0;
    Node* current = node;
    if (current->isLeaf())  return current->data.length();
    return current->weight + getTotalLength(current->right);
}

void Rope::update(Node* node) {
    if (!node) return;
    
    // Update weight (sum of left subtree weights or leaf data length)
    node->weight = node->left ? 
        node->left->weight : 0;
    
    // Update height
    node->height = max(height(node->left), height(node->right)) + 1;
    
    // Update balance factor
    node->balance = static_cast<Node::BalanceFactor>(height(node->left) - height(node->right));
}

Rope::Node* Rope::rotateLeft(Node* x) {
    Node* y = x->right;
    if (y->left) x->right = y->left;
    y->left = x;

    update(x);
    update(y);

    return y;
}

Rope::Node* Rope::rotateRight(Node* y) {
    Node* x = y->left;
    if(x->right) y->left = x->right;
    x->right = y;

    update(y);
    update(x);

    return x;
}

Rope::Node* Rope::rebalance(Node* node) {
    if (node->balance == Node::LH) {
        if(node->left && node->left->balance == Node::RH) {
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);
    }
    if (node->balance == Node::RH) {
        if(node->right && node->right->balance == Node::LH) {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    }
    return node;
}   

void Rope::split(Node* node, int index, Node*& outLeft, Node*& outRight) {
    if (!node) {
        outLeft = outRight = nullptr;
        return;
    }

    if (node->isLeaf()) {
        if (index <= 0) {
            outLeft = nullptr;
            outRight = new Node(node->data);
        } else if (index >= (int)node->data.size()) {
            outLeft = new Node(node->data);
            outRight = nullptr;
        } else {
            outLeft = new Node(node->data.substr(0, index));
            outRight = new Node(node->data.substr(index));
        }
        return;
    }

    if (index < node->weight) {
        Node* splitLeft = nullptr;
        Node* splitRight = nullptr;
        split(node->left, index, splitLeft, splitRight);
        outLeft = splitLeft;
        outRight = concatNodes(splitRight, node->right);
    } else {
        Node* splitLeft = nullptr;
        Node* splitRight = nullptr;
        split(node->right, index - node->weight, splitLeft, splitRight);
        outLeft = concatNodes(node->left, splitLeft);
        outRight = splitRight;
    }
    
    // Ensure new nodes are properly balanced
    if (outLeft) outLeft = rebalance(outLeft);
    if (outRight) outRight = rebalance(outRight);
}

char Rope::charAt(Node* node, int index) const {
    if (!node) throw out_of_range("Index is invalid!");
    if (node->isLeaf()) {
        if (index < 0 || index >= (int)node->data.size())
            throw out_of_range("Index is invalid!");
        return node->data[index];
    }
    if (index < node->weight)
        return charAt(node->left, index);
    else
        return charAt(node->right, index - node->weight);
}

string Rope::toString(Node* node) const {
    if (!node) return "";
    if (node->isLeaf()) return node->data;
    return toString(node->left) + toString(node->right);
}

void Rope::destroy(Node*& node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
    node = nullptr;
}



// =========================
// public ROPE
// =========================
Rope::Rope() : root(nullptr) {}
Rope::~Rope() {
    destroy(root);
}

int Rope::length() const {
    return root ? getTotalLength(root) : 0;
}
bool Rope::empty() const {
    return root == nullptr || root->isLeaf() && root->data.empty();
}

char Rope::charAt(int index) const {
    if (index < 0) throw out_of_range("Index is invalid!");
    return charAt(root, index);
}

string Rope::substring(int start, int length) const {
    string result;
    collectSubstring(root, start, length, result);
    return result;
}

void Rope::collectSubstring(Node* node, int start, int len, string& out) const {
    if (!node || len <= 0) return;

    if (node->isLeaf()) {
        // Only proceed if we haven't collected enough yet
        if (start < (int)node->data.size()) {
            int available = node->data.size() - start;
            int take = min(len, available);
            out += node->data.substr(start, take);
        }
        return;
    }

    // If we need content from left subtree
    if (start < node->weight) {
        int leftTake = min(node->weight - start, len);
        collectSubstring(node->left, start, leftTake, out);
        
        // If we still need more, take from right
        if (leftTake < len) {
            collectSubstring(node->right, 0, len - leftTake, out);
        }
    } else {
        // All needed content is in right subtree
        collectSubstring(node->right, start - node->weight, len, out);
    }
}

void Rope::insert(int index, const string& s) {
    if (index < 0 || index > length()) 
        throw out_of_range("Index is invalid!");
    if (s.empty()) return;

    Node* leftPart = nullptr;
    Node* rightPart = nullptr;

    // Chia cây hiện tại tại vị trí chèn
    split(root, index, leftPart, rightPart);

    // Chèn từng chunk tối đa CHUNK_SIZE ký tự
    int pos = 0;
    while (pos < s.size()) {
        if (leftPart) rebalance(leftPart);
        int chunkLen = (CHUNK_SIZE> s.size() - pos)? CHUNK_SIZE : s.size() - pos;
        string chunk = s.substr(pos, chunkLen);
        Node* newLeaf = new Node(chunk);
        leftPart = concatNodes(leftPart, newLeaf);
        pos += chunkLen;
    }
    if (leftPart) rebalance(leftPart);

    // Ghép lại với phần bên phải
    root = concatNodes(leftPart, rightPart);
    update(root);
}

Rope::Node* Rope::concatNodes(Node* left, Node* right) {
    if (!left) return right;
    if (!right) return left;

    Node* newNode = new Node();
    newNode->setLeft(left);
    newNode->setRight(right);
    update(newNode);
    
    return rebalance(newNode);
}

void Rope::deleteRange(int start, int length) {
    if (start < 0 || start > this->length()) throw out_of_range("Index is invalid!");
    if (length <= 0 || start + length > this->length()) throw out_of_range("Length is invalid!");
    Node* leftPart = nullptr;
    Node* rightPart = nullptr;
    split(root, start, leftPart, rightPart);
    
    Node* toDelete = nullptr;
    split(rightPart, length, toDelete, rightPart);
    
    destroy(toDelete);
    
    root = concatNodes(leftPart, rightPart);
    update(root);
}

string Rope::toString() const {
    return root ? toString(root) : "";
}

// =========================
// RopeTextBuffer
// =========================

RopeTextBuffer::RopeTextBuffer() : rope(), cursorPos(0), history(new HistoryManager) {}
RopeTextBuffer::~RopeTextBuffer() {
    delete history;
}

void RopeTextBuffer::insert(const string& s) {
    if (s.empty()) return;
    rope.insert(cursorPos, s);
    cursorPos += s.length();

    HistoryManager::Action action;
    action.actionName = "insert";
    action.cursorBefore = cursorPos - s.length();
    action.cursorAfter = cursorPos;
    action.data = s;
    history->addAction(action);
}

void RopeTextBuffer::deleteRange(int length) {
    if (length < 0 || cursorPos + length > rope.length()) {
        throw out_of_range("Invalid deletion range");
    }

    // Get the exact text being deleted
    string deletedText = rope.substring(cursorPos, length);
    
    // Perform the deletion
    rope.deleteRange(cursorPos, length);

    // Record the action
    HistoryManager::Action action;
    action.actionName = "delete";
    action.cursorBefore = cursorPos;
    action.cursorAfter = cursorPos;  // Maintain cursor position
    action.data = deletedText;
    history->addAction(action);
}

void RopeTextBuffer::replace(int length, const string& s) {
    if (length < 0 || cursorPos + length > rope.length()) {
        throw out_of_range("Length is invalid!");
    }
    if (s.empty()) {
        rope.deleteRange(cursorPos, length);
    } else {
        rope.deleteRange(cursorPos, length);
        rope.insert(cursorPos, s);
        cursorPos += s.length();
    }

    HistoryManager::Action action;
    action.actionName = "replace";
    action.cursorBefore = cursorPos - s.length();
    action.cursorAfter = cursorPos;
    action.data = s;
    history->addAction(action);
}

void RopeTextBuffer::moveCursorTo(int index) {
    if (index < 0 || index > rope.length()) {
        throw cursor_error();
    }
    cursorPos = index;

    HistoryManager::Action action;
    action.actionName = "move";
    action.cursorBefore = cursorPos;
    cursorPos = index;
    action.cursorAfter = cursorPos;
    action.data = "J"; 
    history->addAction(action);
}

void RopeTextBuffer::moveCursorLeft() {
    if (cursorPos == 0) {
        throw cursor_error();
    }
    HistoryManager::Action action;
    action.actionName = "move";
    action.cursorBefore = cursorPos;
    cursorPos--;
    action.cursorAfter = cursorPos;
    action.data = "L";
    history->addAction(action);
}

void RopeTextBuffer::moveCursorRight() {
    if (cursorPos >= rope.length()) {
        throw cursor_error();
    }
    HistoryManager::Action action;
    action.actionName = "move";
    action.cursorBefore = cursorPos;
    cursorPos++;
    action.cursorAfter = cursorPos;
    action.data = "R";
    history->addAction(action);
}

int RopeTextBuffer::getCursorPos() const {
    return cursorPos;
}

string RopeTextBuffer::getContent() const {
    return rope.toString();
}

int RopeTextBuffer::findFirst(char c) const {
    for (int i = 0; i < rope.length(); ++i) {
        if (rope.charAt(i) == c) {
            return i;
        }
    }
    return -1; // Not found
}

int* RopeTextBuffer::findAll(char c) const {
    int count = 0;
    for (int i = 0; i < rope.length(); ++i) {
        if (rope.charAt(i) == c) {
            count++;
        }
    }
    
    if (count == 0) return nullptr; // No occurrences found
    
    int* occurrences = new int[count];
    int index = 0;
    for (int i = 0; i < rope.length(); ++i) {
        if (rope.charAt(i) == c) {
            occurrences[index++] = i;
        }
    }
    
    return occurrences;
}

void RopeTextBuffer::undo() {
    if (!history || !history->canUndo()) {
        throw runtime_error("No actions to undo.");
    }
    
    HistoryManager::Action a = history->historyActions.getTail()->data;
    history->historyActions.removeTail();
    history->undoActions.insertAtTail(a);
    if (a.actionName == "insert") {
        rope.deleteRange(a.cursorBefore, a.data.length());
        cursorPos = a.cursorBefore;
    } else if (a.actionName == "delete") {
        rope.insert(a.cursorBefore, a.data);
        cursorPos = a.cursorBefore + a.data.length();
    } else if (a.actionName == "move") {
        moveCursorTo(a.cursorBefore);
    } else if (a.actionName == "replace") {
        replace(a.cursorBefore, a.data);
        cursorPos = a.cursorBefore + a.data.length();
    }
}

void RopeTextBuffer::redo() {
    if (!history || !history->canRedo()) {
        throw runtime_error("No actions to redo.");
    }
    HistoryManager::Action a = history->undoActions.getTail()->data;
    history->undoActions.removeTail();
    history->historyActions.insertAtTail(a);
    if (a.actionName == "insert") {
        rope.insert(a.cursorAfter, a.data);
        cursorPos = a.cursorAfter + a.data.length();
    } else if (a.actionName == "delete") {
        rope.deleteRange(a.cursorAfter, a.data.length());
        cursorPos = a.cursorAfter;
    } else if (a.actionName == "move") {
        moveCursorTo(a.cursorAfter);
    } else if (a.actionName == "replace") {
        replace(a.cursorAfter, a.data);
        cursorPos = a.cursorAfter + a.data.length();
    }
}

void RopeTextBuffer::printHistory() const {
    history->printHistory();
}

// =========================
// RopeTextBuffer::HistoryManager
// =========================
RopeTextBuffer::HistoryManager::HistoryManager() {}
RopeTextBuffer::HistoryManager::~HistoryManager() {}

void RopeTextBuffer::HistoryManager::addAction(const Action& a) {
    historyActions.insertAtTail(a);
}

bool RopeTextBuffer::HistoryManager::canUndo() const {
    return historyActions.size() > 0;
}

bool RopeTextBuffer::HistoryManager::canRedo() const {
    return undoActions.size() > 0;
}

void RopeTextBuffer::HistoryManager::printHistory() const {
    cout << "[";
    NodeDDL<Action>* current = historyActions.getHead();
    while (current != nullptr) {
        const Action& a = current->data;
        cout << "(" << a.actionName << ", " << a.cursorBefore << ", " << a.cursorAfter << ", " << a.data << ")";
        current = current->next;
        if (current != nullptr) cout << ", ";
    }
    cout << "]" << endl;
}

void RopeTextBuffer::clear() {
    rope = Rope();
    cursorPos = 0;
    while(history->historyActions.size() > 0) {
        history->historyActions.removeTail();
    }
    while(history->undoActions.size() > 0) {
        history->undoActions.removeTail();
    }
}
