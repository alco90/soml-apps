
#ifndef GENERATOR_H
#define GENERATOR_H

#include "otg2/source.h"

/** 
 * Generator is an interface for any componet which can generate 
 * a packet. It is usually called from a stream.
 */
class Generator: public ISource

{     
public:
  static Generator* create(const char* name);  
  static const char* getDefGeneratorName();
  static const char* listGenerators();
  
};



#endif
