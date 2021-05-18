#include <atomic>

namespace tsds {

namespace stack_struct {

template <typename T>
struct Node {
    T data = NULL;
    Node* next = NULL;
};

} // namespace stack_struct

template <typename T>
class Stack {

    private:
        std::atomic<int> size_ = 0;
        std::atomic<stack_struct::Node<T>*> head_ = NULL;

    public:
        int size(void) {
            return this->size_.load();
        }

        void push(T data) {
            stack_struct::Node<T>* node = (stack_struct::Node<T>*) malloc(sizeof(stack_struct::Node<T>));
            node->data = data;

            // Refresh the head reference, and if it doesn't change (AKA if
            //     node->next remains valid), set the head to the new node.
            do {
                node->next = this->head_.load();
            } while (!this->head_.compare_exchange_weak(node->next, node));

            this->size_.fetch_add(1);
        }

        T pop() {
            stack_struct::Node<T>* head;

            // Refresh the head reference, and if it doesn't change (AKA if
            // head != null and head->next remains valid), set the head to the next node.
            do {
                head = this->head_.load();
                if (head == NULL) {
                    return NULL;
                }
            } while (!this->head_.compare_exchange_weak(head, head->next));

            // Copy the data out of the old head and free the node
            T data = head->data;
            free(head);

            this->size_.fetch_sub(1);

            return data;
        }

        T peek() {
            return this->head_.load()->data;
        }

        void clear() {
            stack_struct::Node<T>* head;
            int oldSize;

            // Set head to null, and ensure we have an accurate prior stack size
            //     when it is set.
            do {
                head = this->head_.load();
                if (head == NULL) {
                    break;
                }
                // Because the compare_exchange_weak ensures that head has not changed,
                // and size always changes when head changes, we can ensure that
                // oldSize == actualSize when compare_exchange_weak sets this->head_ to NULL
                oldSize = this->size_.load();
            } while (!this->head_.compare_exchange_weak(head, NULL));

            // Set size to 0 atomically
            this->size_.fetch_sub(oldSize);

            stack_struct::Node<T>* curr = head;
            stack_struct::Node<T>* prior;

            // Traverse old stack nodes and free them individually
            while (curr != NULL) {
                prior = curr;
                curr = curr->next;
                free(prior);
            }
        }
}; // Class Stack
} // namespace tsds
