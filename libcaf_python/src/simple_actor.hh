#include <caf/event_based_actor.hpp>
#include <caf/atom.hpp>

using init_a         = caf::atom_constant<caf::atom("init")>;
using simple_actor_a = caf::atom_constant<caf::atom("simple_act")>;
using quit_a         = caf::atom_constant<caf::atom("quit")>;

class simple_actor : public caf::event_based_actor
{
public:
    simple_actor (caf::actor_config& ac)
        : caf::event_based_actor (ac)
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
            },
            [&] (caf::message& msg)
            {
                // TODO
                std::cout << "caf::message received" << std::endl;
                myqueue.push_back(std::move(msg));
            },
            [&] (const std::string& s)
            {
                std::cout << "within string handle\nreceived " << s << std::endl;
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
            [&] (quit_a)
            {
                home_system ().registry ().erase( simple_actor_a{} );
                std::cout << "simple_actor quit..\n";
                quit();
            }

        };
    }
private:
    std::vector<caf::message> myqueue;
};

caf::actor spawn_simple_actor (caf::actor_system & system)
{
    return system.spawn<simple_actor>();
}
