#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "project/PriorityQueue.hpp"
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <memory>
#include <random>
TEST_CASE("Empty priority queue"){AdaptableHeapPriorityQueue<int,std::string> pq;REQUIRE(pq.empty());REQUIRE(pq.size()==0);REQUIRE_THROWS_AS(pq.min(),std::out_of_range);REQUIRE_THROWS_AS(pq.removeMin(),std::logic_error);}
TEST_CASE("Insert and minimum"){AdaptableHeapPriorityQueue<int,std::string> pq;pq.insert(7,"seven");pq.insert(2,"two");pq.insert(5,"five");pq.insert(1,"one");REQUIRE(pq.size()==4);REQUIRE(pq.min().key==1);REQUIRE(pq.min().value=="one");REQUIRE(pq.validHeap());}
TEST_CASE("removeMin returns priority order"){AdaptableHeapPriorityQueue<int,std::string> pq;pq.insert(4,"four");pq.insert(1,"one");pq.insert(3,"three");pq.insert(2,"two");REQUIRE(pq.removeMin().key==1);REQUIRE(pq.removeMin().key==2);REQUIRE(pq.removeMin().key==3);REQUIRE(pq.removeMin().key==4);REQUIRE(pq.empty());}
TEST_CASE("Adaptable remove deletes arbitrary entry"){AdaptableHeapPriorityQueue<int,std::string> pq;auto a=pq.insert(10,"a");auto b=pq.insert(2,"b");auto c=pq.insert(7,"c");auto d=pq.insert(4,"d");pq.remove(c);REQUIRE(pq.size()==3);REQUIRE(pq.validHeap());REQUIRE(pq.removeMin().key==2);REQUIRE(pq.removeMin().key==4);REQUIRE(pq.removeMin().key==10);(void)a;(void)b;(void)d;}
TEST_CASE("replaceKey restores heap order"){AdaptableHeapPriorityQueue<int,std::string> pq;auto a=pq.insert(10,"a");pq.insert(3,"b");pq.insert(7,"c");pq.replaceKey(a,1);REQUIRE(pq.min().key==1);REQUIRE(pq.validHeap());pq.replaceKey(a,20);REQUIRE(pq.min().key==3);REQUIRE(pq.validHeap());}
TEST_CASE("replaceValue changes only value"){AdaptableHeapPriorityQueue<int,std::string> pq;auto e=pq.insert(4,"old");pq.replaceValue(e,"new");REQUIRE(pq.min().key==4);REQUIRE(pq.min().value=="new");}
TEST_CASE("Bottom-up buildHeap"){AdaptableHeapPriorityQueue<int,std::string> pq;pq.buildHeap({{9,"nine"},{4,"four"},{7,"seven"},{1,"one"},{3,"three"},{8,"eight"},{2,"two"}});REQUIRE(pq.size()==7);REQUIRE(pq.min().key==1);REQUIRE(pq.validHeap());std::vector<int> keys;while(!pq.empty())keys.push_back(pq.removeMin().key);REQUIRE(keys==std::vector<int>{1,2,3,4,7,8,9});}
TEST_CASE("Heap sort"){std::vector<int> v{10,3,7,1,9,2,8,6,4,5};heapSort(v);REQUIRE(v==std::vector<int>{1,2,3,4,5,6,7,8,9,10});}
TEST_CASE("Heap sort duplicates and empty"){std::vector<int> v{4,1,4,2,1,3,3};heapSort(v);REQUIRE(v==std::vector<int>{1,1,2,3,3,4,4});std::vector<int> e;heapSort(e);REQUIRE(e.empty());}
TEST_CASE("Move construction transfers ownership"){AdaptableHeapPriorityQueue<int,std::string> a;a.insert(8,"eight");a.insert(2,"two");AdaptableHeapPriorityQueue<int,std::string> b(std::move(a));REQUIRE(b.size()==2);REQUIRE(b.min().key==2);REQUIRE(b.validHeap());REQUIRE(a.empty());}
TEST_CASE("Stale handles are rejected"){AdaptableHeapPriorityQueue<int,std::string> pq;auto e=pq.insert(4,"x");pq.remove(e);REQUIRE_THROWS_AS(pq.remove(e),std::invalid_argument);}

