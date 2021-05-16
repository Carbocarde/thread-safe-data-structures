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
