#pragma once

#include <valarray>
#include "vector.hpp"
#include "linked_list.hpp"

const size_t default_size = 4;

template<class K, class T>
struct hash_table_node
{
    K key;
    T data;
};

template<class K, class T>
class hash_table
{
private:
    vec<linked_list<hash_table_node<K, T>>> table;
    std::function<size_t(const K &, size_t)> hash_func;
    size_t count = 0;
    float max_load_factor = 0.75;

public:
    hash_table(std::function<size_t(const K &, size_t)> hash_func)
        : table(default_size), hash_func(hash_func)
    {};

    T *at(const K &key) const;
    T *operator[](const K &key) const
    { return this->at(key); }

    void insert(const K &key, const T &val);
    void remove(const K &key);

    std::string to_string() const;

    void clear();
    ~hash_table()
    { clear(); }
private:
    hash_table_node<K, T> *find_node(const K &key) const;
    void extend_and_rehash();
};
template<class K, class T>
std::string hash_table<K, T>::to_string() const
{
    std::stringstream ss{};
    ss << " {";
    ss << "\n\tcount of elements: " << count << ",";
    ss << "\n\ttable: [";

    for (size_t i = 0; i < table.get_size(); i++) {
        table[i].foreach([&ss](auto &node) {
            ss << "\n\t\t" << node.key << " => " << node.data << ",";
        });
    }
    ss << "\n\t]";
    ss << "\n}";
    return ss.str();
}

template<class K, class T>
hash_table_node<K, T> *hash_table<K, T>::find_node(const K &key) const
{
    size_t ind = hash_func(key, table.get_size());
    return table[ind].find([&key](auto &node) {
        return node.key == key;
    });
}

template<class K, class T>
void hash_table<K, T>::remove(const K &key)
{
    auto ind = hash_func(key, table.get_size());

    if (table[ind].remove_one_if([&key](auto &node) { return node.key == key; })) {
        count--;
    }
}

// --------------- definitions ---------------
template<class K, class T>
void hash_table<K, T>::extend_and_rehash()
{
    size_t prev_table_size = table.get_size();
    table.resize(prev_table_size * 2, prev_table_size * 2);

    for (size_t i = 0; i < prev_table_size; i++) {
        for (size_t ii = 0; ii < table[i].get_size(); ii++) {
            size_t hash;
            if ((hash = hash_func(table[i][ii].key, table.get_size())) != i) {
                table[hash].push_back(table[i][ii]);
                table[i].remove(ii);
            }
        }
    }
}

template<class K, class T>
void hash_table<K, T>::clear()
{
    table.clear();
    count = 0;
}

template<class K, class T>
void hash_table<K, T>::insert(const K &key, const T &val)
{
    size_t ind = hash_func(key, table.get_size());
    auto *node = find_node(key);

    if (node) {
        node->data = val;
        return;
    }

    // new element
    table[ind].push_back({key, val});

    if ((double) (++count) / table.get_size() > max_load_factor)
        extend_and_rehash();
}

template<class K, class T>
T *hash_table<K, T>::at(const K &key) const
{
    auto *node = find_node(key);

    if (node) return &node->data;
    return nullptr;
}

size_t default_hash_func(const std::string &key, size_t mod)
{
    size_t li = key.length() - 1;
    size_t hash = 0;
    for (size_t i = 0; i <= li; i++) {
        hash += key[i] * (size_t) pow(31, (double) (li - i));
    }
    return hash % mod;
}