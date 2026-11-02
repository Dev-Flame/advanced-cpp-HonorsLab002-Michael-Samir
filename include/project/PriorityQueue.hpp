#pragma once
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename K, typename V, typename Compare = std::less<K>>
class AdaptableHeapPriorityQueue {
public:
    struct Entry { K key; V value; std::size_t index; std::size_t id; };
    using EntryPtr = Entry*;
    AdaptableHeapPriorityQueue();
    explicit AdaptableHeapPriorityQueue(Compare comp);
    ~AdaptableHeapPriorityQueue();
    AdaptableHeapPriorityQueue(const AdaptableHeapPriorityQueue&) = delete;
    AdaptableHeapPriorityQueue& operator=(const AdaptableHeapPriorityQueue&) = delete;
    AdaptableHeapPriorityQueue(AdaptableHeapPriorityQueue&& other) noexcept;
    AdaptableHeapPriorityQueue& operator=(AdaptableHeapPriorityQueue&& other) noexcept;
    std::size_t size() const noexcept;
    bool empty() const noexcept;
    EntryPtr insert(const K& key, const V& value);
    const Entry& min() const;
    Entry removeMin();
    void remove(EntryPtr entry);
    void replaceKey(EntryPtr entry, const K& newKey);
    void replaceValue(EntryPtr entry, const V& newValue);
    void buildHeap(std::vector<std::pair<K,V>> entries);
    bool validHeap() const noexcept;
private:
    std::vector<EntryPtr> heap_;
    Compare comp_;
    std::size_t nextId_;
    bool less(EntryPtr a, EntryPtr b) const;
    void swapEntries(std::size_t i, std::size_t j) noexcept;
    void upHeap(std::size_t index);
    void downHeap(std::size_t index);
    void bubble(std::size_t index);
    void validateHandle(EntryPtr entry) const;
};

template <typename T, typename Compare = std::less<T>>
void heapSort(std::vector<T>& values, Compare comp = Compare{});
#include "project/PriorityQueue.tpp"
