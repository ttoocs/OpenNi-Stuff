#include <iostream>
#include <string>
#include <unordered_map>

#include "props.hpp"

int main(){
  std::string test("Hello world!");

  camera_props props;
  
  props.set_prop(test,test);

  props.save_props("test_props_save");
  

  camera_props loadedProps;
  
  loadedProps.load_props("test_props_save");

  std::cout <<   loadedProps.get_prop(test) << std::endl;

/*  
  std::insert_iterator< pair > in_iterator(map,map.begin());
  
  const std::istream_iterator< pair > eos; //End of stream?
  std::istream_iterator< pair > is_iterator; 
  */
  
  
}
