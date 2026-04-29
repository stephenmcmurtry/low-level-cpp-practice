#include <iostream>
#include <utility>
#include <vector>
#include <functional>

template <typename K, typename V>
class OriginalHashMap {
    public:
        OriginalHashMap() {
            currSize = 0;
            buckets.resize(10);
        }

        void insert(std::pair<K, V> p) {
            size_t hashVal = std::hash<K>{}(p.first);
            int hashBucket = hashVal % 10;
            std::cout << "bucket is " << hashBucket << std::endl;
            
            if (buckets.at(hashBucket).size() != 0) {
                std::vector<std::pair<K, V>>& list = buckets.at(hashBucket);
                list.push_back(p);
                std::cout << "Adding " << std::endl;
            } else {
                std::vector<std::pair<K, V>> list;
                list.push_back(p);
                buckets[hashBucket] = list;
                std::cout << "Inserting " << std::endl;
            }
            currSize++;
            // TODO dupes
        }
        
        V operator [](K k) {
            size_t hashVal = std::hash<K>{}(k);
            int hashBucket = hashVal % 10;
            std::cout << "fetch bucket is " << hashBucket << std::endl;
            std::vector<std::pair<K, V>> list = buckets.at(hashBucket);
            for (std::pair<K, V> p : list) {
                if (k == p.first) { // Equals?
                    return p.second;
                }
            }
            // Found nothing!
            return 0.0;
        }
        
        int size() {
            return currSize;
        }
    private:
        std::vector<std::vector<std::pair<K, V>>> buckets;
        int currSize;
};

template<typename K, typename V>
class HashMap {
private:
    struct Node {
        K key;
        V value;
        std::unique_ptr<Node> next;
        Node(K  key, V value): key(std::move(key)), value(std::move(value)), next(nullptr) {};
    };
    std::vector<std::unique_ptr<Node>> buckets;
    int currSize;
    int getBucket(const K& keyToHash) {
        size_t hash = std::hash<K>{}(keyToHash);
        return hash % buckets.size();
    }
    std::unique_ptr<Node> makeNodeAndIncrement(const K& k, const V& v) {
        auto node = std::make_unique<Node>(k, v);
        currSize++;
        return node;
    }
public:
    HashMap(): buckets(19), currSize(0) {}

    void insert(const std::pair<K, V>& pair) {
        int bucket = getBucket(pair.first);
        auto* nodePtr = buckets[bucket].get();
        if (nodePtr == nullptr) {
            buckets[bucket] = makeNodeAndIncrement(pair.first, pair.second);
            return;
        }
        Node* lastNodePtr = nullptr;
        while (nodePtr != nullptr) {
            // Update
            if (nodePtr->key == pair.first) {
                nodePtr->value = std::move(pair.second);
                return;
            }
            // Iterate
            lastNodePtr = nodePtr;
            nodePtr = nodePtr->next.get();
        }
        // Insert
        lastNodePtr->next = makeNodeAndIncrement(pair.first, pair.second);
    }

    V operator [](const K& k) {
        int bucket = getBucket(k);
        auto* nodePtr = buckets[bucket].get();
        while (nodePtr != nullptr) {
            if (nodePtr->key == k) {
                return nodePtr->value;
            }
            nodePtr = nodePtr->next.get();
        }
        return V();
    }

    int size() const {
        return currSize;
    }

};

int main() {
    HashMap<std::string, double> m;
    std::cout << m.size() << std::endl;
    m.insert(std::make_pair("hello", 23));
    m.insert(std::make_pair("world", 46));
    std::cout << m.size() << std::endl;
    std::cout << "hello: " << m["hello"] << std::endl;
    std::cout << "world: " << m["world"] << std::endl;
    std::cout << "undef: " << m["undef"] << std::endl;
    m.insert(std::make_pair("hello", 24));
    std::cout << "hello: " << m["hello"] << std::endl;
    std::cout << "world: " << m["world"] << std::endl;
    std::cout << "undef: " << m["undef"] << std::endl;
    return 0;
}
