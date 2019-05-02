//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2019 Evgeny Tixonow
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

// P.S.
// This file has been updated to Boost.Beast version 235 for support early Boost library versions. ( >= 1.67.0 )

#ifndef BOOST_BEAST_EXAMPLE_COMMON_DETECT_SSL_HPP
#define BOOST_BEAST_EXAMPLE_COMMON_DETECT_SSL_HPP

#include <boost/config.hpp>

#include <boost/beast/core.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/logic/tribool.hpp>

//------------------------------------------------------------------------------
//
// Example: Detect TLS client_hello
//
//  This is an example and also a public interface. It implements
//  an algorithm for determining if a "TLS client_hello" message
//  is received. It can be used to implement a listening port that
//  can handle both plain and TLS encrypted connections.
//
//------------------------------------------------------------------------------

//[example_core_detect_ssl_1

// By convention, the "detail" namespace means "not-public."
// Identifiers in a detail namespace are not visible in the documentation,
// and users should not directly use those identifiers in programs, otherwise
// their program may break in the future.
//
// Using a detail namespace gives the library writer the freedom to change
// the interface or behavior later, and maintain backward-compatibility.

namespace detail {

/** Return `true` if the buffer contains a TLS Protocol client_hello message.

    This function analyzes the bytes at the beginning of the buffer
    and compares it to a valid client_hello message. This is the
    message required to be sent by a client at the beginning of
    any TLS (encrypted communication) session, including when
    resuming a session.

    The return value will be:

    @li `true` if the contents of the buffer unambiguously define
    contain a client_hello message,

    @li `false` if the contents of the buffer cannot possibly
    be a valid client_hello message, or

    @li `boost::indeterminate` if the buffer contains an
    insufficient number of bytes to determine the result. In
    this case the caller should read more data from the relevant
    stream, append it to the buffers, and call this function again.

    @param buffers The buffer sequence to inspect.
    This type must meet the requirements of <em>ConstBufferSequence</em>.

    @return `boost::tribool` indicating whether the buffer contains
    a TLS client handshake, does not contain a handshake, or needs
    additional bytes to determine an outcome.

    @see

    <a href="https://tools.ietf.org/html/rfc2246#section-7.4">7.4. Handshake protocol</a>
    (RFC2246: The TLS Protocol)
*/
template <class ConstBufferSequence>
boost::tribool
is_tls_client_hello (ConstBufferSequence const& buffers);

} // detail

//]

//[example_core_detect_ssl_2

namespace detail {

template <class ConstBufferSequence>
boost::tribool
is_tls_client_hello (ConstBufferSequence const& buffers)
{
    // Make sure buffers meets the requirements
    static_assert(
        boost::asio::is_const_buffer_sequence<ConstBufferSequence>::value,
        "ConstBufferSequence type requirements not met");

/*
    The first message on a TLS connection must be the client_hello,
    which is a type of handshake record, and it cannot be compressed
    or encrypted. A plaintext record has this format:

         0      byte    record_type      // 0x16 = handshake
         1      byte    major            // major protocol version
         2      byte    minor            // minor protocol version
       3-4      uint16  length           // size of the payload
         5      byte    handshake_type   // 0x01 = client_hello
         6      uint24  length           // size of the ClientHello
         9      byte    major            // major protocol version
        10      byte    minor            // minor protocol version
        11      uint32  gmt_unix_time
        15      byte    random_bytes[28]
                ...
*/

    // Flatten the input buffers into a single contiguous range
    // of bytes on the stack to make it easier to work with the data.
    unsigned char buf[9];
    auto const n = boost::asio::buffer_copy(
        boost::asio::mutable_buffer(buf, sizeof(buf)), buffers);

    // Can't do much without any bytes
    if(n < 1)
        return boost::indeterminate;

    // Require the first byte to be 0x16, indicating a TLS handshake record
    if(buf[0] != 0x16)
        return false;

    // We need at least 5 bytes to know the record payload size
    if(n < 5)
        return boost::indeterminate;

    // Calculate the record payload size
    std::uint32_t const length = (buf[3] << 8) + buf[4];

    // A ClientHello message payload is at least 34 bytes.
    // There can be multiple handshake messages in the same record.
    if(length < 34)
        return false;

    // We need at least 6 bytes to know the handshake type
    if(n < 6)
        return boost::indeterminate;

    // The handshake_type must be 0x01 == client_hello
    if(buf[5] != 0x01)
        return false;

    // We need at least 9 bytes to know the payload size
    if(n < 9)
        return boost::indeterminate;

    // Calculate the message payload size
    std::uint32_t const size =
        (buf[6] << 16) + (buf[7] << 8) + buf[8];

    // The message payload can't be bigger than the enclosing record
    if(size + 4 > length)
        return false;

    // This can only be a TLS client_hello message
    return true;
}

} // detail

