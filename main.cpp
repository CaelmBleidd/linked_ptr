#include <iostream>
#include <memory>
#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "linked_ptr.h"

using namespace smart_ptr;
using std::cout;
using std::endl;


struct A {
    A() {
        a = 0;
    }

    int a;
};

struct B : A {
    B() {
        a = 1;
    }
};

bool base_test() {
    cout << "start: base_test" << endl;
    bool check = true;

    linked_ptr<int> ptr1(new int(4));
    linked_ptr<int> ptr2(new int(4));

    linked_ptr<int> ptr3(ptr1);

    check *= ptr2.unique();
    check *= !ptr1.unique();

    check *= (ptr1 == ptr3);

    return check;
}

bool del_test() {
    cout << "start: del_test" << endl;
    bool check = true;
    linked_ptr<std::vector<int>> vptr(new std::vector<int>{1, 2, 3, 4});
    linked_ptr<std::vector<int>> v2ptr(vptr);

    check *= !v2ptr.unique();

    vptr.~linked_ptr<std::vector<int>>();
    std::vector<int> v = *v2ptr;

    check *= (v[0] == 1);
    check *= v2ptr.unique();

    v2ptr.reset();
    check *= (v2ptr.get() == nullptr);

    return check;
}

bool reset_test() {
    cout << "start: reset_test" << endl;
    bool check = true;

    linked_ptr<double> ptr1(new double(3.6));
    linked_ptr<double> ptr2(new double(3.6));

    check *= (ptr1.get() != ptr2.get());

    linked_ptr<double> nptr1(ptr1);

    nptr1.reset(new double(3.6));

    check *= (*nptr1 == 3.6);

    return check;
}


bool test() {
    cout << "start: test" << endl;
    linked_ptr<A> ptr(new B());
    bool check = (ptr->a == 1);
    return check;
}

bool set_test() {
    cout << "start: set_test" << endl;
    std::set<linked_ptr<int>> S;
    linked_ptr<int> p1(new int(1));
    linked_ptr<int> p2(new int(2));
    linked_ptr<int> p3(new int(3));
    S.insert(p1);
    S.insert(p2);
    S.insert(p3);
    return true;
}


bool unique_test() {
    cout << "start: unique_test" << endl;
    bool check = true;
    linked_ptr<int> p1(new int(1));
    linked_ptr<int> p2(new int(2));
    linked_ptr<int> p3(new int(3));

    check *= (p1.unique() && p2.unique() && p3.unique());
    linked_ptr<int> p4 = p3;
    check *= !p3.unique() && !p4.unique();
    p3.reset(new int(5));
    check *= p3.unique() && p4.unique();
    return check;
}

namespace unique2 {
    struct A {
        A(int a1) : a(a1) {}

        int a;
    };

    struct B : A {
        B(int a1, int b1) : A(a1), b(b1) {}

        int b;
    };
}

bool unique2_test() {
    cout << "start: unique2_test" << endl;
    bool check = true;
    linked_ptr<unique2::B> p1(new unique2::B(2, 3));
    linked_ptr<unique2::A> p2(p1);
    check *= !p1.unique();
    check *= !p2.unique();
    return check;
}

bool test_bool(linked_ptr<int> lp, std::shared_ptr<int> sp) {
    cout << "start: test_bool" << endl;
    if (sp) {
        if (!lp)
            return false;
    } else {
        if (lp)
            return false;
    }
    return true;
}

class is_deleted {
public:
    is_deleted(bool &_is_deleted) : _is_deleted(_is_deleted) {}

    ~is_deleted() {
        _is_deleted = true;
    }

private:
    bool &_is_deleted;
};

bool test_swap() {
    cout << "start: test_swap" << endl;

    bool is_a_deleted = false;
    bool is_b_deleted = false;

    auto *a0 = new is_deleted(is_a_deleted);
    auto *b0 = new is_deleted(is_b_deleted);

    linked_ptr<is_deleted> a1(a0);
    linked_ptr<is_deleted> a2(a1);
    linked_ptr<is_deleted> a3(a2);
    linked_ptr<is_deleted> a4(a3);

    linked_ptr<is_deleted> b1(b0);
    linked_ptr<is_deleted> b2(b1);
    linked_ptr<is_deleted> b3(b2);
    linked_ptr<is_deleted> b4(b3);

    a2.swap(b2);
    b3.swap(a2);
    b1.swap(a1);

    a3.reset();
    b2.reset();
    b1.reset();
    a4.reset();

    return is_a_deleted && !is_b_deleted;
}

int main() {
    if (!base_test()) {
        std::cerr << "base_test failed" << std::endl;
    } else cout << "ok" << endl;

    if (!del_test()) {
        std::cerr << "del_test failed" << std::endl;
    } else cout << "ok" << endl;

    if (!reset_test()) {
        std::cerr << "reset_test failed" << std::endl;
    } else cout << "ok" << endl;

    if (!test()) {
        std::cerr << "test failed" << std::endl;
    } else cout << "ok" << endl;

    if (!set_test()) {
        std::cerr << "set_test failed" << std::endl;
    } else cout << "ok" << endl;

    if (!unique_test()) {
        std::cerr << "unique_test failed" << std::endl;
    } else cout << "ok" << endl;

    if (!unique2_test()) {
        std::cerr << "unique2_test failed" << std::endl;
    } else cout << "ok" << endl;

    linked_ptr<int> ptr(new int(4));
    std::shared_ptr<int> ptr2(new int(4));
    if (!test_bool(ptr, ptr2)) {
        std::cerr << "test_bool failed" << std::endl;
    } else cout << "ok" << endl;

    if (!test_swap()) {
        std::cerr << "test_swap failed" << std::endl;
    } else cout << "ok" << endl;

    return 0;
}

