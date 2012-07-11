/*
 * Contains various factory methods for objects which may
 * be dynamically loaded.
 */

#include "cbr_generator.h"
#include "expo_generator.h"

Generator*
Generator::create(
  const char* name
) {
  if (strcmp(name, "cbr") == 0) {
    return new CBR_Generator();
  } else if (strcmp(name, "expo") == 0) {
    return new Expo_Generator();
  }
  return NULL;
}

const char* 
Generator::listGenerators()

{
  return "cbr|expo";
}


const char* 
Generator::getDefGeneratorName()

{
  return "cbr";
}
