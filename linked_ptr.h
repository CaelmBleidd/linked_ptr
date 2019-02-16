//
// Created by Alexey Menshutin on 2019-02-16.
//

#ifndef LINKED_PTR_LINKED_PTR_H
#define LINKED_PTR_LINKED_PTR_H

#include <type_traits>

namespace smart_ptr {
    namespace details {
        struct node {
            node(node *prev, node *next): next(next), prev(prev) {}
            node(): next(nullptr), prev(nullptr) {}

            void insert(node *to_insert)  {
                to_insert->prev = this;
                to_insert->next = next;
                if (next)
                    next->prev = to_insert;
                next = to_insert;
            }

            void remove() {
                if (next)
                    next->prev = prev;
                if (prev)
                    prev->next = next;
                prev = nullptr;
                next = nullptr;
            }

            void swap(node &a) {
                node *first;
                if (a.prev)
                    first = a.prev;
                else
                    first = a.next;

                node *second;
                if (prev)
                    second = prev;
                else
                    second = next;


                remove();
                a.remove();

                if (first)
                    first->insert(this);
                if (second)
                    second->insert(&a);
            }

            mutable node *next = nullptr;
            mutable node *prev = nullptr;
        };
    }

    using namespace details;

    template<typename T>
    class linked_ptr {
        template<typename U>
        friend class linked_ptr;

    public:
        constexpr linked_ptr() noexcept : node_in_list(), pointer(nullptr) {} // default

        explicit linked_ptr(T *pointer) noexcept : node_in_list(), pointer(pointer) {} // from T*

        linked_ptr(linked_ptr const &copy) : node_in_list(), pointer(copy.get()) {
            copy.insert(*this);
        }

        // конструкторы от U

        template<typename U, typename = std::enable_if<std::is_convertible_v<U *, T *>>>
        explicit linked_ptr(U *pointer): node_in_list(), pointer(pointer) {}

        template<typename U, typename = std::enable_if<std::is_convertible_v<U *, T *>>>
        linked_ptr(linked_ptr<U> const &other): node_in_list(), pointer(other.get()) {
            other.insert(*this);
        }

        //todo деструктор
        ~linked_ptr() {
            reset();
        }

        bool unique() const {
            return !node_in_list.next && !node_in_list.prev && pointer;
        }

        T *get() const {
            return pointer;
        }

        void swap(linked_ptr &other) noexcept {
            node_in_list.swap(other.node_in_list);
            std::swap(pointer, other.pointer);
        }

        // присваивания
        linked_ptr &operator=(linked_ptr const &other) noexcept {
            auto tmp(other);
            swap(tmp);
            return *this;
        }

        template<typename U, typename = std::enable_if<std::is_convertible_v<U *, T *>>>
        linked_ptr &operator=(linked_ptr<U> const &other) {
            auto tmp(other);
            swap(tmp);
            return *this;
        }

        // семейство функций reset


        void reset(T *new_pointer = nullptr) {
            destroy();
            pointer = new_pointer;
        }

        template<typename U, typename = std::enable_if<std::is_convertible_v<U *, T *>>>
        void reset(U *new_pointer = nullptr) {
            destroy();
            pointer = new_pointer;
        }

        // операторы -> и *
        T *operator->() const {
            return get();
        }

        T& operator*() const {
            return *get();
        }

        // операторы проверки на равенство и неравенство

        template<typename U>
        inline bool operator==(linked_ptr<U> const &a) const noexcept {
            return get() == a.get();
        }

        template<typename U>
        inline bool operator!=(linked_ptr<U> const &a) const noexcept {
            return !(this == a);
        }

        // операторы сравнения

        template<typename U>
        inline bool operator<(linked_ptr<U> const &a) const noexcept {
            return get() < a.get();
        }

        template<typename U>
        inline bool operator<=(linked_ptr<U> const &a) const noexcept {
            return this < a || this == a;
        }

        template<typename U>
        inline bool operator>(linked_ptr<U> const &a) const noexcept {
            return a < this;
        }

        template<typename U>
        inline bool operator>=(linked_ptr<U> const &a) const noexcept {
            return a <= this;
        }

        // безопасное приведение к логическому выражению
        operator bool() const noexcept {
            return get();
        }


    private:
        void insert(linked_ptr const &a) {
            node_in_list.insert(&a.node_in_list);
        }

        template<typename U, typename = std::enable_if<std::is_convertible_v<U*, T*>>>
        void insert(linked_ptr<U> const &a) const noexcept {
            node_in_list.insert(&a.node_in_list);
        }

        void remove() const noexcept {
            node_in_list.remove();
        }

        void destroy() {
            if (unique() && pointer) {
                delete pointer;
            }
            pointer = nullptr;
            node_in_list.remove();
        }

    public:
        mutable node node_in_list;
        T *pointer;
    };
}

#endif //LINKED_PTR_LINKED_PTR_H
