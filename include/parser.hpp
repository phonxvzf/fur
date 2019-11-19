#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <yaml-cpp/yaml.h>

#include "tracer/scene.hpp"

class parser {
  private:
    Float parse_float(const YAML::Node& node, const std::string& name);
    int parse_int(const YAML::Node& node, const std::string& name);
    std::string parse_string(const YAML::Node& node, const std::string& name);
    math::vector2i parse_vector2i(const YAML::Node& node, const std::string& name);
    math::vector3i parse_vector3i(const YAML::Node& node, const std::string& name);
    math::vector4i parse_vector4i(const YAML::Node& node, const std::string& name);
    math::vector2f parse_vector2f(const YAML::Node& node, const std::string& name);
    math::vector3f parse_vector3f(const YAML::Node& node, const std::string& name);
    math::vector4f parse_vector4f(const YAML::Node& node, const std::string& name);
    tracer::rgb_spectrum parse_rgb_spectrum(
        const YAML::Node& node,
        const std::string& name
        );
    math::tf::transform parse_transform(const YAML::Node& tf_node);
    tracer::material::transport_type parse_transport_model(
        const YAML::Node& node, const std::string& name);
    std::shared_ptr<tracer::material> parse_material(const YAML::Node& mat_node);
    std::shared_ptr<tracer::shape> parse_shape(const YAML::Node& attr, const std::string& name);
    std::shared_ptr<tracer::camera::camera> parse_camera(
        const YAML::Node& cam_node, const math::vector2i& img_res, math::point3f* eye_position);

  public:

    parser();

    std::shared_ptr<tracer::scene> load_scene(
        const std::string& file,
        tracer::render_params* params
        );
};

class parsing_error : public std::runtime_error {
  private:
    std::string error_msg;

  public:
    parsing_error(int line_num, const std::string& msg) : std::runtime_error(msg) {
      error_msg = "parsing error at line " + std::to_string(line_num)
          + ": " + std::runtime_error::what();
    }
    
    const char* what() const noexcept override { 
      return error_msg.c_str();
    }
};

#endif /* PARSER_HPP */
