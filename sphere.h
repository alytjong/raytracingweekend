#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
  public:
    sphere() {}
    sphere(point3 cen, double r, shared_ptr<material> m)
      : center(cen), radius(r), mat_ptr(m) {}

    // Virtual function is a member function which is declared within a
    // base class and is re-defined(Overriden) by a derived class.
    // When you refer to a derived class object using a pointer or a reference
    // to the base class, you can call a virtual function for that object and
    // execute the derived class’s version of the function.
    virtual bool hit(
      const ray& r, double t_min, double t_max, hit_record& rec) const override;

  public:
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
  vec3 oc = r.origin() - center;
  auto a = r.direction().length_squared();
  auto half_b = dot(oc, r.direction());
  auto c = oc.length_squared() - radius*radius;

  auto discriminant = half_b*half_b - a*c;
  if (discriminant < 0) return false;
  auto sqrtd = sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range
  auto root = (-half_b - sqrtd)/a;
  if (root < t_min || t_max < root) {
    root = (-half_b + sqrtd)/a;
    if (root < t_min || t_max < root) return false;
  }

  rec.t = root;
  rec.p = r.at(rec.t);
  vec3 outward_normal = (rec.p - center)/radius;
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mat_ptr;

  return true;
}

#endif
