#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <sstream>

template<typename T>
struct llist_node_t
{
    llist_node_t *next = nullptr;
    llist_node_t *prev = nullptr;
    T data;

    llist_node_t(T data)
    { this->data = data; }
};

template<class T>
class linked_list
{
private:
    llist_node_t<T> *head = nullptr;
    llist_node_t<T> *tail = nullptr;
    size_t size = 0;
    bool destructor_lock = false;

public:
    linked_list() = default;
    linked_list(linked_list &&mv) noexcept {copy_properties(std::move(mv));}

    size_t get_size() const
    { return size; }

    void push_back(T data);

    void push_front(T data);

    void pop_back();

    void pop_front();

    void add_ordered(T data, std::function<bool(const T &, const T &)> compr);

    void remove(size_t ind);

    /// @return count of removed items
    size_t remove_if(std::function<bool(const T &)> compr);

    /// @return 1 if element was found and removed. 0 otherwise.
    bool remove_one_if(std::function<bool(const T &)> compr);

    void insert(size_t ind, T data);

    void replace(size_t ind, T data);

    void clear();

    void foreach(std::function<void(T &)> lambda) const;

    std::string to_string() const;

    T *find(std::function<bool(const T &)> compr) const;

    T &at(size_t ind) const;

    T &operator[](size_t ind) const;

    ~linked_list();

    linked_list& operator=(linked_list&& mv) noexcept {
        if (this != &mv)
            copy_properties(std::move(mv));
        return *this;
    }

private:
    void copy_properties(const linked_list &other);

    void copy_properties(linked_list &&other);

    void remove_node(llist_node_t<T> *node);

    void insert_after_node(llist_node_t<T> *node, T data);

    llist_node_t<T> *node_at(size_t ind) const;

    inline std::string _to_string_info() const;

    inline void _push_back_or_front(T data, bool front);

    inline void _pop_back_or_front(bool front);
};

template<class T>
void linked_list<T>::copy_properties(const linked_list &other)
{
    head = other.head;
    tail = other.tail;
    size = other.size;
}

template<class T>
void linked_list<T>::copy_properties(linked_list &&other)
{
    copy_properties(other);
    other.destructor_lock = true;
}

template<class T>
void linked_list<T>::insert_after_node(llist_node_t<T> *node, T data)
{
    llist_node_t<T> *next_node = node->next;
    llist_node_t<T> *prev_node = node;
    llist_node_t<T> *new_node;

    if (!next_node) {
        push_back(data);
        return;
    }

    new_node = new llist_node_t<T>(data);

    new_node->prev = prev_node;
    new_node->next = next_node;

    prev_node->next = new_node;
    next_node->prev = new_node;

    size++;
}

template<class T>
void linked_list<T>::add_ordered(T data, std::function<bool(const T &, const T &)> compr)
{
    llist_node_t<T> *node = head;

    if (node && compr(data, node->data)) {
        push_front(data);
        return;
    }
    while (node) {
        if (compr(data, node->data)) {
            insert_after_node(node->prev, data);
            return;
        }
        node = node->next;
    }
    push_back(data);
}

template<class T>
std::string linked_list<T>::to_string() const
{
    return _to_string_info();
}

template<class T>
std::string linked_list<T>::_to_string_info() const
{
    std::stringstream ss{};
    ss << "List: {size: " << size << ", head: " << head << ", tail: " << tail << "}";
    return ss.str();
}

template<class T>
void linked_list<T>::clear()
{
    llist_node_t<T> *node = head;
    llist_node_t<T> *next = nullptr;
    while (node) {
        next = node->next;
        delete node;
        node = next;
    }
    head = tail = nullptr;
    size = 0;
}

template<class T>
linked_list<T>::~linked_list()
{
    if(!destructor_lock)
        this->clear();
}

template<class T>
void linked_list<T>::foreach(std::function<void(T &)> lambda) const
{
    llist_node_t<T> *node = head;
    while (node) {
        lambda(node->data);
        node = node->next;
    }
}