//]

//[example_core_detect_ssl_3

/** Detect a TLS/SSL handshake on a stream.
    This function reads from a stream to determine if a TLS/SSL
    handshake is being received. The function call will block
    until one of the following conditions is true:
    @li The disposition of the handshake is determined
    @li An error occurs
    Octets read from the stream will be stored in the passed dynamic
    buffer, which may be used to perform the TLS handshake if the
    detector returns true, or otherwise consumed by the caller based
    on the expected protocol.
    @param stream The stream to read from. This type must meet the
    requirements of @b SyncReadStream.
    @param buffer The dynamic buffer to use. This type must meet the
    requirements of @b DynamicBuffer.
    @param ec Set to the error if any occurred.
    @return `boost::tribool` indicating whether the buffer contains
    a TLS client handshake, does not contain a handshake, or needs
    additional octets. If an error occurs, the return value is
    undefined.
*/
template<
    class SyncReadStream,
    class DynamicBuffer>
boost::tribool
detect_ssl(
    SyncReadStream& stream,
    DynamicBuffer& buffer,
    boost::beast::error_code& ec)
{
    namespace beast = boost::beast;

    // Make sure arguments meet the requirements
    static_assert(beast::is_sync_read_stream<SyncReadStream>::value,
        "SyncReadStream requirements not met");
    static_assert(
        boost::asio::is_dynamic_buffer<DynamicBuffer>::value,
        "DynamicBuffer requirements not met");

    // Loop until an error occurs or we get a definitive answer
    for(;;)
    {
        // There could already be data in the buffer
        // so we do this first, before reading from the stream.
        auto const result = detail::is_tls_client_hello(buffer.data());

        // If we got an answer, return it
        if(! boost::indeterminate(result))
        {
            // This is a fast way to indicate success
            // without retrieving the default category.
            ec.assign(0, ec.category());
            return result;
        }

        // The algorithm should never need more than 4 bytes
        BOOST_ASSERT(buffer.size() < 4);

        // Prepare the buffer's output area.
        auto const mutable_buffer = buffer.prepare(beast::read_size(buffer, 1536));

        // Try to fill our buffer by reading from the stream
        std::size_t const bytes_transferred = stream.read_some(mutable_buffer, ec);

        // Check for an error
        if(ec)
            break;

        // Commit what we read into the buffer's input area.
        buffer.commit(bytes_transferred);
    }

    // error
    return false;
}

//]

//[example_core_detect_ssl_4

