#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "aabb.h"

class triangle : public hittable {
public:
    triangle() {}
    triangle(const point3& v0, const point3& v1, const point3& v2, shared_ptr<material> m)
        : v0(v0), v1(v1), v2(v2), mat_ptr(m) {
        normal = unit_vector(cross(v1 - v0, v2 - v0));
    }

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        //使用了 moller-trumbore 算法来判断光线是否与三角形相交
        auto e1 = v1 - v0;
        auto e2 = v2 - v0;
        auto h = cross(r.direction(), e2);
        auto a = dot(e1, h);

        if (a > -epsilon && a < epsilon)
            return false;

        auto f = 1 / a;
        auto s = r.origin() - v0;
        auto u = f * dot(s, h);
        if (u < 0 || u > 1)
            return false;

        auto q = cross(s, e1);
        auto v = f * dot(r.direction(), q);
        if (v < 0 || u + v > 1)
            return false;

        auto t = f * dot(e2, q);
        if (t < ray_t.min || t > ray_t.max)
            return false;

        rec.t = t;
        rec.p = r.at(t);
        rec.normal = normal;
        rec.mat = mat_ptr;

        return true;
    }

    virtual aabb bounding_box() const override {
        point3 small(fmin(fmin(v0.x(), v1.x()), v2.x()),
            fmin(fmin(v0.y(), v1.y()), v2.y()),
            fmin(fmin(v0.z(), v1.z()), v2.z()));

        point3 big(fmax(fmax(v0.x(), v1.x()), v2.x()),
            fmax(fmax(v0.y(), v1.y()), v2.y()),
            fmax(fmax(v0.z(), v1.z()), v2.z()));

        return aabb(small, big);
    }

    virtual double pdf_value(const point3& o, const vec3& v) const override {
        hit_record rec;
        if (!this->hit(ray(o, v), interval(0.001, infinity), rec))
            return 0;

        auto area = 0.5 * cross(v1 - v0, v2 - v0).length();
        auto distance_squared = rec.t * rec.t * v.length_squared();
        auto cosine = fabs(dot(v, rec.normal) / v.length());

        return distance_squared / (cosine * area);
    }

    virtual vec3 random(const point3& o) const override {
        auto random_point = v0 + random_double() * (v1 - v0) + random_double() * (v2 - v0);
        return random_point - o;
    }

    //读取obj文件并添加到一个hittable_liat 中，这个函数暂时不用，因为返回一个hittable_list 我不知道怎么转换为 shared_ptr<hittable>
    static hittable_list load_obj_model(const std::string& filename, std::shared_ptr<material> mat) {
        std::vector<point3> vertices;
        std::vector<vec3> normals;
        std::vector<vec3> texCoords;
        std::vector<std::vector<int>> faces;

        ReadOBJ::loadOBJ(filename, vertices, normals, texCoords, faces);

        hittable_list obj_triangles;
        for (const auto& face : faces) {
            for (size_t i = 0; i < face.size() - 2; ++i) {
                int v0_idx = face[0], v1_idx = face[i + 1], v2_idx = face[i + 2];
                obj_triangles.add(make_shared<triangle>(
                    vertices[v0_idx], vertices[v1_idx], vertices[v2_idx], mat));
            }
        }

        return obj_triangles;
    }


    //读取obj文件并添加到一个hittable_liat 中,引用传参
    static hittable_list load_obj_model(const std::string & filename, std::shared_ptr<material> mat, hittable_list & world) {
        std::vector<point3> vertices;
        std::vector<vec3> normals;
        std::vector<vec3> texCoords;
        std::vector<std::vector<int>> faces;

        ReadOBJ::loadOBJ(filename, vertices, normals, texCoords, faces);

        //hittable_list obj_triangles;
        for (const auto& face : faces) {
            for (size_t i = 0; i < face.size() - 2; ++i) {
                int v0_idx = face[0], v1_idx = face[i + 1], v2_idx = face[i + 2];
                world.add(make_shared<triangle>(
                    vertices[v0_idx], vertices[v1_idx], vertices[v2_idx], mat));
            }
        }

        return world;
    }


private:
    point3 v0, v1, v2;//顶点
    vec3 normal;//法向
    shared_ptr<material> mat_ptr;//指向材质对象的指针
    const double epsilon = 0.0000001; // 极小值，用于控制数值计算精度
};

#endif
