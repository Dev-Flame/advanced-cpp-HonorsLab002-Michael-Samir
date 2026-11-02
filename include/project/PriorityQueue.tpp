#pragma once
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K,V,Compare>::AdaptableHeapPriorityQueue()
    : heap_{}, owned_{}, comp_{}, nextId_{1} {}

template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K,V,Compare>::AdaptableHeapPriorityQueue(Compare comp)
    : heap_{}, owned_{}, comp_{std::move(comp)}, nextId_{1} {}

template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K,V,Compare>::~AdaptableHeapPriorityQueue() {
    for (auto entry : owned_) {
        delete entry;
    }
}

template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K,V,Compare>::AdaptableHeapPriorityQueue(
    AdaptableHeapPriorityQueue&& other) noexcept
    : heap_(std::move(other.heap_)), owned_(std::move(other.owned_)),
      comp_(std::move(other.comp_)), nextId_(other.nextId_) {
    other.heap_.clear();
    other.owned_.clear();
    other.nextId_ = 1;
}

template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K,V,Compare>&
AdaptableHeapPriorityQueue<K,V,Compare>::operator=(
    AdaptableHeapPriorityQueue&& other) noexcept {
    if (this != &other) {
        for (auto entry : owned_) {
            delete entry;
        }
        heap_ = std::move(other.heap_);
        owned_ = std::move(other.owned_);
        comp_ = std::move(other.comp_);
        nextId_ = other.nextId_;
        other.heap_.clear();
        other.owned_.clear();
        other.nextId_ = 1;
    }
    return *this;
}

template <typename K, typename V, typename Compare>
std::size_t AdaptableHeapPriorityQueue<K,V,Compare>::size() const noexcept {
    return heap_.size();
}

template <typename K, typename V, typename Compare>
bool AdaptableHeapPriorityQueue<K,V,Compare>::empty() const noexcept {
    return heap_.empty();
}

template <typename K, typename V, typename Compare>
bool AdaptableHeapPriorityQueue<K,V,Compare>::less(EntryPtr a, EntryPtr b) const {
    return comp_(a->key, b->key);
}

template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::swapEntries(
    std::size_t i, std::size_t j) noexcept {
    std::swap(heap_[i], heap_[j]);
    heap_[i]->index = i;
    heap_[j]->index = j;
}

template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::upHeap(std::size_t index) {
    while (index > 0) {
        const std::size_t parent = (index - 1) / 2;
        if (!less(heap_[index], heap_[parent])) {
            break;
        }
        swapEntries(index, parent);
        index = parent;
    }
}

template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::downHeap(std::size_t index) {
    while (index < heap_.size() / 2) {
        std::size_t child = 2 * index + 1;
        const std::size_t right = child + 1;
        if (right < heap_.size() && less(heap_[right], heap_[child])) {
            child = right;
        }
        if (!less(heap_[child], heap_[index])) {
            break;
        }
        swapEntries(index, child);
        index = child;
    }
}

template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::bubble(std::size_t index) {
    if (index > 0 && less(heap_[index], heap_[(index - 1) / 2])) {
        upHeap(index);
    } else {
        downHeap(index);
    }
}

template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::validateHandle(EntryPtr entry) const {
    // Check ownership before reading any fields of an unknown pointer.
    if (!entry || owned_.find(entry) == owned_.end() ||
        entry->index >= heap_.size() || heap_[entry->index] != entry) {
        throw std::invalid_argument("Invalid or stale priority-queue entry handle");
    }
}

template <typename K, typename V, typename Compare>
typename AdaptableHeapPriorityQueue<K,V,Compare>::EntryPtr
AdaptableHeapPriorityQueue<K,V,Compare>::insert(const K& key, const V& value) {
    std::unique_ptr<Entry> entry(new Entry{key, value, heap_.size(), nextId_});
    heap_.push_back(entry.get());
    try {
        owned_.insert(entry.get());
    } catch (...) {
        heap_.pop_back();
        throw;
    }
    EntryPtr handle = entry.release();
    ++nextId_;
    upHeap(handle->index);
    return handle;
}

