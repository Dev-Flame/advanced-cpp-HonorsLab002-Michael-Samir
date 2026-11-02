# Lab 22 — Priority Queues; Advanced Heap Structures

## Topic
**7.3 Module 6 — Priority Queues; Advanced Heap Structures**

## Overview
Implement an **adaptable priority queue backed by a binary min-heap** and use heap principles to implement **heap sort**.

This lab extends the basic priority-queue ADT with entry handles. A handle allows an arbitrary entry to be removed or modified without scanning the heap. The implementation must use a vector representation of a complete binary tree.

## Learning Objectives
1. Implement `insert`, `min`, and `removeMin` with a binary heap.
2. Maintain heap order with up-heap and down-heap bubbling.
3. Implement adaptable `remove` and `replaceKey` operations using stable handles.
4. Implement bottom-up heap construction in `O(n)`.
5. Implement in-place heap sort in `O(n log n)`.
6. Reason about ownership, stale handles, move semantics, and complexity.

## Required Complexity
| Operation | Complexity |
|---|---:|
| `size`, `empty` | `O(1)` |
| `min` | `O(1)` |
| `insert` | `O(log n)` |
| `removeMin` | `O(log n)` |
| `remove(entry)` | `O(log n)` |
| `replaceKey(entry,key)` | `O(log n)` |
| `replaceValue(entry,value)` | `O(1)` |
| `buildHeap` | `O(n)` |
| `heapSort` | `O(n log n)` |

A correct output produced with a slower asymptotic algorithm does **not** satisfy the assignment.

## Student Work
Implement every `TODO` in:

```text
include/project/PriorityQueue.tpp
```

Do not change the public method signatures in `PriorityQueue.hpp`.

## Entry Handles
`insert()` returns an `EntryPtr`. The entry stores its current array index. Every heap swap must update both affected indices. A stale or invalid handle must cause `std::invalid_argument`.

## Bottom-Up Heap Construction
`buildHeap()` must not repeatedly call `insert()`. Create all entries and restore heap order from the last internal node toward the root using down-heap bubbling. The goal is the linear-time bottom-up construction discussed with heap structures.

## Heap Sort
Implement `heapSort(std::vector<T>&, Compare)` in place. With the default comparator, the result must be ascending.

## Error Handling
- `min()` on an empty queue: `std::out_of_range`.
- `removeMin()` on an empty queue: `std::logic_error`.
- Invalid/stale handle: `std::invalid_argument`.

## Memory Management
The queue owns its dynamically allocated entries. Release each entry exactly once. Move construction and move assignment must transfer ownership safely and leave the moved-from queue valid and empty.

## Testing
```bash
make
make test
make run
```

Visible tests are necessary but not sufficient. Instructor tests may check large inputs, duplicate keys, custom comparators, stale handles, singleton/empty heaps, move assignment, memory ownership, and complexity.

## Analysis
Complete `docs/analysis.md`, explaining heap height, bubbling complexity, adaptable operations, bottom-up construction, heap sort, and auxiliary space.
