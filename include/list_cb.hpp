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
    using F = std::function<void (Message &, Session &, list_cb &)>;
    using L = std::list<F>;

    friend class cb_invoker;

    explicit list_cb(){}

    explicit list_cb(const L & l) :
        invoke_l_(l), c_iter_{invoke_l_.cbegin()}, message_p{nullptr}, session_p{nullptr}
    {}

    explicit list_cb(L && l) noexcept :
        invoke_l_(std::move(l)), c_iter_{invoke_l_.cbegin()}, message_p{nullptr}, session_p{nullptr}
    {}

    void operator ()(std::size_t diff = 0){
        for(std::size_t i = 0; i < diff; i++){
            if(c_iter_ == invoke_l_.cend())
                break;

            c_iter_++;
            skip_target();
        }

        return exec_fwd(false);
    }

private:

    void exec(Message & message, Session & session) {
        message_p = std::addressof(message);
        session_p = std::addressof(session);
        return exec_fwd();
    }

    void exec_fwd(bool _first = true){
        if constexpr (Message::header_type::is_request::value){
            if(c_iter_ == invoke_l_.cend())
                c_iter_--;

            if(!_first)
                skip_target();
        }
        return (*c_iter_++) (*message_p, *session_p, *this);
    }

    void reset(){
        c_iter_ = invoke_l_.cbegin();
    }

    void skip_target(){
        auto current_target = message_p->target();
        std::size_t pos = current_target.find('/', 1);

        if(pos != -1){
            auto next_target = current_target.substr(pos);
            std::string next_t{next_target.begin(), next_target.end()};
            message_p->target(next_t);
        }
    }

    L invoke_l_;
    typename L::const_iterator c_iter_;
    Message * message_p;
    Session * session_p;

}; // list_cb class

} // namespace

#endif // LIST_CB_HPP
