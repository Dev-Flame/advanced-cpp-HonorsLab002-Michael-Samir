#include "project/PriorityQueue.hpp"
#include <iostream>
#include <string>
#include <vector>
int main(){
    AdaptableHeapPriorityQueue<int,std::string> pq;
    auto low=pq.insert(5,"medium"); auto urgent=pq.insert(1,"urgent"); auto later=pq.insert(9,"later");
    std::cout<<"Minimum priority: "<<pq.min().key<<" ("<<pq.min().value<<")\n";
    pq.replaceKey(later,0);
    std::cout<<"After replaceKey, minimum priority: "<<pq.min().key<<"\n";
    pq.remove(urgent);
    std::cout<<"After removing urgent entry, size: "<<pq.size()<<"\n";
    std::vector<int> data{9,4,7,1,3,8,2,6,5}; heapSort(data);
    std::cout<<"Heap-sort result: "; for(int x:data)std::cout<<x<<' '; std::cout<<'\n';
    (void)low; return 0;
}
