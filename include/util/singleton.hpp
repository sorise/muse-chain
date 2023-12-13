//
// Created by 14270 on 2024-01-02.
//

#ifndef GLORIA_SINGLETON_HPP
#define GLORIA_SINGLETON_HPP
#include <memory>
#include <memory_resource>

namespace muse::chain{
    template <class T>
    std::shared_ptr<T> Singleton() {
        static std::shared_ptr<T> instance = std::make_shared<T>();
        return instance;
    }

    template<typename T, typename ...Args>
    auto new_by_pool(Args&&... args) ->T*{
        static_assert(std::is_destructible<T>());
        void * place = Singleton<std::pmr::synchronized_pool_resource>()->allocate(sizeof(T));
        T* real = new(place) T(std::forward<Args>(args)...);
        return real;
    }

    template<typename T>
    auto delete_by_pool(T *ptr) ->void{
        static_assert(std::is_destructible<T>());
        ptr->~T();
        Singleton<std::pmr::synchronized_pool_resource>()->deallocate(ptr, sizeof(T));
    }


}

#endif //GLORIA_SINGLETON_HPP
