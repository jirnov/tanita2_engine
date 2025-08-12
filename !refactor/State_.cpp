#include <precompiled.h>
#include "State_.h"
#include "Python_.h"

bp::object empty_function( bp::tuple /*args*/, bp::dict /*kwargs*/ )
{
    return bp::object();
}

void exportState( void )
{
    using namespace bp;
    using namespace ingame;

    class_<State>("State", init<bp::object, bp::object, bp::object, bp::object, bp::object>(
                               (arg("sequence"), arg("on_enter") = bp::object(), 
                                arg("on_update") = bp::object(), arg("on_exit") = bp::object(),
                                arg("link") = bp::object())))
        .def_readwrite("sequence",  &State::sequence)
        .add_property("on_enter",  &State::get_on_enter, &State::set_on_enter)
        .add_property("on_update",  &State::get_on_update, &State::set_on_update)
        .add_property("on_exit",  &State::get_on_exit, &State::set_on_exit)
        .add_property("link",  &State::get_link, &State::set_link)
        ;

    def("empty_function", raw_function(empty_function));
}

using namespace ingame;

State::State( bp::object sequence, bp::object on_enter, 
              bp::object on_update, bp::object on_exit,
              bp::object link )
{
    python::Python python;    
    weakmeth = python["weakmethod"].attr("WeakMethod");
    emptyfunc = bp::object(raw_function(empty_function));

    this->sequence = sequence;
    this->on_enter = emptyfunc;
    this->on_update = emptyfunc;
    this->on_exit = emptyfunc;
    this->link = emptyfunc;
}

void State::set_on_enter( bp::object on_enter )
{
    this->on_enter = (on_enter == bp::object()) ? emptyfunc : weakmeth(on_enter);
}

void State::set_on_update( bp::object on_update )
{
    this->on_update = (on_update == bp::object()) ? emptyfunc : weakmeth(on_update);
}

void State::set_on_exit( bp::object on_exit )
{
    this->on_exit = (on_exit == bp::object()) ? emptyfunc : weakmeth(on_exit);
}

void State::set_link( bp::object link )
{
    this->link = (link == bp::object()) ? emptyfunc : weakmeth(link);
}
