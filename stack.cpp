#include <iostream>
#include <thread>
#include <mutex>
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
    std::mutex stack_mutex;
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
        Node<T>* newNode = new Node<T>(data);
        this->stack_mutex.lock();
        if (this->head_ == NULL) {
            this->head_ = newNode;
        } else {
            newNode->next = this->head_;
            this->head_ = newNode;
        }
        this->stack_mutex.unlock();
        this->size_++;
    }

    T pop() {
        if (this->head_ == NULL) {
            return NULL;
        } else {
            this->stack_mutex.lock();
            T data = this->head_->data;
            this->head_ = this->head_->next;
            this->stack_mutex.unlock();
            this->size_--;
            return data;
        }
    }

    T peek() {
        return this->head_->data;
    }

    void print() {
        this->stack_mutex.lock();
        Node<T>* curr = this->head_;

        if (curr != NULL) {
            while (curr->next != NULL) {
                cout << std::to_string(curr->data) << " > ";
                curr = curr->next;
            }

            cout << std::to_string(curr->data) << "\n";
        }
        this->stack_mutex.unlock();
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

void threadOperations(Stack<int>& stack, int* operations, int operationsLen) {
    for (int i = 1; i <= operationsLen; i++) {
        if (operations[i] == 0) {
            stack.push(1);
        } else {
            stack.pop();
        }
    }
}

// Generate an array of len+1 size, 0 indicating a push operation, 1 being a pop
// 0th element is the expected size of the stack after all operations
int* generateOperations(int len) {

    srand( (unsigned) 152 ); // Set random number seed
    int* arr = (int*) malloc(sizeof(int) * (len + 1));
    int stackSize = 0;
    int operation;

    for (int i = 1; i <= len; ++i) {
        operation = rand() % 2;
        if (stackSize == 0) {
            // Make sure we don't pop more than we push
            // So we can verify the stack size at the end
            arr[i] = 0;
            stackSize++;
        } else {
            arr[i] = operation;
            if (operation == 0) {
                stackSize++;
            } else {
                stackSize--;
            }
        }
    }

    arr[0] = stackSize;

    return arr;
}

int testThreadSafe(int instructions) {
    /*
    Methodology: Because we cannot verify the order in which elements should be
    added/removed, we can only verify the total length of the final stack.
    */
    int passed = 0;
    Stack<int> stack;

    int arrSize = instructions;

    int* arr1 = generateOperations(arrSize);
    int* arr2 = generateOperations(arrSize);
    int* arr3 = generateOperations(arrSize);

    int expectedSize = arr1[0] + arr2[0] + arr3[0];

    thread th1(threadOperations, std::ref(stack), arr1, arrSize); // https://stackoverflow.com/a/64840489
    thread th2(threadOperations, std::ref(stack), arr2, arrSize);
    thread th3(threadOperations, std::ref(stack), arr3, arrSize);

    th1.join();
    th2.join();
    th3.join();

    // Free arrays after the instructions have finished being used
    free(arr1);
    free(arr2);
    free(arr3);

    if (stack.size() != expectedSize) {
        cout << stack.size() << " != " << expectedSize << "\n";
    } else {
        passed++;
    }

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

    cout << "\nStart of Thread Testing\n";
    passed = testThreadSafe(1000);
    cout << "[" << passed << "/1] Thread Test Passed\n";

    return 0;
}
