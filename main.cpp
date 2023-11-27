#include <iostream>
#include "hash_table.hpp"

int main() {
    hash_table<std::string, int> hashmap(default_hash_func);
    hashmap.insert("Poniedziałek", 1);
    hashmap.insert("Wtorek", 2);
    hashmap.insert("Środa", 3);
    hashmap.insert("Czwartek", 4);
    hashmap.insert("Piątek", 5);
    hashmap.insert("Sobota", 6);
    hashmap.insert("Niedziela", 7);

    std::cout << *hashmap["Piątek"] << std::endl;
    hashmap.remove("Piątek");
    std::cout << (hashmap["Piątek"] == nullptr ? "removed successfully" : "wtf?") << std::endl;

    std::cout << hashmap.to_string() << std::endl;

    return 0;
}
