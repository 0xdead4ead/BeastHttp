#ifndef LIST_CB_HPP
#define LIST_CB_HPP

#include <memory>
#include <functional>
#include <list>

namespace http {

template<class Message, class Session>
class list_cb{

public:

    using ptr = std::shared_ptr<list_cb<Message, Session> >;
    using F = std::function<void (const Message &, Session &, list_cb &)>;
    using L = std::list<F>;

//    template<class... Callback>
//    explicit self_type(Callback && ... handlers) :
//        invoke_l_{boost::forward<Callback>(handlers)...}, c_iter_(invoke_l_.cbegin())
//    {}

    friend class cb_invoker;

    explicit list_cb(){}

    explicit list_cb(const L & l) :
        invoke_l_(l), c_iter_{invoke_l_.cbegin()}, message_cp{nullptr}, session_p{nullptr}
    {}

    explicit list_cb(L && l) noexcept :
        invoke_l_(std::move(l)), c_iter_{invoke_l_.cbegin()}, message_cp{nullptr}, session_p{nullptr}
    {}

    void operator ()(){
        return exec_fwd();
    }

private:

    void exec(const Message & message, Session & session) {
        message_cp = std::addressof(message);
        session_p = std::addressof(session);
        return exec_fwd();
    }

    void exec_fwd(){
        if(c_iter_ == invoke_l_.cend())
            return;
        else
            return (*c_iter_++) (*message_cp, *session_p, *this);
    }

    void reset(){
        c_iter_ = invoke_l_.cbegin();
    }

    L invoke_l_;
    typename L::const_iterator c_iter_;
    const Message * message_cp;
    Session * session_p;

}; // list_cb class

} // namespace

#endif // LIST_CB_HPP
