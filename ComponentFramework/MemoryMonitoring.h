#include <iostream>
#include <cstdlib>
#include <new>
#include <algorithm>

class MemoryMonitor {
public:
    // Static variables to track memory usage
    static size_t totalAllocations;
    static size_t totalDeallocations;
    static size_t totalMemoryAllocated;
    static size_t totalMemoryDeallocated;
    static size_t peakMemoryUsage;

    // Overload 'new' operator to track allocations
    static void* operator new(size_t size) {
        totalAllocations++;                 // Increment allocation count
        totalMemoryAllocated += size;       // Add to total allocated memory
        peakMemoryUsage = std::max(peakMemoryUsage, totalMemoryAllocated);  // Track peak memory usage
        return ::operator new(size);        // Call the global new operator
    }

    // Overload 'delete' operator to track deallocations
    static void operator delete(void* pointer, size_t size) {
        totalDeallocations++;               // Increment deallocation count
        totalMemoryDeallocated += size;     // Add to total deallocated memory
        totalMemoryAllocated -= size;       // Decrease current allocated memory
        ::operator delete(pointer);         // Call the global delete operator
    }

    // Function to print the memory statistics
    static void printStats() {
        std::cout << "Allocations: " << totalAllocations << std::endl;
        std::cout << "Deallocations: " << totalDeallocations << std::endl;
        std::cout << "Total Memory Allocated: " << totalMemoryAllocated << " bytes" << std::endl;
        std::cout << "Total Memory Deallocated: " << totalMemoryDeallocated << " bytes" << std::endl;
        std::cout << "Current Memory Usage: " << totalMemoryAllocated << " bytes" << std::endl;
        std::cout << "Peak Memory Usage: " << peakMemoryUsage << " bytes" << std::endl;
        if (totalAllocations > totalDeallocations) {
            std::cout << "Warning: Potential memory leak detected!" << std::endl;
        }
    }
};

// Initialize static members
size_t MemoryMonitor::totalAllocations = 0;
size_t MemoryMonitor::totalDeallocations = 0;
size_t MemoryMonitor::totalMemoryAllocated = 0;
size_t MemoryMonitor::totalMemoryDeallocated = 0;
size_t MemoryMonitor::peakMemoryUsage = 0;

//// Example usage
//int main() {
//    // Example allocation and deallocation
//    int* arr1 = new int[100];  // Allocate memory for 100 integers
//    int* arr2 = new int[50];   // Allocate memory for 50 integers
//
//    delete[] arr1;             // Deallocate memory for 100 integers
//    delete[] arr2;             // Deallocate memory for 50 integers
//
//    // Print memory statistics
//    MemoryMonitor::printStats();
//
//    // Optionally, allocate more memory to see peak usage
//    double* arr3 = new double[1000]; // Allocate memory for 1000 doubles
//    delete[] arr3;                  // Deallocate memory for 1000 doubles
//
//    // Print memory statistics again
//    MemoryMonitor::printStats();
//
//    return 0;
//}