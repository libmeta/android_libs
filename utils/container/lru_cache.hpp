#pragma once

#include <algorithm>
#include <assert.h>
#include <functional>
#include <mutex>
#include <unordered_map>

namespace xlab {

template <typename K, typename V>
struct LRUCache {
private:
    struct Node {
        K key;
        V val;
        Node* prev = nullptr;
        Node* next = nullptr;

        Node(K k, V v)
            : key(k)
            , val(v)
        {
        }
    };

    using DeleteElementFunc = std::function<void(V)>;

    size_t _max_size;
    size_t _cur_size = 0;
    DeleteElementFunc _delete_func = nullptr;

    Node* head = nullptr;
    Node* tail = nullptr;

    std::unordered_map<K, Node*> _map;
    std::mutex _mutex;

public:
    LRUCache(size_t max_size = 10, DeleteElementFunc delete_func = nullptr) noexcept
        : _max_size(max_size)
        , _delete_func(std::move(delete_func))
    {
        assert(max_size > 0);
    }

    LRUCache(LRUCache& lru) = delete;

    LRUCache(LRUCache&& lru) noexcept
    {
        std::lock_guard<decltype(_mutex)> locker(lru._mutex);

        head = lru.head;
        tail = lru.tail;
        _max_size = lru._max_size;
        _cur_size = lru._cur_size;
        _map = std::move(lru._map);
        _delete_func = lru._delete_func;

        lru.head = nullptr;
        lru.tail = nullptr;
        lru._cur_size = 0;
        lru._map.clear();
    }

    ~LRUCache()
    {
        ClearWithoutLock();
    }

    LRUCache& operator=(LRUCache& lru) = delete;

    LRUCache& operator=(LRUCache&& lru)
    {
        std::lock_guard<decltype(_mutex)> locker1(_mutex);
        std::lock_guard<decltype(lru._mutex)> locker2(lru._mutex);

        ClearWithoutLock();

        head = lru.head;
        tail = lru.tail;
        _max_size = lru._max_size;
        _cur_size = lru._cur_size;
        _map = std::move(lru._map);
        _delete_func = lru._delete_func;

        lru.head = nullptr;
        lru.tail = nullptr;
        lru._cur_size = 0;
        lru._map.clear();
    }

private:
    inline void RemoveNodeWithoutLock(Node* node)
    {
        if (node == nullptr) {
            return;
        }
        if (_delete_func != nullptr) {
            _delete_func(node->val);
        }
        _map.erase(node->key);
        delete node;
    }

    inline void MoveToHeadWithoutLock(Node* node)
    {
        if (node == nullptr) {
            return;
        }

        if (node == head) {
            return;
        }

        if (node == tail) {
            tail = node->prev;
            tail->next = nullptr;
            node->prev = nullptr;
            node->next = head;
            head->prev = node;
            head = node;
            return;
        }

        node->prev->next = node->next;
        node->next->prev = node->prev;
        node->prev = nullptr;
        node->next = head;
        head->prev = node;
        head = node;
    }

    inline void ClearWithoutLock()
    {
        auto node = head;
        while (node != nullptr) {
            auto next = node->next;
            RemoveNodeWithoutLock(node);
            node = next;
        }
        _map.clear();
        head = nullptr;
        tail = nullptr;
        _cur_size = 0;
    }

public:
    void Clear()
    {
        std::lock_guard<decltype(_mutex)> locker(_mutex);
        ClearWithoutLock();
    }

    void Put(const K& key, V& val)
    {
        std::lock_guard<decltype(_mutex)> locker(_mutex);
        if (_map.find(key) != _map.end()) {
            Node* node = _map[key];
            if (_delete_func != nullptr) {
                _delete_func(node->val);
            }
            node->val = val;
            MoveToHeadWithoutLock(node);
            return;
        }

        while (_cur_size >= _max_size) {
            auto last = tail;
            if (tail == nullptr) {
                throw std::runtime_error("some error");
            }
            tail = tail->prev;
            if (tail != nullptr) {
                tail->next = nullptr;
            }
            RemoveNodeWithoutLock(last);
            _cur_size -= 1;
        }

        Node* node = new Node(key, val);
        if (_cur_size == 0) {
            head = node;
            tail = node;
        } else {
            head->prev = node;
            node->next = head;
            head = node;
        }
        _map[key] = node;
        _cur_size += 1;
    }

    bool Find(const K& key)
    {
        std::lock_guard<decltype(_mutex)> locker(_mutex);
        return _map.find(key) != _map.end();
    }

    V& Get(const K& key)
    {
        std::lock_guard<decltype(_mutex)> locker(_mutex);
        if (_map.find(key) == _map.end()) {
            throw std::runtime_error("no this key");
        }
        Node* node = _map[key];
        MoveToHeadWithoutLock(node);
        return node->val;
    }

    bool TryGet(const K& key, V& val)
    {
        std::lock_guard<decltype(_mutex)> locker(_mutex);
        if (_map.find(key) == _map.end()) {
            return false;
        }
        Node* node = _map[key];
        MoveToHeadWithoutLock(node);
        val = node->val;
        return true;
    }
};

}
