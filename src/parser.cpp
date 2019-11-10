#include <vector>

#include "parser.hpp"
#include "math/util.hpp"
#include "tracer/point_light.hpp"
#include "tracer/rect_light.hpp"
#include "tracer/shapes/de_sphere.hpp"
#include "tracer/shapes/de_inf_spheres.hpp"
#include "tracer/shapes/de_mandelbulb.hpp"
#include "tracer/shapes/de_quad.hpp"

parser::parser() {}

Float parser::parse_float(const YAML::Node& node, const std::string& name) {
  try {
    return node[name].as<Float>();
  } catch (const std::exception& e) {
    throw parsing_error(node[name].Mark().line, "while parsing " + name + ": " + e.what());
  }
}

int parser::parse_int(const YAML::Node& node, const std::string& name) {
  try {
    return node[name].as<int>();
  } catch (const std::exception& e) {
    throw parsing_error(node[name].Mark().line, "while parsing " + name + ": " + e.what());
  }
}

std::string parser::parse_string(const YAML::Node& node, const std::string& name) {
  try {
    return node[name].as<std::string>();
  } catch (const std::exception& e) {
    throw parsing_error(node[name].Mark().line, "while parsing " + name + ": " + e.what());
  }
}

math::vector2i parser::parse_vector2i(
    const YAML::Node& node,
    const std::string& name)
{
  try {
    math::vector2i v;
    std::string value = node[name].as<std::string>();
    std::sscanf(value.c_str(), "%d%d", &v.x, &v.y);
    return v;
  } catch (const std::exception& e) {
    throw parsing_error(node[name].Mark().line, "while parsing " + name + ": " + e.what());
  }
}

math::vector3i parser::parse_vector3i(
    const YAML::Node& node,
    const std::string& name)
{
  try {
    math::vector3i v;
    std::string value = node[name].as<std::string>();
    std::sscanf(value.c_str(), "%d%d%d", &v.x, &v.y, &v.z);
    return v;
  } catch (const std::exception& e) {
    throw parsing_error(node[name].Mark().line, "while parsing " + name + ": " + e.what());
  }
}

math::vector4i parser::parse_vector4i(
    const YAML::Node& node,
    const std::string& name)
{
  try {
    math::vector4i v;
    std::string value = node[name].as<std::string>();
    std::sscanf(value.c_str(), "%d%d%d%d", &v.x, &v.y, &v.z, &v.w);
    return v;
  } catch (const std::exception& e) {
    throw parsing_error(node[name].Mark().line, "while parsing " + name + ": " + e.what());
  }
}

math::vector2f parser::parse_vector2f(
    const YAML::Node& node,
    const std::string& name)
{
  try {
    math::vector2f v;
    std::string value = node[name].as<std::string>();
    std::sscanf(value.c_str(), "%f%f", &v.x, &v.y);
    return v;
  } catch (const std::exception& e) {
    throw parsing_error(node[name].Mark().line, "while parsing " + name + ": " + e.what());
  }
}

math::vector3f parser::parse_vector3f(
    const YAML::Node& node,
    const std::string& name)
{
  try {
    math::vector3f v;
    std::string value = node[name].as<std::string>();
    std::sscanf(value.c_str(), "%f%f%f", &v.x, &v.y, &v.z);
    return v;
  } catch (const std::exception& e) {
    throw parsing_error(node[name].Mark().line, "while parsing " + name + ": " + e.what());
  }
}

math::vector4f parser::parse_vector4f(
    const YAML::Node& node,
    const std::string& name)
{
  try {
    math::vector4f v;
    std::string value = node[name].as<std::string>();
    std::sscanf(value.c_str(), "%f%f%f%f", &v.x, &v.y, &v.z, &v.w);
    return v;
  } catch (const std::exception& e) {
    throw parsing_error(node[name].Mark().line, "while parsing " + name + ": " + e.what());
  }
}

