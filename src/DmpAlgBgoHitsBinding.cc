#include <boost/python.hpp>
#include "DmpAlgBgoHits.h"

BOOST_PYTHON_MODULE(libDmpAlgBgoHits){
  using namespace boost::python;

  class_<DmpAlgBgoHits,boost::noncopyable,bases<DmpVAlg> >("DmpAlgBgoHits",init<>());
}
