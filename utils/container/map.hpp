#pragma once

#include <functional>
#include <shared_mutex>
#include <unordered_map>

namespace xlab {

template <typename K, typename V>
class _Map_Template {
public:
    using key_type = K;
    using mapped_type = V;
    using value_type = std::pair<const key_type, mapped_type>;
    using reference = value_type&;
    using const_reference = const value_type&;

private:
    class iterator {
        friend class _Map_Template<K, V>;

    private:
        using InnerIteratorType = typename std::unordered_map<K, V>::iterator;
        using InnerIteratorPointeeType = typename std::unordered_map<K, V>::reference;
        InnerIteratorType _inner_iterator;

    public:
        iterator(InnerIteratorType inner_iterator)
            : _inner_iterator(std::move(inner_iterator))
        {
        }

        InnerIteratorPointeeType operator*() const { return *_inner_iterator; }

        const iterator& operator++()
        {
            ++_inner_iterator;
            return *this;
        }

        iterator operator++(int)
        {
            iterator copy(*this);
            ++_inner_iterator;
            return copy;
        }

        bool operator==(const iterator& other) const
        {
            return _inner_iterator == other._inner_iterator;
        }

        bool operator!=(const iterator& other) const
        {
            return _inner_iterator != other._inner_iterator;
        }
    };

private:
    iterator begin() { return iterator(_umap.begin()); }

    iterator end() { return iterator(_umap.end()); }

private:
    std::unordered_map<K, V> _umap;
    std::shared_mutex mMutex;

public:
    std::unordered_map<K, V>& InnerMap()
    {
        return _umap;
    }

    void Set(const K& key, const V& value)
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        _umap[key] = value;
    }

    void Set(const K& key, V&& value)
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        _umap[key] = std::move(value);
    }

    bool Find(const K& key)
    {
        std::shared_lock<decltype(mMutex)> locker(mMutex);
        return _umap.find(key) != _umap.end();
    }

    void Erase(const K& key)
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        _umap.erase(key);
    }

    void Clear()
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        _umap.clear();
    }

    size_t Size()
    {
        return _umap.size();
    }

    V Get(const K& key)
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        return _umap[key];
    }

    // V &operator[](const K &key) {
    //     std::lock_guard<decltype(mMutex)> locker(mMutex);
    //     return _umap[key];
    // }

    void Enumerate(std::function<void(const K&, V&)> func)
    {
        std::shared_lock<decltype(mMutex)> locker(mMutex);
        for (auto& item : *this) {
            func(item.first, item.second);
        }
    }
};

template <typename K, typename V>
using Map = _Map_Template<K, V>;

}