tracer::rgb_spectrum parser::parse_rgb_spectrum(
    const YAML::Node& node,
    const std::string& name)
{
  try {
    std::string value = node[name].as<std::string>();
    math::vector3f v;
    std::sscanf(value.c_str(), "%f%f%f", &v.x, &v.y, &v.z);
    return tracer::rgb_spectrum(v.x, v.y, v.z);
  } catch (const std::exception& e) {
    throw parsing_error(node[name].Mark().line, "while parsing " + name + ": " + e.what());
  }
}

math::tf::transform parser::parse_transform(const YAML::Node& tf_node) {
  if (!tf_node.IsDefined()) return math::tf::identity;
  if (tf_node.IsSequence()) {
    using namespace math;
    std::vector<tf::transform> transforms;
    for (size_t i = 0; i < tf_node.size(); ++i) {
      YAML::Node tf = tf_node[i];
      if (tf["translate"].IsDefined()) {
        transforms.push_back(tf::translate(parse_vector3f(tf, "translate")));
      } else if (tf["rotate"].IsDefined()) {
        YAML::Node rotate_node = tf["rotate"];
        if (rotate_node["axis"].IsDefined() && rotate_node["angle"].IsDefined()) {
          transforms.push_back(tf::rotate(
              parse_vector3f(rotate_node, "axis").normalized(),
              math::radians(parse_float(rotate_node, "angle"))
              ));
        } else {
          throw parsing_error(
              rotate_node.Mark().line,
              "both axis and angle must be provided for rotation"
              );
        }
      } else if (tf["scale"].IsDefined()) {
        transforms.push_back(tf::scale(parse_vector3f(tf, "scale")));
      } else {
        throw parsing_error(tf_node.Mark().line, "unknown transformation type");
      }
    }

    tf::transform ret(tf::identity);
    for (auto it = transforms.rbegin(); it != transforms.rend(); ++it) {
      ret = ret * (*it);
    }
    return ret;
  }

  throw parsing_error(tf_node.Mark().line, "`transform' must be a sequence");
}

tracer::materials::phong parser::parse_material(const YAML::Node& mat_node) {
  if (mat_node["phong"].IsDefined()) {
    YAML::Node phong_node = mat_node["phong"];
    if (phong_node["Kd"].IsDefined() && phong_node["Ks"].IsDefined()
        && phong_node["Es"].IsDefined() && phong_node["color"].IsDefined())
    {
      return tracer::materials::phong(
          parse_rgb_spectrum(phong_node, "color"),
          parse_float(phong_node, "Kd"),
          parse_float(phong_node, "Ks"),
          parse_float(phong_node, "Es")
          );
    } else {
      throw parsing_error(phong_node.Mark().line, "specify color, Kd, Ks and Es");
    }
  }

  throw parsing_error(mat_node.Mark().line, "only phong model is available right now");
}

std::shared_ptr<tracer::light_source> parser::parse_light_source(const YAML::Node& ls_node) {
  if (ls_node["type"].IsDefined()) {
    std::string type = parse_string(ls_node, "type");
    if (type == "point") {
      if (ls_node["color"].IsDefined()) {
        tracer::rgb_spectrum rgb = parse_rgb_spectrum(ls_node, "color");
        math::tf::transform tf = math::tf::identity;
        if (ls_node["transform"].IsDefined()) {
          tf = parse_transform(ls_node["transform"]);
        }
        return std::shared_ptr<tracer::light_source>(new tracer::point_light(tf, rgb));
      } else {
        throw parsing_error(ls_node.Mark().line, "light source `color' must be specified");
      }
    } else if (type == "rect") {
      if (ls_node["color"].IsDefined()) {
        tracer::rgb_spectrum rgb = parse_rgb_spectrum(ls_node, "color");
        math::tf::transform tf = math::tf::identity;
        if (ls_node["transform"].IsDefined()) {
          tf = parse_transform(ls_node["transform"]);
        }
        size_t spp = 1;
        if (ls_node["spp"].IsDefined()) {
          spp = parse_int(ls_node, "spp");
          if (spp < 1) throw parsing_error(ls_node.Mark().line, "`spp' must be larger than 0");
        }
        if (ls_node["p_min"].IsDefined() && ls_node["p_max"].IsDefined()) {
          return std::shared_ptr<tracer::light_source>(
              new tracer::rect_light(
                tf, rgb, parse_vector2f(ls_node, "p_min"), parse_vector2f(ls_node, "p_max"), spp
                )
              );
        } else {
          throw parsing_error(ls_node.Mark().line, "`p_min' and `p_max' must be specified");
        }
      } else {
        throw parsing_error(ls_node.Mark().line, "light source `color' must be specified");
      }
    } else {
      throw parsing_error(ls_node[type].Mark().line, "unknown light source type `" + type + "'");
    }
  }

  throw parsing_error(ls_node.Mark().line, "light source `type' must be specified");
}

