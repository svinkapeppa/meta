#include <iostream>
#include <vector>

// Template parameters, that can be passed to `Server` class
enum class Role {
  Default,        //                                    Dummy `Role`, which is needed for initialization
  Mediator,       //                          `Server` becomes a `Mediator` between `Client` and `Worker`
  Proxy,          //            `Server` becomes a proxied variant of `Worker`, which is used by `Client`
  Observer,       //       `Server` becomes an `Observer` and notifies `Client` about changes in `Worker`
};

// Interface for `Worker`
class WorkerInterface {
public:
  // `Worker` can either perform some action, or update its internal state
  void virtual action(int=-1) = 0;
  void virtual update(int) = 0;
};

// Concrete `Worker`
class Worker : public WorkerInterface {
public:
  Worker(WorkerInterface& _server) : server(_server), state(0) {}

  // Get `Worker` internal state
  void action(int=-1) {
    std::cout << "Worker internal state is " << state << std::endl;
  }

  // Set `Worker` internal state
  void update(int _state) {
    state = _state;
    server.action(state);
  }

private:
  // `Worker` works with some `Server`
  WorkerInterface& server;

  // `Worker` has some internal state
  int state;
};

// Concrete `Client`
class Client {
public:
  // Perform action on `Worker`
  void action(WorkerInterface& worker) {
    worker.action();
  }

  // Notify everyone, that `Client` is updated
  void update() {
    std::cout << "Client was updated" << std::endl;
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Here comes partial template specialization
template<Role>
class Server {};

// Dummy `Server`
template<>
class Server<Role::Default> : public WorkerInterface {
  void action(int) {}
  void update(int) {}
};

// `Mediator` `Server`
template<>
class Server<Role::Mediator> : public WorkerInterface {
public:
  // `Server` can store `Worker` objects
  void add_worker(Worker& worker) {
    workers.push_back(worker);
  }

  // Action is applied for every `Worker` object
  void action(int=-1) {
    std::cout << "This `action` call was done via `Mediator`" << std::endl;
    for (auto worker : workers) {
      worker.action();
    }
  }

  void update(int) {}
 
private:
  std::vector<Worker> workers;
};

template<>
class Server<Role::Proxy> : public WorkerInterface {
public:
  // Specify which `Worker` need to be proxied
  Server(WorkerInterface& _worker) : worker(_worker) {}

  void action(int=-1) {
    std::cout << "This `action` call was proxied" << std::endl;
    worker.action();
  }

  void update(int) {}

private:
  WorkerInterface& worker;
};

template<>
class Server<Role::Observer> : public WorkerInterface {
public:
  // Allow `Client` subscribe for `Worker`
  void add_client(Client& client) {
    clients.push_back(client);
  }

  void action(int state = -1) {
    std::cout << "Notification from `Observer`" << std::endl;
    std::cout << "Worker's internal state changed to " << state << std::endl;
    for (auto client : clients) {
      client.update();
    }
  }

  void update(int) {}

private:
  std::vector<Client> clients;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
  // Dummy objects, needed for further usage and initialization
  Server<Role::Default> server_default;
  Worker worker_default_1(server_default);
  Worker worker_default_2(server_default);
  Worker worker_default_3(server_default);
  Client client_default_1;
  Client client_default_2;

  // Different types of `Servers`
  Server<Role::Mediator> server_mediator;
  Server<Role::Proxy> server_proxy(worker_default_1);
  Server<Role::Observer> server_observer;
  
  // Add `Worker` objects to `Mediator`
  server_mediator.add_worker(worker_default_1);
  server_mediator.add_worker(worker_default_2);
  server_mediator.add_worker(worker_default_3);

  // Add `Client` objects to `Observer`
  server_observer.add_client(client_default_1);
  server_observer.add_client(client_default_2);

  // Concrete `Client`
  Client client;

  // Concrete `Worker`
  Worker worker(server_observer);

  // `Mediator` test
  client.action(server_mediator);
  std::cout << std::endl;

  // `Proxy` test
  client.action(server_proxy);
  std::cout << std::endl;

  // `Observer` test
  worker.update(1);
}
