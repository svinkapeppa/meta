#include <iostream>

enum class Role {
	Mediator,
	Proxy,
	Observer,
};

class IB {
public:
	void virtual action() = 0;
};

class B : public IB {
public:
	void action() {
		std::cout << "Action performed" << std::endl;
	}
};

template<Role>
class C {};

class A {
public:
	void action(IB& b) {
		b.action();
	}

	void update() {
		std::cout << "Updated" << std::endl;
	}
};

template<>
class C<Role::Mediator> : public IB {
public:
	void action() {
		std::cout << "This `action` call was done via `Mediator`" << std::endl;
		b[0].action();
		b[1].action();
	}

	B b[2];
};

template<>
class C<Role::Proxy> : public IB {
public:
	C(IB* _ib) : ib(_ib) {}

	void action() {
		std::cout << "This `action` call was proxed" << std::endl;
		ib -> action();
	}

	IB* ib;
};

template<>
class C<Role::Observer> {
public:
	C(A* _a, B* _b) : a(_a), b(_b) {}

	void action() {
		std::cout << "Notification from `Observer`" << std::endl;
		a -> update();
	}

	A* a;
	B* b;
};

int main() {
	A a;
	B b;
	C<Role::Mediator> c_mediator;
	C<Role::Proxy> c_proxy(&b);
	C<Role::Observer> c_observer(&a, &b);

	a.action(c_mediator);

	std::cout << std::endl;

	a.action(c_proxy);

	std::cout << std::endl;

	c_observer.action();
}