TEST_CASE("Null, foreign, and removed handles are rejected by every update") {
    using Queue = AdaptableHeapPriorityQueue<int, int>;
    Queue queue;
    Queue other;
    auto removed = queue.insert(1, 10);
    auto foreign = other.insert(2, 20);
    Queue::Entry fabricated{3, 30, 0, 1};
    queue.remove(removed);
    auto current = queue.insert(4, 40);
    REQUIRE(current != removed);
    for (Queue::EntryPtr handle : {Queue::EntryPtr{nullptr}, removed, foreign, &fabricated}) {
        REQUIRE_THROWS_AS(queue.remove(handle), std::invalid_argument);
        REQUIRE_THROWS_AS(queue.replaceKey(handle, 0), std::invalid_argument);
        REQUIRE_THROWS_AS(queue.replaceValue(handle, 0), std::invalid_argument);
    }
    REQUIRE(queue.size() == 1);
    REQUIRE(queue.min().value == 40);
    queue.removeMin();
    REQUIRE_THROWS_AS(queue.remove(current), std::invalid_argument);
    REQUIRE_THROWS_AS(queue.min(), std::out_of_range);
    REQUIRE_THROWS_AS(queue.removeMin(), std::logic_error);
}

TEST_CASE("Arbitrary removal can bubble the last entry upward or downward") {
    AdaptableHeapPriorityQueue<int, int> queue;
    std::vector<decltype(queue)::EntryPtr> handles;
    for (int key : {1, 10, 2, 11, 12, 3, 4}) {
        handles.push_back(queue.insert(key, key));
    }
    queue.remove(handles[4]);
    REQUIRE(queue.validHeap());
    REQUIRE(handles[6]->index == 1);
    queue.remove(handles[0]);
    REQUIRE(queue.validHeap());
    for (int key : {2, 3, 4, 10, 11}) {
        REQUIRE(queue.removeMin().key == key);
    }
    REQUIRE(queue.empty());
}

TEST_CASE("Heap construction replaces contents and invalidates old handles") {
    AdaptableHeapPriorityQueue<int, int> queue;
    auto old = queue.insert(1, 10);
    queue.buildHeap({{4, 40}, {2, 20}, {2, 21}});
    REQUIRE(queue.size() == 3);
    REQUIRE(queue.validHeap());
    REQUIRE_THROWS_AS(queue.replaceKey(old, 0), std::invalid_argument);
    REQUIRE(queue.removeMin().key == 2);
    REQUIRE(queue.removeMin().key == 2);
    REQUIRE(queue.removeMin().key == 4);
    queue.buildHeap({});
    REQUIRE(queue.empty());
    REQUIRE(queue.validHeap());
    auto single = queue.insert(7, 70);
    REQUIRE(single->id > old->id);
    REQUIRE_THROWS_AS(queue.remove(old), std::invalid_argument);
    queue.buildHeap({{8, 80}});
    REQUIRE(queue.min().key == 8);
    REQUIRE_THROWS_AS(queue.remove(single), std::invalid_argument);
}

TEST_CASE("Custom comparator supports a max-priority queue") {
    AdaptableHeapPriorityQueue<int, int, std::greater<int>> queue;
    auto handle = queue.insert(2, 20);
    queue.insert(7, 70);
    queue.insert(4, 40);
    REQUIRE(queue.min().key == 7);
    queue.replaceKey(handle, 9);
    REQUIRE(queue.min().key == 9);
    queue.replaceKey(handle, 1);
    REQUIRE(queue.validHeap());
    REQUIRE(queue.removeMin().key == 7);
    queue.buildHeap({{3, 30}, {8, 80}, {5, 50}});
    for (int key : {8, 5, 3}) {
        REQUIRE(queue.removeMin().key == key);
    }
}

