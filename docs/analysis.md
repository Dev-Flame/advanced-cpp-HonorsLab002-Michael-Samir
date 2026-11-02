# Lab 22 — Complexity Analysis

## 1. Heap Height
Explain why a complete binary heap containing `n` entries has height `O(log n)`.

## 2. Priority Queue Operations
Analyze `insert`, `min`, and `removeMin`, explicitly relating bubbling to heap height.

## 3. Adaptable Operations
Explain why a handle containing the current array index permits `remove(entry)` and `replaceKey(entry,key)` in `O(log n)`, while scanning for an entry would not meet the specification.

## 4. Bottom-Up Construction
Explain why repeated insertion is `O(n log n)` in the worst case, while bottom-up construction is `O(n)`. Give an argument based on subtree heights rather than only stating the result.

## 5. Heap Sort
State and justify time complexity, auxiliary-space complexity, and whether your implementation is in place.

## 6. Design Reflection
Compare a heap-based priority queue with unsorted-list and sorted-list implementations when both insertion and minimum removal need to be efficient.
