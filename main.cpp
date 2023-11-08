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
