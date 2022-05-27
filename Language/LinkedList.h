#include <iostream>


// A linked list node
struct Node
{
    int data;
    struct Node* next = nullptr;


};

//insert a new node in front of the list
inline void push(struct Node** head, int node_data)
{
    /* 1. create and allocate node */
    struct Node* newNode = new Node;

    /* 2. assign data to node */
    newNode->data = node_data;

    /* 3. set next of new node as head */
    newNode->next = *head;

    /* 4. move the head to point to the new node */
    *head = newNode;
}

//insert new node after a given node
inline void insertAfter(struct Node* prev_node, int node_data)
{
    /*1. check if the given prev_node is NULL */
    if (prev_node == NULL)
    {
        std::cout << "the given previous node is required,cannot be NULL"; return;
    }

    /* 2. create and allocate new node */
    struct Node* newNode = new Node;

    /* 3. assign data to the node */
    newNode->data = node_data;

    /* 4. Make next of new node as next of prev_node */
    newNode->next = prev_node->next;

    /* 5. move the next of prev_node as new_node */
    prev_node->next = newNode;
}

/* insert new node at the end of the linked list */
inline void append(struct Node** head, int node_data)
{
    /* 1. create and allocate node */
    struct Node* newNode = new Node;

    struct Node* last = *head; /* used in step 5*/

    /* 2. assign data to the node */
    newNode->data = node_data;

    /* 3. set next pointer of new node to null as its the last node*/
    newNode->next = NULL;

    /* 4. if list is empty, new node becomes first node */
    if (*head == NULL)
    {
        *head = newNode;
        return;
    }

    /* 5. Else traverse till the last node */
    while (last->next != NULL)
        last = last->next;

    /* 6. Change the next of last node */
    last->next = newNode;
}

// display linked list contents
inline void displayList(struct Node* node)
{
    //traverse the list to display each node
    while (node != NULL)
    {
        std::cout << node->data << "-->";
        node = node->next;
    }

    if (node == NULL)
        std::cout << "null\n";
}

// get first element of list
inline int head(struct Node* node)
{
    if (NULL == node) return 0;
    return node->data;
}

// delete first node in the linked list
inline Node* deleteFirstNode(struct Node* head)
{
    if (head == NULL)
        return NULL;

    // Move the head pointer to the next node
    Node* tempNode = head;
    head = head->next;
    delete tempNode;

    return head;
}

// delete last node from linked list
inline Node* removeLastNode(struct Node* head)
{
    if (head == NULL)
        return NULL;

    if (head->next == NULL) {
        delete head;
        return NULL;
    }

    // first find second last node
    Node* second_last = head;
    while (second_last->next->next != NULL)
        second_last = second_last->next;

    // Delete the last node
    delete (second_last->next);

    // set next of second_last to null
    second_last->next = NULL;

    return head;
}