template<class T>
llist_node_t<T> *linked_list<T>::node_at(size_t ind) const
{
    if (ind >= size)
        return nullptr;

    llist_node_t<T> *node = nullptr;

    if (size - ind > ind) {
        node = head;
        for (size_t i = 0; i < ind; i++) {
            node = node->next;
        }
        return node;
    } else {
        node = tail;
        for (size_t i = size - 1; i > ind; i--) {
            node = node->prev;
        }
        return node;
    }
    throw std::runtime_error("Corrupted list");
}

template<class T>
void linked_list<T>::replace(size_t ind, T data)
{
    if (head == nullptr)
        throw std::runtime_error("List is empty");

    llist_node_t<T> *node = this->node_at(ind);
    node->data = data;
}

template<class T>
void linked_list<T>::insert(size_t ind, T data)
{
    if (head == nullptr || ind == 0) {
        push_back(data);
        return;
    }

    llist_node_t<T> *pnode = this->node_at(ind - 1);
    if (!pnode) throw std::runtime_error("index out-of-bounds");

    insert_after_node(pnode, data);
}

template<class T>
void linked_list<T>::remove(size_t ind)
{
    if (head == nullptr)
        return;

    if(ind == 0)
        pop_front();
    else if(ind == size - 1)
        pop_back();
    else
        remove_node(this->node_at(ind));
}

template<class T>
size_t linked_list<T>::remove_if(std::function<bool(const T &)> compr)
{
    llist_node_t<T> *node = head;
    size_t rn = 0;
    while (node) {
        if (compr(node->data)) {
            llist_node_t<T> *next = node->next;
            this->remove_node(node);
            node = next;
            rn++;
        } else
            node = node->next;
    }
    return rn;
}

template<class T>
bool linked_list<T>::remove_one_if(std::function<bool(const T &)> compr)
{
    llist_node_t<T> *node = head;
    while (node) {
        if (compr(node->data)) {
            this->remove_node(node);
            return true;
        }
        node = node->next;
    }
    return false;
}

template<class T>
inline void linked_list<T>::_push_back_or_front(T data, bool front)
{
    auto *new_node = new llist_node_t<T>(data);

    if (head == nullptr) {
        head = tail = new_node;
    } else {
        if (!front) {
            new_node->prev = tail;
            tail->next = new_node;
            tail = new_node;
        } else {
            new_node->next = head;
            head->prev = new_node;
            head = new_node;
        }
    }
    size++;
}

template<class T>
inline void linked_list<T>::_pop_back_or_front(bool front)
{
    if (head == nullptr)
        return;

    if (head == tail) {
        delete head;
        tail = head = nullptr;
    } else if (front) {
        llist_node_t<T> *new_head = head->next;
        delete head;
        head = new_head;
        new_head->prev = nullptr;
    } else {
        llist_node_t<T> *new_tail = tail->prev;
        delete tail;
        tail = new_tail;
        new_tail->next = nullptr;
    }
    size--;
}

template<class T>
void linked_list<T>::push_back(T data)
{
    _push_back_or_front(data, false);
}

template<class T>
void linked_list<T>::push_front(T data)
{
    _push_back_or_front(data, true);
}

template<class T>
void linked_list<T>::pop_back()
{ _pop_back_or_front(false); }

template<class T>
void linked_list<T>::pop_front()
{ _pop_back_or_front(true); }

template<class T>
T &linked_list<T>::at(size_t ind) const
{
    auto node = node_at(ind);
    if (!node)
        throw std::runtime_error("index out-of-bounds");
    return node->data;
}

template<class T>
T &linked_list<T>::operator[](size_t ind) const
{
    return this->at(ind);
}

template<class T>
T *linked_list<T>::find(std::function<bool(const T &)> compr) const
{
    llist_node_t<T> *node = head;
    while (node) {
        if (compr(node->data))
            return &node->data;
        node = node->next;
    }
    return nullptr;
}

template<class T>
void linked_list<T>::remove_node(llist_node_t<T> *node)
{
    if (size == 0)
        return;

    llist_node_t<T> *next_node = node->next;
    llist_node_t<T> *prev_node = node->prev;

    if (next_node)
        next_node->prev = prev_node;
    else
        tail = prev_node;

    if (prev_node)
        prev_node->next = next_node;
    else
        head = next_node;

    size--;
}
