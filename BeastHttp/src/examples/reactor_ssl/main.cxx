#include <http/reactor/listener.hxx>
#include <http/reactor/ssl/session.hxx>

#include <http/basic_router.hxx>
#include <http/out.hxx>

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/signal_set.hpp>

#include <thread>

namespace beast = boost::beast;

// Returns a success response (200)
template<class ResponseBody, class RequestBody>
auto make_200(const beast::http::request<RequestBody>& request,
              typename ResponseBody::value_type body,
              beast::string_view content)
{
    beast::http::response<ResponseBody> response{beast::http::status::ok, request.version()};
    response.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(beast::http::field::content_type, content);
    response.body() = body;
    response.prepare_payload();
    response.keep_alive(request.keep_alive());

    return response;
}

// Returns a not found response (404)
template<class ResponseBody, class RequestBody>
auto make_404(const beast::http::request<RequestBody>& request,
              typename ResponseBody::value_type body,
              beast::string_view content)
{
    beast::http::response<ResponseBody> response{beast::http::status::not_found, request.version()};
    response.set(beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(beast::http::field::content_type, content);
    response.body() = body;
    response.prepare_payload();
    response.keep_alive(request.keep_alive());

    return response;
}

static boost::asio::io_context ioc;
static boost::asio::posix::stream_descriptor out{ioc, ::dup(STDOUT_FILENO)};
static boost::asio::signal_set sig_set(ioc, SIGINT, SIGTERM);
static boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

int main()
{
    // localhost.crt
    std::string const& crt =
            "-----BEGIN CERTIFICATE-----\n"
            "MIID7zCCAtegAwIBAgIUNCRVPYAJtn2rRrk9JpaD3Wz82pQwDQYJKoZIhvcNAQEL\n"
            "BQAwgYcxCzAJBgNVBAYTAlVTMQswCQYDVQQIDAJDQTEUMBIGA1UEBwwLTG9zLUFu\n"
            "Z2VsZXMxDjAMBgNVBAoMBUJlYXN0MQ4wDAYDVQQLDAVCZWFzdDESMBAGA1UEAwwJ\n"
            "bG9jYWxob3N0MSEwHwYJKoZIhvcNAQkBFhJ4eHggYXQgeXl5IGRvdCBjb20wHhcN\n"
            "MTkwMTIxMjEwMzI5WhcNMjQwMTIwMjEwMzI5WjCBhzELMAkGA1UEBhMCVVMxCzAJ\n"
            "BgNVBAgMAkNBMRQwEgYDVQQHDAtMb3MtQW5nZWxlczEOMAwGA1UECgwFQmVhc3Qx\n"
            "DjAMBgNVBAsMBUJlYXN0MRIwEAYDVQQDDAlsb2NhbGhvc3QxITAfBgkqhkiG9w0B\n"
            "CQEWEnh4eCBhdCB5eXkgZG90IGNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\n"
            "AQoCggEBALa0Anf4VW6roKn5dOBZyvdQrZ5/nX17pGvnec/2Ge0jO/LM3oujYg2F\n"
            "uX3r2D6u7Aje+N5tCgT0jtQI3KzhL+5H7/yJHRMhhRc8MadXHJudNvDOtOMqgisb\n"
            "CzIsp5jrav4AFMvUVieq1N9JkPwKl4Lgh41qCnA64QNx9mfzqGG3oegfWjIQdY93\n"
            "XlS/wZxlGErtPUTiznWdG5G8cSPUZLgpEC2mFzfbTjhIigwWbuJ038TUdjrU6WOK\n"
            "JLbBImZDBuQLoBovF8QyDZ/U0k3Dd+PItWG5yM7GfEDF47NqIZ2X5gh8NtGaok+6\n"
            "I4VOfRH2CqLqGfQuDlxxxUW6ZfwRjRsCAwEAAaNRME8wHwYDVR0jBBgwFoAUjiyu\n"
            "7Mq2GtYXIY+RDqRXSyDzt/AwCQYDVR0TBAIwADALBgNVHQ8EBAMCBPAwFAYDVR0R\n"
            "BA0wC4IJbG9jYWxob3N0MA0GCSqGSIb3DQEBCwUAA4IBAQCXtGUK0FtcFzsiJYXW\n"
            "StMsmeNSTAItI11KfKaBf8x3Nv8Cjv7YHIP6E8L3Y1caZ31XAWOq2yC3XVtdJiBV\n"
            "u/9DdFDRBR62MZ/VSgBV7bnjsqtOFGffAY1i31nj8Bb3YkhvllW8iQuFyF1zBJAO\n"
            "DQ/C/GHN1toBQFz4QKiE+0w4Jx/iyo7XogAdvHTHNDxAKAgS921gnSxJG4dFHkLk\n"
            "v32/Dn1d95RFHYbUNza3WxNfiigKeqfn7G8t6C3rD8ErUSOwQxysjjVUzpB/M6jU\n"
            "S0F7R6eve9fGXa1HQsKlOuhpXM61UBLnAc5CirVZJrQXoimK6gFy0vWY3/0NJiRy\n"
            "UIhn\n"
            "-----END CERTIFICATE-----\n";

    // localhost.key
    std::string const& key =
            "-----BEGIN RSA PRIVATE KEY-----\n"
            "MIIEowIBAAKCAQEAtrQCd/hVbqugqfl04FnK91Ctnn+dfXuka+d5z/YZ7SM78sze\n"
            "i6NiDYW5fevYPq7sCN743m0KBPSO1AjcrOEv7kfv/IkdEyGFFzwxp1ccm5028M60\n"
            "4yqCKxsLMiynmOtq/gAUy9RWJ6rU30mQ/AqXguCHjWoKcDrhA3H2Z/OoYbeh6B9a\n"
            "MhB1j3deVL/BnGUYSu09ROLOdZ0bkbxxI9RkuCkQLaYXN9tOOEiKDBZu4nTfxNR2\n"
            "OtTpY4oktsEiZkMG5AugGi8XxDINn9TSTcN348i1YbnIzsZ8QMXjs2ohnZfmCHw2\n"
            "0ZqiT7ojhU59EfYKouoZ9C4OXHHFRbpl/BGNGwIDAQABAoIBAFFdRSQsYc+Re1F3\n"
            "GIe5NVyCf29wobB6WbT8NxFYJh+na1Erz4gcUNR40yyy1XDE8bTZp7dWk/m0986R\n"
            "y8Tp3DCftM6gOWSFrwGz+uDtOH5Mu2TuiTwJeEK+HnRP0quVW+aWHqzPeTbdK0Pt\n"
            "PTYheWv9liByMwfcEM05+WQ97KTnI8WQ7Re9m8TS0wa9FSb4vcfsv+b/jGwFyHj9\n"
            "K05aDsNLa9DAb6YQg3w50d78PN4+hgIlKGl26OgrCKU0o+RHQKstbnBp8fDhnzTM\n"
            "AyAGt4RztMIpZTFXN7viaZ4zYJV4I5946txehNQ1ZTp8XWV1Et89peFvkS/2imd+\n"
            "EMCpyhECgYEA4ySmCZ6faJA/kH3zkzKxRdziJLDedbWBqVdl3bVEcdWnS1gJWlRy\n"
            "azcxGdlkMCoWrLMabyb6Th6KQW9WIuuDd9tY+wIDTfqZSsP5MgYcT++le6DPbaFI\n"
            "kVffnkqAQLk9OCGXb11GzdkxZGUH9k7vgSKSQ+szlOEzZt66sfHkb+0CgYEAzeoL\n"
            "SAtbaE189WJQOdCtw19Wg27HWx9YMSxFqYW05sA+lpumLBq4ZN22z/3N4D+uCK4p\n"
            "b6lSxT1Z+tGyCT4g3ArfS1j4Ypgl1qSs8h21uHE5OyP+upX0bSeCCWiWzp9nvS9b\n"
            "jrpLA1SjyOX8HIrnBxGk9hfyeDjzyqzieZ3GgCcCgYA0TbUaE/ebCVRJvmb/JzYg\n"
            "4zsDveKL8k64ItLnGZ14JP2P4ObOcRGj1KKu1Bs46AgunlNg8vXCVU/6v1tzUQ4F\n"
            "5HF54z3FkAYKbFQOi8+rLIDLlf0NvozKQxTQC1G7FayeJ0YKs8zhC7lO5+DMe+K0\n"
            "trQ9hYRsLOs8Ld+JqCy1BQKBgGXkwLxf0lWksocCpUxWtQWOsdLMCJ5QvmVm0Pn4\n"
            "DfJ5gv8n+mystD5FcGcv4qZrHIVzGVIF6c1sdJAnIABu2/fG8YPpfM/dFixGxMXy\n"
            "aa/hZz6aqzxNY+x3tmNNfAK24rC9NINGMERvp7SMCdtd6MCYvJaCdxuCozDRqCgi\n"
            "KM3VAoGBAOL/jjWj8O8TPAv+4rDT5F4gzg/x3d8m7hAqjeG76Rt/SZt3Gbgo+UVg\n"
            "Tf9Wnv+5OgNDneMPnaG7pXgJBdkFCjxq7XjDqG+Bd2gBiAHQDUoD93782mpF0rMy\n"
            "/OxtFm+Wu5ag6nhLH1383xTzOLau+B3820igoF8y+X0l1Pw35uFf\n"
            "-----END RSA PRIVATE KEY-----\n";

    std::string const& dh =
            "-----BEGIN DH PARAMETERS-----\n"
            "MIIBCAKCAQEAw5V8Zv0UXTzjBLBr+Wje5RktwL1K27giAQoZIKfs5MsKqAkaGJOI\n"
            "jeThplBGu26wZOxUKa0+aSU780JQY75aOYXqw6trLPC8Ay9ogQP9XzbxyJQPj2lJ\n"
            "LBwHnDVwU9xIYmwVBzo5QbVyssxtQlh+XckOARTQ4dz3x5lob9/W0Q6beRWZG7w6\n"
            "ruYU2DlZ5HMT2bMJkYV+T1Z6ZBVg8uXjuAvsqjHRJNDvKDPXWeZqHE4I4xFQo5MU\n"
            "ua0cgFeqJ9lzwiGKgTnwAswKA/c/XIX/xsCAdL1wp+a+U98loQfS/ZvWTg4Wer88\n"
            "18rx/G5U0pwJzRDqNbX2cwl3+3rj8KlsKwIBAg==\n"
            "-----END DH PARAMETERS-----\n";

    ctx.set_options(boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::no_sslv2 |
                    boost::asio::ssl::context::single_dh_use);

    ctx.use_certificate_chain(boost::asio::buffer(crt.data(), crt.size()));
    ctx.use_private_key(boost::asio::buffer(key.data(), key.size()),
                        boost::asio::ssl::context::file_format::pem);
    ctx.use_tmp_dh(boost::asio::buffer(dh.data(), dh.size()));

    ctx.set_password_callback(
                [](std::size_t, boost::asio::ssl::context_base::password_purpose)
    {
        return "test";
    });

    using namespace _0xdead4ead;

    using SslHttpSession = http::reactor::ssl::_default::session_type;
    using HttpListener = http::reactor::_default::listener_type;

    http::basic_router<SslHttpSession> router{boost::regex::ECMAScript};

    // Set router targets
    router.get(R"(^/$)", [](auto request, auto context) {
        // Send content message to client and wait to receive next request
        context.send(make_200<beast::http::string_body>(request, "Main page\n", "text/html"));
    });

    router.all(R"(^.*$)", [](auto request, auto context) {
        context.send(make_404<beast::http::string_body>(request, "Resource is not found\n", "text/html"));
    });

    // Handshake process is success
    const auto& onHandshake = [](auto context) {
        context.recv();
    };

    // Error and warning handler
    const auto& onError = [](auto code, auto from) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, "From:", from, "Info:", code.message());

        // I/O context will be stopped, if code value is EADDRINUSE or EACCES
        if (code == boost::system::errc::address_in_use or
                code == boost::system::errc::permission_denied)
            ioc.stop();
    };

    // Handler incoming connections
    const auto& onAccept = [&](auto asioSocket) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, asioSocket.remote_endpoint().address().to_string(), "connected!");

        SslHttpSession::handshake(ctx, std::move(asioSocket), router, onHandshake, onError);
    };

    // https://localhost:8080
    auto const address = boost::asio::ip::make_address("127.0.0.1");
    auto const port = static_cast<unsigned short>(8080);

    http::out::prefix::version::time::pushn<std::ostream>(
                out, "Start accepting on", address.to_string() + ':' + std::to_string(port));

    // Start accepting
    HttpListener::launch(ioc, {address, port}, onAccept, onError);

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    sig_set.async_wait([](boost::system::error_code const&, int sig) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, "Capture", sig == SIGINT ? "SIGINT." : "SIGTERM.", "Stop!");
        ioc.stop();
    });

    uint32_t pool_size = std::thread::hardware_concurrency() * 2;

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> threads;
    threads.reserve(pool_size > 0 ? pool_size : 4);
    for(uint32_t i = 0; i < pool_size; i++)
        threads.emplace_back(std::bind(static_cast<std::size_t (boost::asio::io_context::*)()>
                                       (&boost::asio::io_context::run), std::ref(ioc)));

    // Block until all the threads exit
    for(auto& t : threads)
        t.join();

    return 0;
}