TEST_CASE("Handles follow ownership through moves and moved-from queues work") {
    AdaptableHeapPriorityQueue<int, int> source;
    auto handle = source.insert(3, 30);
    source.insert(5, 50);
    AdaptableHeapPriorityQueue<int, int> destination(std::move(source));
    REQUIRE(source.empty());
    REQUIRE_THROWS_AS(source.remove(handle), std::invalid_argument);
    destination.replaceKey(handle, 1);
    REQUIRE(destination.min().value == 30);
    source.insert(9, 90);

    source = std::move(destination);
    REQUIRE(destination.empty());
    REQUIRE_THROWS_AS(destination.replaceValue(handle, 0), std::invalid_argument);
    source.replaceValue(handle, 31);
    REQUIRE(source.min().value == 31);
    REQUIRE(source.validHeap());
    auto& sameQueue = source;
    source = std::move(sameQueue);
    REQUIRE(source.size() == 2);
    REQUIRE(source.validHeap());
    REQUIRE(destination.insert(8, 80) != nullptr);
}

TEST_CASE("Mixed adaptable operations preserve handles and sorted priorities") {
    AdaptableHeapPriorityQueue<int, int> queue;
    std::vector<decltype(queue)::EntryPtr> handles(512);
    std::vector<int> order;
    for (int key = 0; key < 512; ++key) {
        order.push_back(key);
    }
    std::mt19937 random(42);
    std::shuffle(order.begin(), order.end(), random);
    for (int key : order) {
        handles[key] = queue.insert(key, key);
    }
    std::vector<int> expected;
    for (int key = 0; key < 512; ++key) {
        if (key % 3 == 0) {
            queue.remove(handles[key]);
        } else {
            const int priority = (key * 37) % 101 - 50;
            queue.replaceKey(handles[key], priority);
            queue.replaceValue(handles[key], -key);
            REQUIRE(handles[key]->value == -key);
            expected.push_back(priority);
        }
        REQUIRE(queue.validHeap());
    }
    std::sort(expected.begin(), expected.end());
    for (int key : expected) {
        REQUIRE(queue.removeMin().key == key);
    }
    REQUIRE(queue.empty());
}

TEST_CASE("Bottom-up construction uses a linear number of comparisons") {
    std::size_t comparisons = 0;
    const auto compare = [&comparisons](int a, int b) {
        ++comparisons;
        return a < b;
    };
    AdaptableHeapPriorityQueue<int, int, decltype(compare)> queue(compare);
    std::vector<std::pair<int, int>> entries;
    for (int key = 4095; key >= 0; --key) {
        entries.emplace_back(key, key);
    }
    queue.buildHeap(std::move(entries));
    REQUIRE(comparisons < 3 * 4096);
    REQUIRE(queue.validHeap());
    for (int key = 0; key < 4096; ++key) {
        REQUIRE(queue.removeMin().key == key);
    }
}

TEST_CASE("Heap sort handles singleton, ordered, reversed, and custom ordering") {
    for (std::vector<int> values : {std::vector<int>{}, {1}, {1, 2}, {2, 1},
                                   {1, 2, 3, 4, 5}, {5, 4, 3, 2, 1},
                                   {4, 4, 4}, {-1, -8, 0, 3, -8}}) {
        auto expected = values;
        std::sort(expected.begin(), expected.end());
        heapSort(values);
        REQUIRE(values == expected);
        std::sort(expected.begin(), expected.end(), std::greater<int>{});
        heapSort(values, std::greater<int>{});
        REQUIRE(values == expected);
    }
    std::vector<std::string> words{"pear", "apple", "orange", "apple"};
    heapSort(words);
    REQUIRE(words == std::vector<std::string>{"apple", "apple", "orange", "pear"});
}

TEST_CASE("Destruction and move assignment release all owned entries once") {
    std::weak_ptr<int> removedValue;
    std::weak_ptr<int> liveValue;
    std::weak_ptr<int> replacedValue;
    {
        AdaptableHeapPriorityQueue<int, std::shared_ptr<int>> source;
        auto value = std::make_shared<int>(1);
        removedValue = value;
        auto removed = source.insert(1, value);
        value.reset();
        source.remove(removed);

        value = std::make_shared<int>(2);
        liveValue = value;
        source.insert(2, value);
        value.reset();

        AdaptableHeapPriorityQueue<int, std::shared_ptr<int>> destination;
        value = std::make_shared<int>(3);
        replacedValue = value;
        destination.insert(3, value);
        value.reset();
        destination = std::move(source);
        REQUIRE(replacedValue.expired());
        REQUIRE_FALSE(liveValue.expired());
    }
    REQUIRE(removedValue.expired());
    REQUIRE(liveValue.expired());
}
