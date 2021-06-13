#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
  public:
    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) { objects.push_back(object); }

    // Virtual function is a member function which is declared within a
    // base class and is re-defined(Overriden) by a derived class.
    // When you refer to a derived class object using a pointer or a reference
    // to the base class, you can call a virtual function for that object and
    // execute the derived class’s version of the function.
    virtual bool hit(
      const ray& r, double t_min, double t_max, hit_record& rec) const override;

  public:
    std::vector<shared_ptr<hittable> > objects;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
  // hit function when there are multiple objects that can be hit by a single ray
  hit_record temp_rec;
  bool hit_anything = false;
  auto closest_so_far = t_max;

  // iterate through all objects in the list
  for (const auto& object : objects) {
    if (object -> hit(r, t_min, closest_so_far, temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }

  return hit_anything; // will only record hit to the closest object to the ray source
}

#endif