std::shared_ptr<tracer::shape> parser::parse_shape(
    const YAML::Node& attr,
    const std::string& name)
{
  math::tf::transform tf = parse_transform(attr["transform"]);
  tracer::materials::phong phong = parse_material(attr["material"]);

  if (name == "de_sphere") {
    tracer::shape* shape = new tracer::shapes::de_sphere(tf, parse_float(attr, "radius"));
    shape->surface = phong;
    return std::shared_ptr<tracer::shape>(shape);
  } else if (name == "de_inf_spheres") {
    tracer::shape* shape = new tracer::shapes::de_inf_spheres(
        tf,
        parse_float(attr, "radius"),
        parse_float(attr, "cell")
        );
    shape->surface = phong;
    return std::shared_ptr<tracer::shape>(shape);
  } else if (name == "de_mandelbulb") {
    tracer::shape* shape = new tracer::shapes::de_mandelbulb(tf);
    shape->surface = phong;
    return std::shared_ptr<tracer::shape>(shape);
  } else if (name == "de_quad") {
    tracer::shape* shape = new tracer::shapes::de_quad(
        tf,
        parse_vector3f(attr, "a"),
        parse_vector3f(attr, "b"),
        parse_vector3f(attr, "c"),
        parse_vector3f(attr, "d")
        );
    shape->surface = phong;
    return std::shared_ptr<tracer::shape>(shape);
  }

  throw parsing_error(attr["shape"].Mark().line, "unknown shape `" + name + "'");
}
    
std::shared_ptr<tracer::camera::camera> parser::parse_camera(
    const YAML::Node& cam_node, const math::vector2i& img_res, math::point3f* eye_position)
{
  if (cam_node["type"].IsDefined()) {
    std::string type = parse_string(cam_node, "type");
    math::vector3f position(0.0f);
    math::vector3f lookat(0.0f, 0.0f, 1.0f);
    math::vector3f up(0.0f, 1.0f, 0.0f);
    Float near = 0.1f;
    Float far = 1000.0f;
    if (type == "perspective" || type == "persp" || type == "orthographic" || type == "ortho") {
      position = parse_vector3f(cam_node, "position");
      lookat = parse_vector3f(cam_node, "lookat");
      up = parse_vector3f(cam_node, "up");
      near = parse_float(cam_node, "near");
      far = parse_float(cam_node, "far");

      *eye_position = position;

      math::tf::transform tf_lookat = math::tf::look_at(lookat, position, up);

      if (type == "perspective" || type == "persp") {
        if (cam_node["fov"].IsDefined()) {
          Float fov = math::radians(parse_float(cam_node, "fov"));
          return std::shared_ptr<tracer::camera::camera>(
              new tracer::camera::persp(
                tf_lookat, img_res, { 1.0f, 1.0f }, near, far, fov, Float(img_res.x) / img_res.y
                )
              );
        } else {
          throw parsing_error(
              cam_node.Mark().line, "field of view `fov' must be specified for perspective camera"
              );
        }
      } else {
        // orthographic
        return std::shared_ptr<tracer::camera::camera>(
            new tracer::camera::ortho(tf_lookat, img_res, { 1.0f, 1.0f }, near, far)
            );
      }
    } else {
      throw parsing_error(cam_node["type"].Mark().line, "unknown camera type `" + type + "'");
    }
  }

  throw parsing_error(cam_node.Mark().line, "camera `type' must be specified");
}

