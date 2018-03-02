import time
from CAF import *

# global varibles definition
mycounter = 0
wait_time = 1
my_exit = False

def msg_handler(msg):
    global mycounter, my_exit
    mycounter += 1

    print("\n\tmsg_handler received ", msg, "; type = ", type(msg))
    print("\tmsg_handler function called ", mycounter, " times")
    
    if mycounter == 3:
        my_exit = True

def main():

    spawn_simple_actor(msg_handler)
    simple_actor = get_from_registry(atom("simple_act"))

    # INIT (this will start the callback mechanism)
    init_a = atom("init")
    send(simple_actor, atom("init"))

    # SEND VECTOR INT
    v = vector_int([6, 7, 42, 1234])
    # send(simple_actor, "foo", v)

    # SEND MESSAGES (a.k.a. lists and tuples)
    msg1 = [6, 7, 42, 1234]
    send(simple_actor, msg1)

    msg2 = (6, "foo", "bar", 18)
    send(simple_actor, msg2)

    msg3 = (6, "baz", 42, "1234", v)
    send(simple_actor, msg3)

    while True:
        time.sleep(wait_time)
        if my_exit == True:
            break

    print("\nkilling actor...")
    send(simple_actor, atom("quit"))

if __name__ == "__main__":
    main()
