/*
 * tk.h --
 *
 *	Declarations for Tk-related things that are visible
 *	outside of the Tk module itself.
 *
 * Copyright (c) 1989-1994 The Regents of the University of California.
 * Copyright (c) 1994 The Australian National University.
 * Copyright (c) 1994-1997 Sun Microsystems, Inc.
 * Copyright (c) 1998-1999 Scriptics Corporation.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tk.cpp,v 1.1.1.1 2003-06-22 09:31:22 min Exp $
 */




/*
 * When version numbers change here, you must also go into the following files
 * and update the version numbers:
 *
 * README
 * unix/configure.in
 * win/makefile.bc
 * win/makefile.vc
 * win/README
 * mac/README
 * library/tk.tcl	(Not for patch release updates)
 *
 * The release level should be  0 for alpha, 1 for beta, and 2 for
 * final/patch.  The release serial value is the number that follows the
 * "a", "b", or "p" in the patch level; for example, if the patch level
 * is 4.3b2, TK_RELEASE_SERIAL is 2.  It restarts at 1 whenever the
 * release level is changed, except for the final release, which should
 * be 0.
 *
 * You may also need to update some of these files when the numbers change
 * for the version of Tcl that this release of Tk is compiled against.
 */



/* 
 * A special definition used to allow this header file to be included 
 * in resource files.
 */



/*
 * The following definitions set up the proper options for Macintosh
 * compilers.  We use this method because there is no autoconf equivalent.
 */






//#   include <tcltk.ic>
//#   include <tcl.h>




done.
minInterpreterNode-Count=0
ValueDumper: 0
