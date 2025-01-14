#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <cmath>

const size_t MAX_POW = 10;
const size_t MAX_BLOCK = 1024;

const size_t MIN_POW = 5;
const size_t MIN_BLOCK = 32;

const size_t PAGE_SIZE = 4096;

std::vector<size_t> pwrs = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

struct Page{
    size_t _size;
    void* _page_start;
};

class Allocator{
    private:
    std::vector<Page> _pages;
    std::map<size_t, std::vector<void*>> _free_blocks;

    public:
    Allocator(size_t mem){
        while(mem >= PAGE_SIZE){
            Page tmp;
            tmp._size = 0; // == free page
            tmp._page_start = (void*)malloc(PAGE_SIZE);
            if(tmp._page_start == NULL) {std::cerr << "malloc: error" << std::endl; exit(1);}
            else _pages.push_back(tmp);
            // std::cout << tmp._page_start << std::endl;
            mem -= PAGE_SIZE;
        }
        // std::cout << _pages.size() << "\n";
    }

    void* alloc(size_t block_size){
        if(block_size > PAGE_SIZE){std::cerr << "alloc: to_do" << std::endl; return NULL;}
        void* res = NULL;
        size_t power = ceil(log2(block_size));
        block_size = pwrs[power];
        // std::cout << block_size << std::endl;
        bool get = false;
        for(auto& elem : _pages){
            if(elem._size == block_size){
                if(!_free_blocks[block_size].empty()){
                    res = _free_blocks[block_size].back();
                    // std::cout << "Size of list " << block_size << " is " << _free_blocks[block_size].size() << std::endl;
                    _free_blocks[block_size].pop_back();
                    get = true;
                    break;
                }
            }
            else if(elem._size == 0){
                elem._size = block_size;
                void* tmp = elem._page_start;
                size_t count = PAGE_SIZE;
                while(count > block_size){
                    _free_blocks[block_size].push_back(tmp);
                    tmp += block_size;
                    count -= block_size;
                }
                // std::cout << "Size of list " << block_size << " is " << _free_blocks[block_size].size() << std::endl;
                res = tmp;
                get = true;
                break;
            }
        }
        if(!get) {std::cerr << "alloc: no clear pages" << std::endl; exit(3);}
        return res;
    }
    void free(void* block){
        if(block == NULL) return;
        bool get_free = false;
        for(auto& elem : _pages){
            if(block >= elem._page_start and block <= (elem._page_start + PAGE_SIZE)){
                _free_blocks[elem._size].push_back(block);
                get_free = true;
            }
        }
        if(!get_free){
            std::cerr << "No permission to free the 'block'" << std::endl;
        }
    } 
};


// Allocator createMemoryAllocator(size_t memory_size){}
// void* alloc(Allocator allocator, size_t block_size){}
// void free(Allocator allocator, void* block){} 