/** Detect a TLS/SSL handshake asynchronously on a stream.
    This function is used to asynchronously determine if a TLS/SSL
    handshake is being received.
    The function call always returns immediately. The asynchronous
    operation will continue until one of the following conditions
    is true:
    @li The disposition of the handshake is determined
    @li An error occurs
    This operation is implemented in terms of zero or more calls to
    the next layer's `async_read_some` function, and is known as a
    <em>composed operation</em>. The program must ensure that the
    stream performs no other operations until this operation completes.
    Octets read from the stream will be stored in the passed dynamic
    buffer, which may be used to perform the TLS handshake if the
    detector returns true, or otherwise consumed by the caller based
    on the expected protocol.
    @param stream The stream to read from. This type must meet the
    requirements of @b AsyncReadStream.
    @param buffer The dynamic buffer to use. This type must meet the
    requirements of @b DynamicBuffer.
    @param handler Invoked when the operation completes.
    The handler may be moved or copied as needed.
    The equivalent function signature of the handler must be:
    @code
    void handler(
        error_code const& error,    // Set to the error, if any
        boost::tribool result       // The result of the detector
    );
    @endcode
    Regardless of whether the asynchronous operation completes
    immediately or not, the handler will not be invoked from within
    this function. Invocation of the handler will be performed in a
    manner equivalent to using `boost::asio::io_context::post`.
*/
template<
    class AsyncReadStream,
    class DynamicBuffer,
    class CompletionToken>
BOOST_ASIO_INITFN_RESULT_TYPE(                      /*< `BOOST_ASIO_INITFN_RESULT_TYPE` customizes the return value based on the completion token >*/
    CompletionToken,
    void(boost::beast::error_code, boost::tribool)) /*< This is the signature for the completion handler >*/
async_detect_ssl(
    AsyncReadStream& stream,
    DynamicBuffer& buffer,
    CompletionToken&& token);

//]

//[example_core_detect_ssl_5

// This is the composed operation.
template<
    class AsyncReadStream,
    class DynamicBuffer,
    class Handler>
class detect_ssl_op;

// Here is the implementation of the asynchronous initation function
template<
    class AsyncReadStream,
    class DynamicBuffer,
    class CompletionToken>
BOOST_ASIO_INITFN_RESULT_TYPE(
    CompletionToken,
    void(boost::beast::error_code, boost::tribool))
async_detect_ssl(
    AsyncReadStream& stream,
    DynamicBuffer& buffer,
    CompletionToken&& token)
{
    namespace beast = boost::beast;

    // Make sure arguments meet the requirements
    static_assert(beast::is_async_read_stream<AsyncReadStream>::value,
        "SyncReadStream requirements not met");
    static_assert(
        boost::asio::is_dynamic_buffer<DynamicBuffer>::value,
        "DynamicBuffer requirements not met");

    // This helper manages some of the handler's lifetime and
    // uses the result and handler specializations associated with
    // the completion token to help customize the return value.
    //
    boost::asio::async_completion<
        CompletionToken, void(beast::error_code, boost::tribool)> init{token};

    // Create the composed operation and launch it. This is a constructor
    // call followed by invocation of operator(). We use BOOST_ASIO_HANDLER_TYPE
    // to convert the completion token into the correct handler type,
    // allowing user defined specializations of the async result template
    // to take effect.
    //
    detect_ssl_op<
        AsyncReadStream,
        DynamicBuffer,
        BOOST_ASIO_HANDLER_TYPE(
            CompletionToken, void(beast::error_code, boost::tribool))>{
                stream, buffer, init.completion_handler}(beast::error_code{}, 0);

    // This hook lets the caller see a return value when appropriate.
    // For example this might return std::future<error_code, boost::tribool> if
    // CompletionToken is boost::asio::use_future.
    //
    // If a coroutine is used for the token, the return value from
    // this function will be the `boost::tribool` representing the result.
    //
    return init.result.get();
}

//]

//[example_core_detect_ssl_6

// Read from a stream to invoke is_tls_handshake asynchronously.
// This will be implemented using Asio's "stackless coroutines"
// which are based on macros forming a switch statement. The
// operation is derived from `coroutine` for this reason.
//
template<
    class AsyncReadStream,
    class DynamicBuffer,
    class Handler>
class detect_ssl_op : public boost::asio::coroutine
{
    // This composed operation has trivial state,
    // so it is just kept inside the class and can
    // be cheaply copied as needed by the implementation.

    AsyncReadStream& stream_;

    // Boost.Asio and the Networking TS require an object of
    // type executor_work_guard<T>, where T is the type of
    // executor returned by the stream's get_executor function,
    // to persist for the duration of the asynchronous operation.
    boost::asio::executor_work_guard<
        decltype(std::declval<AsyncReadStream&>().get_executor())> work_;

