/*
 * Copyright 2004-2010 WINLAB, Rutgers University, USA
 * Copyright 2007-2014 National ICT Australia (NICTA)
 *
 * This software may be used and distributed solely under the terms of
 * the MIT license (License).  You should find a copy of the License in
 * COPYING or at http://opensource.org/licenses/MIT. By downloading or
 * using this software you accept the terms and the liability disclaimer
 * in the License.
 */

#ifndef OTG2_VERSION_H_
#define OTG2_VERSION_H_

#ifdef HAVE_CONFIG_H
# include "config.h"

#else
# define PACKAGE_STRING __FILE__ " " __DATE__
# define PACKAGE_NAME __FILE__
# define PACKAGE_VERSION __DATE__

#endif /* HAVE_CONFIG_H */
#define OTG2_VERSION PACKAGE_VERSION

#define COPYRIGHT "Copyright (c) 2005-2007 WINLAB, 2007-2014 NICTA"

#endif /* OTG2_VERSION_H_ */

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
