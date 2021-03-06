#ifndef __MEMORY_MGMT_H__
#define __MEMORY_MGMT_H__

#include "common.h"

template<size_t POOL_SIZE = 4096UL>
class var_memory_pool {
public:
  struct meta_data {
    size_t size;
    int free;
    meta_data* next;
  };

  var_memory_pool()
  {
    free_list_head = reinterpret_cast<meta_data*>(memory);
    free_list_head->size = POOL_SIZE - sizeof(meta_data);
    free_list_head->free = 1;
    free_list_head->next = nullptr;

    std::cout << "pool initialized" << std::endl;
    dump();
  }

  void* alloc(size_t size)
  {
    if (free_list_head == nullptr) {
      return nullptr;
    }

    void* result = nullptr;
    meta_data* curr = free_list_head;
    const size_t required_size = size + sizeof(meta_data);
    while ((curr != nullptr) && ((curr->size < required_size) || (curr->free == 0))) {
      curr = curr->next;
    }

    if (curr != nullptr) {
      if (curr->size == required_size) {
        curr->free = 0;
      } else if (curr->size > required_size) {
        split(curr, size);
      }
      result = static_cast<void*>(curr + 1);
    }

    std::cout << "alloc(" << size << ")" << std::endl;
    dump();
    return result;
  }

  void free(void* ptr)
  {
    meta_data* curr = reinterpret_cast<meta_data*>(ptr);
    curr = curr - 1;
    curr->free = 1;
    merge_regions();

    std::cout << "free(" << ptr << ")" << std::endl;
    dump();
  }

  void dump()
  {
    std::cout << "---\nstart\t\t\tend\t\t\tsize\tfree\n";
    meta_data* curr = free_list_head;
    while (curr != nullptr) {
      uint8_t* end = reinterpret_cast<uint8_t*>(curr) + sizeof(meta_data) + curr->size - 1;
      std::cout <<  curr << "\t"
        << static_cast<void*>(end) << "\t"
        << sizeof(meta_data) << "+" << curr->size << "\t"
        << curr->free << std::endl;
      curr = curr->next;
    }
    std::cout << "---" << std::endl;
  }

private:

  void split(meta_data* fitting_slot, size_t size) {
    assert(fitting_slot->size > size);
    meta_data* free_block = reinterpret_cast<meta_data*>(
      reinterpret_cast<uint8_t*>(fitting_slot) + size + sizeof(meta_data));
    free_block->size = fitting_slot->size - size - sizeof(meta_data);
    assert(free_block->size > 0);
    free_block->free = 1;
    free_block->next = fitting_slot->next;

    fitting_slot->size = size;
    fitting_slot->free = 0;
    fitting_slot->next = free_block;
  }

  void merge_regions()
  {
    meta_data* curr = free_list_head;
    while ((curr != nullptr) && (curr->next != nullptr)) {
      if ((curr->free == 1) && (curr->next->free == 1)) {
        curr->size += sizeof(meta_data) + curr->next->size;
        curr->next = curr->next->next;
      } else {
        curr = curr->next;
      }
    }
  }

  meta_data* free_list_head;
  uint8_t memory[POOL_SIZE];
};

template<class T, size_t POOL_SIZE = 16384UL>
class fixed_memory_pool {
public:

  union slot {
    uint8_t block[sizeof(T)];
    slot* next;
  };

  fixed_memory_pool() :
    free_list_head(pool_)
  {
    for (size_t i = 0; i < SLOT_COUNT - 1; ++i) {
      pool_[i].next = pool_ + i + 1;
    }

    pool_[SLOT_COUNT - 1].next = nullptr;
  }

  void* alloc()
  {
    if (free_list_head == nullptr)
      return nullptr;

    slot* new_slot = free_list_head;
    free_list_head = free_list_head->next;
    return new_slot->block;
  }

  void free(void* mem)
  {
    slot* free_slot = reinterpret_cast<slot*>(mem);
    free_slot->next = free_list_head;
    free_list_head = free_slot;
  }

private:
  static const size_t SLOT_COUNT = (POOL_SIZE + sizeof(slot)) / sizeof(slot);
  slot* free_list_head;
  slot pool_[SLOT_COUNT];
};

template<class T>
class list_node {
public:
  list_node(T v) :
    val(v),
    next(nullptr)
  {}

  static void* operator new(size_t sz);
  static void operator delete(void* ptr);

  T val;
  list_node* next;
};

fixed_memory_pool<list_node<int>> list_node_pool;
var_memory_pool<> list_node_var_pool;

template<class T>
void* list_node<T>::operator new(size_t sz)
{
  //return list_node_pool.alloc();
  return list_node_var_pool.alloc(sz);
}

template<class T>
void list_node<T>::operator delete(void* ptr)
{
  //list_node_pool.free(ptr);
  list_node_var_pool.free(ptr);
}

template<typename T>
class SmartPtr {
public:
  explicit SmartPtr(T *obj) :
    inner_(new Inner(obj))
  {
    inner_->AddRef();
  }

  SmartPtr(const SmartPtr &other)
  {
    inner_ = other.inner_;
    inner_->AddRef();
  }

  ~SmartPtr()
  {
    if (inner_->Release() == 0) {
      delete inner_;
    }
  }

  SmartPtr& operator=(const SmartPtr &other)
  {
    if (this == &other) {
      return *this;
    }

    if (inner_->Release() == 0) {
      delete inner_;
    }
    inner_ = other.inner_;
    inner_->AddRef();

    return *this;
  }

  T& operator*()
  {
    return *Get();
  }

  T* operator->()
  {
    return Get();
  }

  T* Get()
  {
    return inner_->Get();
  }

private:
  class Inner {
  public:
    explicit Inner(T *obj) :
      ref_count_(0),
      obj_(obj)
    {}

    ~Inner()
    {
      delete obj_;
    }

    void AddRef()
    {
      ref_count_++;
    }

    int Release()
    {
      if (ref_count_ > 0) {
        ref_count_--;
        delete obj_;
        obj_ = nullptr;
      }
      return ref_count_;
    }

    T* Get()
    {
        return obj_;
    }

  private:
    int ref_count_;
    T* obj_;
  };

  Inner *inner_;
};

#endif // __MEMORY_MGMT_H__
