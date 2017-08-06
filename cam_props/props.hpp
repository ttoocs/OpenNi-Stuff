#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include <boost/filesystem.hpp>

#include <boost/filesystem/fstream.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>

#define PROP_FILE "camera_props.txt"

typedef std::pair<std::string, std::string> strPair;

class camera_props
{
  public:
  void set_file(std::string file);
  void load_props(std::string file);
  void save_props(std::string file);
  std::string get_prop(std::string prop);
  void set_prop(std::string prop, std::string val);
  
//  std::string filename;
  std::unordered_map<std::string,std::string> prop_map;

  private:
};

void camera_props::load_props(std::string file){
  if(boost::filesystem::exists(file)){

    std::ifstream ifs(file);

    boost::archive::text_iarchive  iarch(ifs);
    iarch >> prop_map;
      

  } else {
    std::cout << "camera_props:: FILE NOT FOUND." << std::endl;
  }
}

void camera_props::save_props(std::string file){

    std::ofstream ofs(file);  //Cannot be one-lined, as then it doesn't compile /shrug.
    boost::archive::text_oarchive oarch(ofs); 
    oarch << prop_map;
}

std::string camera_props::get_prop(std::string prop){
  auto tmp = prop_map.find(prop);
  if(tmp != prop_map.end())
    return tmp->first;
  return NULL;
}
void camera_props::set_prop(std::string prop, std::string val){
  prop_map.erase(prop);
  prop_map.insert(strPair  (prop,val));
}
