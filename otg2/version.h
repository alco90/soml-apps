#ifndef OTG2_VERSION_H_
#define OTG2_VERSION_H_

#ifdef HAVE_CONFIG_H
# include <config.h>
# define OTG2_VERSION PACKAGE_VERSION
#else
/* This is a phony version number, but is unlikely to be ever used */
# define OTG2_VERSION "2.x.dev0"
#endif /* HAVE_CONFIG_H */

#define COPYRIGHT "Copyright (c) 2005-2007 WINLAB, 2007-2012 NICTA"

#endif /* OTG2_VERSION_H_ */
