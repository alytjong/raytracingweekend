#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

class material; // does this call material.h or is this just a placeholder?

struct hit_record {
  point3 p;
  vec3 normal;
  shared_ptr<material> mat_ptr;
  double t;
  bool front_face;

  inline void set_face_normal(const ray& r, const vec3& outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal :-outward_normal;
  }
};

class hittable {
  public:
    // Virtual function is a member function which is declared within a
    // base class and is re-defined(Overriden) by a derived class.
    // When you refer to a derived class object using a pointer or a reference
    // to the base class, you can call a virtual function for that object and
    // execute the derived class’s version of the function.
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif
