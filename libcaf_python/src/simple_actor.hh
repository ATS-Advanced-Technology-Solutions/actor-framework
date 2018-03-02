#include <caf/event_based_actor.hpp>
#include <caf/atom.hpp>

using init_a              = caf::atom_constant<caf::atom("init")>;
using simple_actor_a      = caf::atom_constant<caf::atom("simple_act")>;
using set_callback_func_a = caf::atom_constant<caf::atom("set_cb")>;
using exec_callback_a     = caf::atom_constant<caf::atom("exec_cb")>;
using quit_a              = caf::atom_constant<caf::atom("quit")>;

using msg_handler = std::function<void(caf::message)>;

class simple_actor : public caf::event_based_actor
{
public:
    simple_actor (caf::actor_config& ac, msg_handler& f)
        : caf::event_based_actor (ac),
        msg_handler_(f)
    {
        home_system ().registry ().put (simple_actor_a{}, caf::actor_cast<caf::strong_actor_ptr>(this));
    };

    const char* name () const override { return "simple_actor"; }
    caf::behavior make_behavior () override
    {
        return
        {
            [&] (init_a)
            {
                // TODO
                std::cout << "simple_actor init completed" << std::endl;
                delayed_send(this, std::chrono::seconds(1), exec_callback_a {});
            },
            [&] (caf::message& msg)
            {
                // TODO
                std::cout << "caf::message received by simple_actor" << std::endl;
                myqueue_.push_back(std::move(msg));
            },
            [&] (const std::string& s)
            {
                std::cout << "string handle[" << s << "] received by simple_actor" << std::endl;
            },
            [&] (std::string& s, std::vector<int>& v)
            {
                // perform some operations
                s += " bar";           
                v.push_back(42);

                // send message back
                send(caf::actor_cast<caf::actor>(current_mailbox_element()->sender), std::move(s), v);
                // send(caf::actor_cast<caf::actor>(current_mailbox_element()->sender), caf::make_message(std::move(s), v));
            },
            [&](exec_callback_a)
            {
                if(msg_handler_ != nullptr && !myqueue_.empty() )
                {
                    auto& msg = myqueue_.front();
                    msg_handler_( std::move(msg) );
                    myqueue_.pop_front();
                }
                delayed_send(this, std::chrono::seconds(1), exec_callback_a {});
            },
            [&] (quit_a)
            {
                home_system ().registry ().erase( simple_actor_a{} );
                std::cout << "simple_actor quit..\n";
                quit();
            }

        };
    }
private:
    std::deque<caf::message> myqueue_;
    msg_handler  msg_handler_;
};

caf::actor spawn_simple_actor (caf::actor_system & system, msg_handler& f)
{
    return system.spawn<simple_actor>(f);
}