    DynamicBuffer& buffer_;
    Handler handler_;
    boost::tribool result_ = false;

public:
    // Boost.Asio requires that handlers are CopyConstructible.
    // The state for this operation is cheap to copy.
    detect_ssl_op(detect_ssl_op const&) = default;

    // The constructor just keeps references the callers varaibles.
    //
    template<class DeducedHandler>
    detect_ssl_op(
        AsyncReadStream& stream,
        DynamicBuffer& buffer,
        DeducedHandler&& handler)
        : stream_(stream)
        , work_(stream.get_executor())
        , buffer_(buffer)
        , handler_(std::forward<DeducedHandler>(handler))
    {
    }

    // Associated allocator support. This is Asio's system for
    // allowing the final completion handler to customize the
    // memory allocation strategy used for composed operation
    // states. A composed operation needs to use the same allocator
    // as the final handler. These declarations achieve that.

    using allocator_type =
        boost::asio::associated_allocator_t<Handler>;

    allocator_type
    get_allocator() const noexcept
    {
        return (boost::asio::get_associated_allocator)(handler_);
    }

    // Executor hook. This is Asio's system for customizing the
    // manner in which asynchronous completion handlers are invoked.
    // A composed operation needs to use the same executor to invoke
    // intermediate completion handlers as that used to invoke the
    // final handler.

    using executor_type = boost::asio::associated_executor_t<
        Handler, decltype(std::declval<AsyncReadStream&>().get_executor())>;

    executor_type get_executor() const noexcept
    {
        return (boost::asio::get_associated_executor)(handler_, stream_.get_executor());
    }

    // Our main entry point. This will get called as our
    // intermediate operations complete. Definition below.
    //
    void operator()(boost::beast::error_code ec, std::size_t bytes_transferred);
};

//]

//[example_core_detect_ssl_7

// detect_ssl_op is callable with the signature
// void(error_code, bytes_transferred),
// allowing `*this` to be used as a ReadHandler
//
template<
    class AsyncStream,
    class DynamicBuffer,
    class Handler>
void
detect_ssl_op<AsyncStream, DynamicBuffer, Handler>::
operator()(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    namespace beast = boost::beast;

    // This introduces the scope of the stackless coroutine
    BOOST_ASIO_CORO_REENTER(*this)
    {
        // There could already be data in the buffer
        // so we do this first, before reading from the stream.
        result_ = detail::is_tls_client_hello(buffer_.data());

        // If we got an answer, return it
        if(! boost::indeterminate(result_))
        {
            // We need to invoke the handler, but the guarantee
            // is that the handler will not be called before the
            // call to async_detect_ssl returns, so we must post
            // the operation to the executor. The helper function
            // `bind_handler` lets us bind arguments in a safe way
            // that preserves the type customization hooks of the
            // original handler.
            BOOST_ASIO_CORO_YIELD
            boost::asio::post(
                stream_.get_executor(),
                beast::bind_handler(std::move(*this), ec, 0));
        }
        else
        {
            // Loop until an error occurs or we get a definitive answer
            for(;;)
            {
                // Try to fill our buffer by reading from the stream
                BOOST_ASIO_CORO_YIELD
                        stream_.async_read_some(buffer_.prepare(beast::read_size(buffer_, 1536)), std::move(*this));

                // Check for an error
                if(ec)
                    break;

                // Commit what we read into the buffer's input area.
                buffer_.commit(bytes_transferred);

                // See if we can detect the handshake
                result_ = detail::is_tls_client_hello(buffer_.data());

                // If it is detected, call the handler
                if(! boost::indeterminate(result_))
                {
                    // We don't need bind_handler here because we were invoked
                    // as a result of an intermediate asynchronous operation.
                    break;
                }
            }
        }

        // Invoke the final handler.
        handler_(ec, result_);
    }
}

//]

#endif
