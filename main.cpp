#include <boost/asio.hpp>

boost::asio::io_service io_service;

boost::asio::ip::tcp::acceptor createTCPServer(int port) {
    boost::asio::ip::tcp::acceptor acceptor(io_service,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    return acceptor;
}


bool socketSetNonBlockNoDelay(boost::asio::ip::tcp::socket& socket) {
    boost::system::error_code ec;

    // Set the socket nonblocking.
    socket.non_blocking(true, ec);
    if (ec) return false;

    // Set TCP_NODELAY option.
    socket.set_option(boost::asio::ip::tcp::no_delay(true), ec);
    if (ec) return false;

    return true;
}

boost::asio::ip::tcp::socket acceptClient(boost::asio::ip::tcp::acceptor& acceptor) {
    boost::asio::ip::tcp::socket socket(acceptor.get_io_service());

    // This will block until a new connection is accepted
    acceptor.accept(socket);

    return socket;
}


std::unique_ptr<char[]> chatMalloc(size_t size) {
    std::unique_ptr<char[]> ptr;
    try {
        ptr = std::make_unique<char[]>(size);
    } catch (std::bad_alloc& ba) {
        std::cerr << "Out of memory: " << ba.what() << std::endl;
        std::exit(1);
    }
    return ptr;
}

std::unique_ptr<char[]> chatRealloc(std::unique_ptr<char[]> ptr, size_t size) {
    ptr.reset();
    try {
        ptr = std::make_unique<char[]>(size);
    } catch (std::bad_alloc& ba) {
        std::cerr << "Out of memory: " << ba.what() << std::endl;
        std::exit(1);
    }
    return ptr;
}

#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <string>

using boost::asio::ip::tcp;

struct Client {
    std::string nick;
    tcp::socket socket;

    Client(boost::asio::io_service& io_service, std::string nick)
        : socket(io_service), nick(std::move(nick)) {}
};

class Chat {
public:
    Chat(boost::asio::io_service& io_service, short port)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

    void sendMsgToAllClientsBut(int excluded, const std::string& msg) {
        for (auto& client : clients_) {
            if (client->socket.native_handle() == excluded) continue;
            boost::asio::write(client->socket, boost::asio::buffer(msg));
        }
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::string nick = "user:" + std::to_string(socket.native_handle());
                    clients_.push_back(std::make_shared<Client>(std::move(socket), nick));
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<Client>> clients_;
};

int main() {
    boost::asio::io_service io_service;
    Chat chat(io_service, 12345);
    io_service.run();
}