std::shared_ptr<tracer::scene> parser::load_scene(
    const std::string& file,
    tracer::render_params* params)
{
  YAML::Node root = YAML::LoadFile(file);

  auto empty_scene = std::make_shared<tracer::scene>();

  // render options
  if (root["render"].IsDefined()) {
    YAML::Node render_config = root["render"];
    if (render_config["resolution"].IsDefined()) {
      math::vector2i img_res(0, 0);
      std::string value = render_config["resolution"].as<std::string>();
      std::sscanf(value.c_str(), "%dx%d", &img_res.x, &img_res.y);
      if (img_res.x < 64 || img_res.y < 64) {
        throw parsing_error(
            render_config["resolution"].Mark().line,
            "image resolution must be larger than or equal to 64x64");
      }
      params->img_res = img_res;
    }
    if (render_config["shadow_bias"].IsDefined()) {
      params->shadow_bias = parse_float(render_config, "shadow_bias");
    }
    if (render_config["spp"].IsDefined()) {
      params->spp = parse_int(render_config, "spp");
    }
    if (render_config["seed"].IsDefined()) {
      params->seed = parse_int(render_config, "seed");
    }
    if (render_config["tile_size"].IsDefined()) {
      params->tile_size = parse_vector2i(render_config, "tile_size");
    }
  }

  // intersect options
  if (root["intersect"].IsDefined()) {
    YAML::Node intersect_config = root["intersect"];
    if (intersect_config["hit_epsilon"].IsDefined()) {
      params->intersect_options.hit_epsilon = parse_float(intersect_config, "hit_epsilon");
    }
    if (intersect_config["normal_delta"].IsDefined()) {
      params->intersect_options.normal_delta = parse_float(intersect_config, "normal_delta");
    }
    if (intersect_config["max_iters"].IsDefined()) {
      params->intersect_options.trace_max_iters = parse_int(intersect_config, "max_iters");
    }
  }

  // define scene
  if (root["scene"].IsDefined()) {
    YAML::Node scene_config = root["scene"];

    if (scene_config["camera"].IsDefined()) {
      empty_scene->camera = parse_camera(
          scene_config["camera"], params->img_res, &params->eye_position
          );
    } else {
      throw parsing_error(scene_config.Mark().line, "`camera' must be specified");
    }

    if (scene_config["objects"].IsDefined()) {
      YAML::Node object_node = scene_config["objects"];
      if (object_node.IsSequence()) {
        for (size_t i = 0; i < object_node.size(); ++i) {
          YAML::Node object = object_node[i];
          if (object["shape"].IsDefined()) {
            std::string shape_name = object["shape"].as<std::string>();
            empty_scene->shapes.push_back(parse_shape(object, shape_name));
          } else {
            throw parsing_error(
                object_node.Mark().line,
                "an object's shape must be specified by attribute `shape'"
                );
          }
        }
      } else {
        throw parsing_error(object_node.Mark().line, "`objects' must be a sequence");
      }
    }

    if (scene_config["lights"].IsDefined()) {
      YAML::Node light_node = scene_config["lights"];
      if (light_node.IsSequence()) {
        for (size_t i = 0; i < light_node.size(); ++i) {
          empty_scene->light_sources.push_back(parse_light_source(light_node[i]));
        }
      } else {
        throw parsing_error(light_node.Mark().line, "`lights' must be a sequence");
      }
    } else {
      throw parsing_error(scene_config.Mark().line, "specify light source `lights'");
    }
  } else {
    throw parsing_error(root.Mark().line, "no `scene' defined");
  }

  return empty_scene;
}
