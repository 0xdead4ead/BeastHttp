// This example is based on the existing
// Original: https://www.boost.org/doc/libs/1_67_0/libs/beast/example/http/server/async/http_server_async.cpp

// Copyright (c) 2016-2017 Vinnie Falco
// Copyright (c) 2018 0xdead4ead

#include <iostream>

#include <server.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

using string_view = boost::beast::string_view;

using namespace boost::filesystem;


// Return base root path
path & get_doc_root_path(){
    static path doc_root;
    return doc_root;
}

// Append an HTTP rel-path to a local filesystem path.
path path_concat(
        const string_view & target,
        const string_view & resource
        ){

    path result = get_doc_root_path();

    result.concat(target.begin(), target.end());
    result.concat(resource.begin(), resource.end());

    return result;
}

// Return a reasonable mime type based on the extension of a file.
string_view mime_type(const path & extension)
{
    using boost::beast::iequals;
    string_view ext
    {
        extension.string().data(),
                extension.string().size()};

    if(iequals(ext, ".htm"))  return "text/html";
    if(iequals(ext, ".html")) return "text/html";
    if(iequals(ext, ".php"))  return "text/html";
    if(iequals(ext, ".css"))  return "text/css";
    if(iequals(ext, ".txt"))  return "text/plain";
    if(iequals(ext, ".js"))   return "application/javascript";
    if(iequals(ext, ".json")) return "application/json";
    if(iequals(ext, ".xml"))  return "application/xml";
    if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if(iequals(ext, ".flv"))  return "video/x-flv";
    if(iequals(ext, ".png"))  return "image/png";
    if(iequals(ext, ".jpe"))  return "image/jpeg";
    if(iequals(ext, ".jpeg")) return "image/jpeg";
    if(iequals(ext, ".jpg"))  return "image/jpeg";
    if(iequals(ext, ".gif"))  return "image/gif";
    if(iequals(ext, ".bmp"))  return "image/bmp";
    if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if(iequals(ext, ".tiff")) return "image/tiff";
    if(iequals(ext, ".tif"))  return "image/tiff";
    if(iequals(ext, ".svg"))  return "image/svg+xml";
    if(iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

// Returns a bad request response
template<class Request>
auto bad_request(Request & req, const std::string & why)
{
    boost::beast::http::response<boost::beast::http::string_body> res
    {
        boost::beast::http::status::bad_request,
                req.version()};

    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = why;
    res.prepare_payload();
    return res;
};

// Returns a not found response
template<class Request>
auto not_found(Request & req)
{
    boost::beast::http::response<boost::beast::http::string_body> res
    {
        boost::beast::http::status::not_found, req.version()};

    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "The resource '" + req.target().to_string() + "' was not found.\n";
    res.prepare_payload();
    return res;
};

// Returns a server error response
template<class Request>
auto server_error(Request & req, const boost::beast::string_view & what)
{
    boost::beast::http::response<boost::beast::http::string_body> res
    {
        boost::beast::http::status::internal_server_error, req.version()};

    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "An error occurred: '" + what.to_string() + "'\n";
    res.prepare_payload();
    return res;
};

// Return body of resource if not error, else send response of signal error
template<class Request, class Session>
boost::beast::http::file_body::value_type process_resource(
        const path & path_to_target, boost::beast::error_code & ec, Request & req, Session & session)
{
    // Attempt to open the file
    boost::beast::http::file_body::value_type body;
    body.open(path_to_target.c_str(), boost::beast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if(ec == boost::system::errc::no_such_file_or_directory)
        session.do_write(not_found(req));
    // Handle an unknown error
    else if(ec)
        session.do_write(server_error(req, ec.message()));

    return body;
}


int main(int argc, char* argv[])
{
//g++ -c -std=gnu++14 -I../../include -o ex4_server.o ./ex4_server.cpp
//g++ -o ex4_server ex4_server.o -lboost_system -lboost_thread -lpthread -lboost_regex -licui18n -lboost_filesystem

    //##################################################################
    if (argc != 5)
    {
      std::cerr << "Usage: ex4_server <address> <port> <threads> <doc_root>\n";
      std::cerr << "  IPv4, try:  0.0.0.0 80 1 .\n";
      std::cerr << "  IPv6, try:  0::0 80 1 .\n";
      return 1;
    }


    //##################################################################
    const string_view address{argv[1]};
    const string_view port{argv[2]};
    const string_view threads{argv[3]};
    const string_view doc_root_arg{argv[4]};
    //##################################################################

    if(doc_root_arg.back() == '/')
        get_doc_root_path() = path{doc_root_arg.begin(), doc_root_arg.end() - 1};
    else
        get_doc_root_path() = doc_root_arg.data();

    //##################################################################
    http::server instance;

    //##################################################################
    // Respond to HEAD request ('/' => 'index.html')
    instance.head("/", [](auto & req, auto & session){
        // make final path to resource
        path path_to_target;
        if(req.target().back() == '/')
            path_to_target = path_concat(req.target(), "index.html");
        else
            path_to_target = path_concat(req.target(), "");

        boost::beast::error_code ec;
        auto body = process_resource(path_to_target, ec, req, session);

        if(!ec){
            auto const body_size = body.size();

            boost::beast::http::response<boost::beast::http::empty_body> res
            {
                boost::beast::http::status::ok, req.version()};

            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, mime_type(path_to_target.extension()));
            res.content_length(body_size);
            res.keep_alive(req.keep_alive());

            return session.do_write(std::move(res));
        }
    });

    //##################################################################
    // Respond to GET request ('/' => 'index.html')
    instance.get("/", [](auto & req, auto & session){
        // make final path to resource
        path path_to_target;
        if(req.target().back() == '/')
            path_to_target = path_concat(req.target(), "index.html");
        else
            path_to_target = path_concat(req.target(), "");

        boost::beast::error_code ec;
        auto body = process_resource(path_to_target, ec, req, session);

        if(!ec){
            auto const body_size = body.size();

            boost::beast::http::response<boost::beast::http::file_body> res{
                std::piecewise_construct,
                std::make_tuple(std::move(body)),
                std::make_tuple(boost::beast::http::status::ok, req.version())};

            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, mime_type(path_to_target.extension()));
            res.content_length(body_size);
            res.keep_alive(req.keep_alive());

            return session.do_write(std::move(res));
        }
    });

    instance.all(".*", [](auto & req, auto & session){
        // Make sure we can handle the method
        if(req.method() != boost::beast::http::verb::get &&
                req.method() != boost::beast::http::verb::head)
            return session.do_write(bad_request(req, "Unknown HTTP-method\n"));

        if( req.target().empty() ||
            req.target()[0] != '/' ||
            req.target().find("..") != boost::beast::string_view::npos)
            return session.do_write(bad_request(req, "Illegal request-target\n"));

        return session.do_write(not_found(req));
    });

    const auto & on_accept = [](auto & session){
        http::base::out(session.getConnection().stream().remote_endpoint().address().to_string() + " connected");
        session.do_read();
    };

    const auto & on_error = [](auto & error, auto & info){
        std::cout << "Error code: " << error.value() << ", "
             << "Info: " << info << std::endl;
    };

    if(!instance.listen(address.data(), boost::lexical_cast<uint32_t>(port), on_accept, on_error)){
        std::cout << "Failed to resolve address or can't open listener!" << std::endl;
        http::base::processor::get().stop();
    };

    //##################################################################
    http::base::processor::get().register_signals_handler([](int signal){
        if(signal == SIGINT)
            http::base::out("Interactive attention signal");
        else if(signal == SIGTERM)
            http::base::out("Termination request");
        else
            http::base::out("Quit");
        http::base::processor::get().stop();
    }, std::vector<int>{SIGINT,SIGTERM, SIGQUIT});

    uint32_t pool_size = boost::lexical_cast<uint32_t>(threads);
    http::base::processor::get().start(pool_size == 0 ? 4 : pool_size << 1);
    http::base::processor::get().wait();

}
