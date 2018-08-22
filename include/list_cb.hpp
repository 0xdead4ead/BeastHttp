#ifndef LIST_CB_HPP
#define LIST_CB_HPP

#include <memory>
#include <functional>
#include <list>
#include <string>

namespace http {

template<class Message, class Session>
class list_cb{

public:

    using self_type = list_cb<Message,Session>;
    using ptr = std::shared_ptr<list_cb<Message, Session> >;
    using F = std::function<void (Message &, Session &, list_cb &)>;
    using L = std::list<F>;

    friend class cb_invoker;
    friend class exec_base;

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

            c_iter_++; cb_pos++;
            skip_target();
        }

#ifndef __cpp_if_constexpr
        return exec_fwd<self_type,Message::header_type::is_request::value>{}(*this, false);
#else
        return exec_fwd(false);
#endif
    }

    /// \brief Return actual position cb in list
    auto pos(){
        return cb_pos - 1;
    }

    const std::string & current_target(){
        return current_target_;
    }

private:

    void exec(Message & message, Session & session) {
        message_p = std::addressof(message);
        session_p = std::addressof(session);
#ifndef __cpp_if_constexpr
        return exec_fwd<self_type,Message::header_type::is_request::value>{}(*this);
#else
        return exec_fwd();
#endif
    }

#ifndef __cpp_if_constexpr
    struct exec_base{};

    template<typename T, bool IsRequest = true>
    struct exec_fwd : exec_base{
        void operator()(T & t, bool _first = true){
            if(_first)
                t.current_target_ = t.message_p->target().to_string();

            if(t.c_iter_ == t.invoke_l_.cend()){
                t.c_iter_--; t.cb_pos--;
            }

            if(t.invoke_l_.size() > 1)
                t.skip_target();

            t.cb_pos++;
            return (*t.c_iter_++) (*t.message_p, *t.session_p, t);
        }
    };

    template<typename T>
    struct exec_fwd<T,false> : exec_base{
        void operator()(T & t){
            t.cb_pos++;
            return (*t.c_iter_++) (*t.message_p, *t.session_p, t);
        }
    };
#else
    void exec_fwd(bool _first = true){
        if constexpr (Message::header_type::is_request::value){
            if(_first)
                current_target_ = message_p->target().to_string();

            if(c_iter_ == invoke_l_.cend()){
                c_iter_--; cb_pos--;
            }

            if(invoke_l_.size() > 1)
                skip_target();
        }
        cb_pos++;
        return (*c_iter_++) (*message_p, *session_p, *this);
    }
#endif
    void reset(){
        c_iter_ = invoke_l_.cbegin();
        cb_pos = 0;
    }

    void skip_target(){
        std::size_t pos = current_target_.find('/', 1);

        if(pos != std::string::npos){
            auto next_target = current_target_.substr(0, pos);
            current_target_ = current_target_.substr(pos);

            message_p->target(next_target);
        }else
            message_p->target(current_target_);

    }

    L invoke_l_;
    typename L::const_iterator c_iter_;
    Message * message_p;
    Session * session_p;
    std::string current_target_;
    std::size_t cb_pos = 0;

}; // list_cb class

} // namespace http

#endif // LIST_CB_HPP
