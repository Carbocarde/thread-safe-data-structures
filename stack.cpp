#include <iostream>
using namespace std;

template <typename T>
class Node {

    public:
        T data;
        Node *next = NULL;

        Node(T data) {
            this->data = data;
        }
};

template <typename T>
class Stack {

    private:
    int size_;
    Node<T> *head_ = NULL;

    public:
    Stack() {
        this->size_ = 0;
    }

    int size(void) {
        return this->size_;
    }

    void push(T data) {
        if (this->head_ == NULL) {
            Node<T>* newNode = new Node<T>(data);
            this->head_ = newNode;
        } else {
            Node<T>* newNode = new Node<T>(data);
            newNode->next = this->head_;
            this->head_ = newNode;
        }
        this->size_++;
    }

    T pop() {
        if (this->head_ == NULL) {
            return NULL;
        } else {
            T data = this->head_->data;
            this->head_ = this->head_->next;
            this->size_--;
            return data;
        }
    }

    T peek() {
        return this->head_->data;
    }

    void print() {
        Node<T>* curr = this->head_;

        if (curr != NULL) {
            while (curr->next != NULL) {
                cout << std::to_string(curr->data) << " > ";
                curr = curr->next;
            }

            cout << std::to_string(curr->data) << "\n";
        }
    }
};

// 2 possible points
int testInit() {
    int passed = 0;
    Stack<int> stack;

    passed += (stack.size() == 0);
    passed += (stack.pop() == NULL);

    return passed;
}

// 4 possible points
int testPushPeek() {
    int passed = 0;
    Stack<int> stack;

    stack.push(1);
    passed += (stack.size() == 1);
    passed += (stack.peek() == 1);

    stack.push(2);
    passed += (stack.size() == 2);
    passed += (stack.peek() == 2);

    return passed;
}

// 4 possible points
int testPop() {
    int passed = 0;
    Stack<int> stack;

    passed += (stack.pop() == NULL);

    stack.push(1);
    stack.push(2);

    passed += (stack.pop() == 2);
    passed += (stack.pop() == 1);
    passed += (stack.pop() == NULL);

    return passed;
}

int main() {

    int passed = 0;

    cout << "\nStart of Stack Unit Testing\n";
    passed = testInit();
    cout << "[" << passed << "/2] Initialization Tests Passed\n";
    passed = testPushPeek();
    cout << "[" << passed << "/4] Push/Peek Unit Tests Passed\n";
    passed = testPop();
    cout << "[" << passed << "/4] Pop Unit Tests Passed\n";
    return 0;
}