template <typename K, typename V, typename Compare>
const typename AdaptableHeapPriorityQueue<K,V,Compare>::Entry&
AdaptableHeapPriorityQueue<K,V,Compare>::min() const {
    if (empty()) {
        throw std::out_of_range("min() on empty priority queue");
    }
    return *heap_.front();
}

template <typename K, typename V, typename Compare>
typename AdaptableHeapPriorityQueue<K,V,Compare>::Entry
AdaptableHeapPriorityQueue<K,V,Compare>::removeMin() {
    if (empty()) {
        throw std::logic_error("removeMin() on empty priority queue");
    }
    Entry result = *heap_.front();
    remove(heap_.front());
    return result;
}

template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::remove(EntryPtr entry) {
    validateHandle(entry);
    const std::size_t index = entry->index;
    swapEntries(index, heap_.size() - 1);
    heap_.pop_back();
    entry->index = std::numeric_limits<std::size_t>::max();
    if (index < heap_.size()) {
        bubble(index);
    }
    // Retain the allocation as an invalid handle; the destructor frees it once.
    // This costs O(total entries created) storage over the queue's lifetime.
}

template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::replaceKey(
    EntryPtr entry, const K& key) {
    validateHandle(entry);
    entry->key = key;
    bubble(entry->index);
}

template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::replaceValue(
    EntryPtr entry, const V& value) {
    validateHandle(entry);
    entry->value = value;
}

template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::buildHeap(
    std::vector<std::pair<K,V>> entries) {
    AdaptableHeapPriorityQueue replacement(comp_);
    replacement.nextId_ = nextId_;
    replacement.heap_.reserve(entries.size());
    replacement.owned_.reserve(entries.size());
    for (const auto& item : entries) {
        std::unique_ptr<Entry> entry(new Entry{
            item.first, item.second, replacement.heap_.size(), replacement.nextId_});
        replacement.owned_.insert(entry.get());
        replacement.heap_.push_back(entry.release());
        ++replacement.nextId_;
    }

    // Bottom-up heap construction is O(n): most nodes are near the leaves.
    for (std::size_t index = replacement.size() / 2; index > 0; --index) {
        replacement.downHeap(index - 1);
    }
    owned_.merge(replacement.owned_);
    for (auto entry : heap_) {
        entry->index = std::numeric_limits<std::size_t>::max();
    }
    heap_.swap(replacement.heap_);
    nextId_ = replacement.nextId_;
}

template <typename K, typename V, typename Compare>
bool AdaptableHeapPriorityQueue<K,V,Compare>::validHeap() const noexcept {
    for (std::size_t index = 0; index < heap_.size(); ++index) {
        if (heap_[index]->index != index ||
            (index > 0 && less(heap_[index], heap_[(index - 1) / 2]))) {
            return false;
        }
    }
    return true;
}

template <typename T, typename Compare>
void heapSort(std::vector<T>& values, Compare comp) {
    // Build a max-heap relative to comp, then move each maximum to the end.
    // Iterative sifting keeps auxiliary space O(1).
    const auto siftDown = [&](std::size_t root, std::size_t count) {
        while (root < count / 2) {
            std::size_t child = 2 * root + 1;
            if (child + 1 < count && comp(values[child], values[child + 1])) {
                ++child;
            }
            if (!comp(values[root], values[child])) {
                break;
            }
            std::swap(values[root], values[child]);
            root = child;
        }
    };
    for (std::size_t index = values.size() / 2; index > 0; --index) {
        siftDown(index - 1, values.size());
    }
    for (std::size_t count = values.size(); count > 1; --count) {
        std::swap(values[0], values[count - 1]);
        siftDown(0, count - 1);
    }
}
