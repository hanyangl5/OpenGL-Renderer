#pragma once
#include <string>

enum class ObjectType {
  none_type,
  model_obj,
  model_gltf,
  model_fbx,
  texture_jpg,
  texture_png,
  texture_bmp,
  texture_tga,
  texture_psd
};

// Dregen Object
class DObject {
public:
  std::string GetTypeStr() {
    switch (type) {

    case ObjectType::model_obj:
      return "model_obj";
    case ObjectType::model_gltf:
      return "model_gltf";
    case ObjectType::model_fbx:
      return "model_fbx";
    case ObjectType::texture_jpg:
      return "texture_jpg";
    case ObjectType::texture_png:
      return "texture_png";
    case ObjectType::texture_bmp:
      return "texture_bmp";
    case ObjectType::texture_tga:
      return "texture_tga";
    case ObjectType::texture_psd:
      return "texture_psd";
    default:
      return "something wrong";
    }
  }
  DObject(std::string n, std::string dir, ObjectType t)
      : name(n), type(t), directory(dir) {}
  std::string name{};
  std::string directory{};
  ObjectType type = ObjectType::none_type;
};