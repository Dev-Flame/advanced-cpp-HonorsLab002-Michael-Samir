#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "project/PriorityQueue.hpp"
#include <string>
#include <vector>
#include <utility>
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
