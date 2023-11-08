#include <boost/asio.hpp>

boost::asio::io_service io_service;

boost::asio::ip::tcp::acceptor createTCPServer(int port) {
    boost::asio::ip::tcp::acceptor acceptor(io_service,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    return acceptor;
}
