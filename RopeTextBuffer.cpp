#include "RopeTextBuffer.h"

using namespace std;


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
    return node ? node-> height : 0 //if have node, return its height
}

int Rope::getTotalLength(Node* node) const {
    if (!node) return 0;
    Node* current = node;
    if (current->isLeaf())  return current->data.length();
    return current->weight + getTotalLength(current->right);
}

void Rope::update(Node* node) {
    if (!node) return;
    node-> weight = getTotalLength(node->left);
    node-> height = max(node->left->height,node->right->height) + 1;
    if( node->left && node->right) {
        node->balance = static_cast<Node::BalanceFactor>(node->left->height - node->right->height);
    } else if (node->left) {
        node->balance = Node::LH;
    } else if (node->right) {
        node->balance = Node::RH;
    } else {
        node->balance = Node::EH;
    }
}

Node* Rope::rotateLeft(Node* x) {
    Node* y = x->right;
    if (y->left) x->right = y->left;
    y->left = x;

    update(x);
    update(y);

    return y;
}

Node* Rope::rotateRight(Node* y) {
    Node* x = y->left;
    if(x->right) y->left = x->right;
    x->right = y;

    update(y);
    update(x);

    return x;
}

Node* Rope::rebalance(Node* node) {
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
    if(node->isLeaf()) {
        if (index == 0) {
            outLeft = nullptr;
            outRight = new Node(node->data);
        } else if (index == node->data.length()-1) {
            outLeft = new Node(node->data);
            outRight = nullptr;
        } else {
        outLeft = new Node(node->data.substr(0, index));
        outRight = new Node(node->data.substr(index));
        }
        node-> left = outLeft;
        node-> right = outRight;
        update(outLeft);
        update(outRight);
        update(node);
    } else {
        if (index < node->weight) {
            split(node->left, index, outLeft, outRight);
            node->right = outRight;
        } else {
            split(node->right, index - node->weight, outLeft, outRight);
            node->left = outLeft;
        }
    }
}

char Rope::charAt(Node* node, int index) const {
    if ( node->isLeaf() && index >= node->data.length()) {
        throw out_of_range("Index is invalid!");
    }
    if (index >= node->weight) 
        return charAt(node->right, index- node->weight);
    if (node->isLeaf()) 
        return node->data[index]
    else 
        return charAt(node->left,index);
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

BalanceFactor Rope::findBalanceFactor() const {
    Node* node = root;
    if (   
        !node || 
        node->isLeaf() ||
        (!node->left && !node->right) ||
        node->left->height == node->right->height
    ) return Node::EH;
    
    if (!node->left) return Node::RH;
    if (!node->right) return Node::LH;
    
    return static_cast<Node::BalanceFactor>(node->left->height - node->right->height);
}

// =========================
// public ROPE
// =========================
Rope::Rope() : root() {}
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
    if (start < 0 || start > this->length()) throw out_of_range("Index is invalid!");
    if (length <= 0 || start + length > this->length()) throw out_of_range("Length is invalid!");
    Node* leftPart = nullptr;
    Node* rightPart = nullptr;
    split(root, start, leftPart, rightPart);
    string result = toString(leftPart) + toString(rightPart).substr(0, length);
    destroy(leftPart);
    destroy(rightPart);
    return result;
}

void Rope::insert(int index, const string& s) {
    if (index < 0 || index > length()) throw out_of_range("Index is invalid!");
    Node* leftPart = nullptr;
    Node* rightPart = nullptr;
    split(root, index, leftPart, rightPart);
    
    Node* newNode = new Node(s);
    root = concatNodes(leftPart, newNode);
    root = concatNodes(root, rightPart);
    
    update(root);
}

Node* Rope::concatNodes(Node* left, Node* right) {
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
