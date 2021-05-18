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
    for (int i = 1; i < operationsLen; i++) {
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

// 3 Possible Points
void testThreadSafe(int instructions) {
    /*
    Methodology: Because we cannot verify the order in which elements should be
    added/removed, we can only verify the total length of the final stack.
    */
    int passed = 0;

    int arrSize = instructions + 1;

    // Test #1: Test push operation
    Stack<int> pushStack;
    int* arr = (int*) malloc(sizeof(int) * (instructions + 1));
    for (int i = 1; i <= instructions; i++) {
        arr[i] = 0; // Set instruction to push
    }

    // Three threads means there should be 3 * instructions elements
    int expectedSize = 3 * instructions;

    thread th1(threadOperations, std::ref(pushStack), arr, arrSize);
    thread th2(threadOperations, std::ref(pushStack), arr, arrSize);
    thread th3(threadOperations, std::ref(pushStack), arr, arrSize);

    th1.join();
    th2.join();
    th3.join();

    // Free array after the instructions have finished being used
    free(arr);

    if (pushStack.size() != expectedSize) {
        cout << "Failure with push operation: " << pushStack.size() << " != " << expectedSize << "\n";
    } else {
        passed++;
    }

    cout << "[" << passed << "/1] Push operation tests passed\n";

    // Test #2: Test pop operation
    passed = 0;

    arr = (int*) malloc(sizeof(int) * (instructions + 1));
    for (int i = 1; i <= instructions; i++) {
        arr[i] = 1; // Set instruction to pop
    }

    Stack<int> popStack;

    // Initialize stack using single thread
    for (int i = 0; i < instructions*4; i++) {
        popStack.push(1);
    }

    expectedSize = popStack.size() - (3 * (instructions));

    thread th4(threadOperations, std::ref(popStack), arr, arrSize);
    thread th5(threadOperations, std::ref(popStack), arr, arrSize);
    thread th6(threadOperations, std::ref(popStack), arr, arrSize);

    th4.join();
    th5.join();
    th6.join();

    // Free array after the instructions have finished being used
    free(arr);

    if (popStack.size() != expectedSize) {
        cout << "Failure with pop operation: " << popStack.size() << " != " << expectedSize << "\n";
    } else {
        passed++;
    }

    cout << "[" << passed << "/1] Pop operation tests passed\n";

    // Test #3: Test Push/Pop operations simultaneously
    passed = 0;

    Stack<int> pushPopStack;

    int* arr1 = generateOperations(instructions);
    int* arr2 = generateOperations(instructions);
    int* arr3 = generateOperations(instructions);

    expectedSize = arr1[0] + arr2[0] + arr3[0];

    thread th7(threadOperations, std::ref(pushPopStack), arr1, arrSize);
    thread th8(threadOperations, std::ref(pushPopStack), arr2, arrSize);
    thread th9(threadOperations, std::ref(pushPopStack), arr3, arrSize);

    th7.join();
    th8.join();
    th9.join();

    // Free arrays after the instructions have finished being used
    free(arr1);
    free(arr2);
    free(arr3);

    if (pushPopStack.size() != expectedSize) {
        cout << pushPopStack.size() << " != " << expectedSize << "\n";
    } else {
        passed++;
    }

    cout << "[" << passed << "/1] Push & Pop operation tests passed\n";
}

int main() {

    int passed = 0;
    int totalPassed = 0;

    cout << "\nStart of Stack Unit Testing\n";
    passed = testInit();
    totalPassed += passed;
    cout << "[" << passed << "/2] Initialization Tests Passed\n";
    passed = testPushPeek();
    totalPassed += passed;
    cout << "[" << passed << "/4] Push/Peek Unit Tests Passed\n";
    passed = testPop();
    totalPassed += passed;
    cout << "[" << passed << "/4] Pop Unit Tests Passed\n";

    if (totalPassed < 10) {
        cout << "\nWARNING: Not all single-thread tests passed. Please fix before attempting to test multithreading\n";
    } else {
        cout << "\nStart of Thread Testing\n";
        testThreadSafe(100);
    }

    return 0;
}
