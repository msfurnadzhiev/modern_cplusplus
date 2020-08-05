#include <iostream>
#include <type_traits>

namespace UniquePointer {

template<typename T>
struct default_deleter {
  void operator()(T *ptr) const{
    delete ptr;
  }
};

template<typename T, typename Deleter = default_deleter<T>>
class unique_ptr {

  using pointer = T*;
  using element_type = T;
  using deleter_type = Deleter;

  pointer ptr_resource = pointer();
  deleter_type operator_delete = deleter_type();

 public:
  unique_ptr() = default;

  explicit unique_ptr(pointer raw_resource) : ptr_resource(std::move(raw_resource)) {};

  unique_ptr(pointer raw_resource, deleter_type deleter) :
      ptr_resource(std::move(raw_resource)), operator_delete(deleter) {}

  ~unique_ptr() {
    if (ptr_resource)
      operator_delete(ptr_resource);
  }

  unique_ptr(const  unique_ptr&) = delete;

  unique_ptr& operator=(const  unique_ptr&) = delete;

  unique_ptr(unique_ptr&& other)  noexcept {
    ptr_resource = other.ptr_resource;
    operator_delete = other.operator_delete;

    other.ptr_resource = pointer();
  }

  unique_ptr& operator=(unique_ptr&& other)  noexcept {
    if(ptr_resource)
      operator_delete(ptr_resource);
    ptr_resource = other.ptr_resource;
    operator_delete = other.operator_delete;

    other.ptr_resource = pointer();

  }


  pointer get() const {
    return ptr_resource;
  }

  deleter_type  get_deleter() const {
    return operator_delete;
  }

  element_type& operator*() const {
    return *ptr_resource;
  }

  pointer operator->() const {
    return ptr_resource;
  }

  void reset(pointer new_resource = pointer()) {
    if(ptr_resource)
      operator_delete(ptr_resource);
    ptr_resource = new_resource;
  }

  pointer release() {
    pointer  old_ptr = ptr_resource;
    ptr_resource = pointer();
    return old_ptr;
  }

  void swap(unique_ptr& other) {
    std::swap(ptr_resource, other.ptr_resource);
    std::swap(operator_delete, other.operator_delete);
  }
};

}



int main() {

  //Examples...

  UniquePointer::unique_ptr<int> ptr(new int);

  UniquePointer::unique_ptr<int> ptr_arr(new int[42]);

  UniquePointer::unique_ptr<int, std::add_pointer_t<decltype(free)>>
      ptr_m_1((int*)malloc(sizeof(int)), free);

  auto call_free = [](int* ptr) {
    free(ptr);
  };

  UniquePointer::unique_ptr<int, decltype(call_free)>
      ptr_m_2((int*)malloc(sizeof(int)), call_free);

  return 0;
}

