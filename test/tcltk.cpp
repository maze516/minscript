/*
 * tcl.h --
 *
 *	This header file describes the externally-visible facilities
 *	of the Tcl interpreter.
 *
 * Copyright (c) 1987-1994 The Regents of the University of California.
 * Copyright (c) 1994-1997 Sun Microsystems, Inc.
 * Copyright (c) 1993-1996 Lucent Technologies.
 * Copyright (c) 1998-1999 Scriptics Corporation.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tcltk.cpp,v 1.1.1.1 2003-06-22 09:31:22 min Exp $
 */






/*
 * When version numbers change here, must also go into the following files
 * and update the version numbers:
 *
 * README
 * library/init.tcl	(only if major.minor changes, not patchlevel)
 * unix/configure.in
 * win/makefile.bc	(only if major.minor changes, not patchlevel)
 * win/makefile.vc	(only if major.minor changes, not patchlevel)
 * win/README
 * win/README.binary
 * mac/README
 *
 * The release level should be  0 for alpha, 1 for beta, and 2 for
 * final/patch.  The release serial value is the number that follows the
 * "a", "b", or "p" in the patch level; for example, if the patch level
 * is 7.6b2, TCL_RELEASE_SERIAL is 2.  It restarts at 1 whenever the
 * release level is changed, except for the final release which is 0
 * (the first patch will start at 1).
 */



/*
 * The following definitions set up the proper options for Windows
 * compilers.  We use this method because there is no autoconf equivalent.
 */










 /* __WIN32__ */

/*
 * The following definitions set up the proper options for Macintosh
 * compilers.  We use this method because there is no autoconf equivalent.
 */




/*
 * Utility macros: STRINGIFY takes an argument and wraps it in "" (double
 * quotation marks), JOIN joins two arguments.
 */








/* 
 * A special definition used to allow this header file to be included 
 * in resource files so that they can get obtain version information from
 * this file.  Resource compilers don't like all the C stuff, like typedefs
 * and procedure declarations, that occur below.
 */




//#include <stdio.h>


/*
 * Definitions that allow Tcl functions with variable numbers of
 * arguments to be used with either varargs.h or stdarg.h.  TCL_VARARGS
 * is used in procedure prototypes.  TCL_VARARGS_DEF is used to declare
 * the arguments in a function definiton: it takes the type and name of
 * the first argument and supplies the appropriate argument declaration
 * string for use in the function definition.  TCL_VARARGS_START
 * initializes the va_list data structure and returns the first argument.
 */




/*
 * Macros used to declare a function to be exported by a DLL.
 * Used by Windows, maps to no-op declarations on non-Windows systems.
 * The default build on windows is for a DLL, which causes the DLLIMPORT
 * and DLLEXPORT macros to be nonempty. To build a static library, the
 * macro STATIC_BUILD should be defined.
 * The support follows the convention that a macro called BUILD_xxxx, where
 * xxxx is the name of a library we are building, is set on the compile line
 * for sources that are to be placed in the library. See BUILD_tcl in this
 * file for an example of how the macro is to be used.
 */















/*
 * Definitions that allow this header file to be used either with or
 * without ANSI C features like function prototypes.
 */




//#message _ANSI_ARGS_ 1





/*
 * Macro to use instead of "void" for arguments that must have
 * type "void *" in ANSI C;  maps them to type "char *" in
 * non-ANSI systems.
 */





 /* __WIN32__ */

/*
 * Miscellaneous declarations.
 */





    typedef void *ClientData;
 /* __STDC__ */



/*
 * Data structures defined opaquely in this module. The definitions below
 * just provide dummy types. A few fields are made visible in Tcl_Interp
 * structures, namely those used for returning a string result from
 * commands. Direct access to the result field is discouraged in Tcl 8.0.
 * The interpreter result is either an object or a string, and the two
 * values are kept consistent unless some C code sets interp->result
 * directly. Programmers should use either the procedure Tcl_GetObjResult()
 * or Tcl_GetStringResult() to read the interpreter's result. See the
 * SetResult man page for details.
 * 
 * Note: any change to the Tcl_Interp definition below must be mirrored
 * in the "real" definition in tclInt.h.
 *
 * Note: Tcl_ObjCmdProc procedures do not directly set result and freeProc.
 * Instead, they set a Tcl_Obj member in the "real" structure that can be
 * accessed with Tcl_GetObjResult() and Tcl_SetObjResult().
 */

typedef struct Tcl_Interp {
    char *result;		/* If the last command returned a string
				 * result, this points to it. */
    void (*freeProc) (char *blockPtr);
				/* Zero means the string result is
				 * statically allocated. TCL_DYNAMIC means
				 * it was allocated with ckalloc and should
				 * be freed with ckfree. Other values give
				 * the address of procedure to invoke to
				 * free the result. Tcl_Eval must free it
				 * before executing next command. */
    int errorLine;              /* When TCL_ERROR is returned, this gives
                                 * the line number within the command where
                                 * the error occurred (1 if first line). */
} Tcl_Interp;

/* TODO
typedef struct Tcl_AsyncHandler_ *Tcl_AsyncHandler;
typedef struct Tcl_Channel_ *Tcl_Channel;
typedef struct Tcl_Command_ *Tcl_Command;
typedef struct Tcl_Event Tcl_Event;
typedef struct Tcl_Pid_ *Tcl_Pid;
typedef struct Tcl_RegExp_ *Tcl_RegExp;
typedef struct Tcl_TimerToken_ *Tcl_TimerToken;
typedef struct Tcl_Trace_ *Tcl_Trace;
typedef struct Tcl_Var_ *Tcl_Var;
*/
struct Tcl_AsyncHandler_;
typedef Tcl_AsyncHandler_ *Tcl_AsyncHandler;
struct Tcl_Channel_;
typedef Tcl_Channel_ *Tcl_Channel;
struct Tcl_Command_;
typedef Tcl_Command_ *Tcl_Command;
struct Tcl_Event;
struct Tcl_Pid_;
typedef Tcl_Pid_ *Tcl_Pid;
struct Tcl_RegExp_;
typedef Tcl_RegExp_ *Tcl_RegExp;
struct Tcl_TimerToken_;
typedef Tcl_TimerToken_ *Tcl_TimerToken;
struct Tcl_Trace_;
typedef Tcl_Trace_ *Tcl_Trace;
struct Tcl_Var_;
typedef Tcl_Var_ *Tcl_Var;


/*
 * When a TCL command returns, the interpreter contains a result from the
 * command. Programmers are strongly encouraged to use one of the
 * procedures Tcl_GetObjResult() or Tcl_GetStringResult() to read the
 * interpreter's result. See the SetResult man page for details. Besides
 * this result, the command procedure returns an integer code, which is 
 * one of the following:
 *
 * TCL_OK		Command completed normally; the interpreter's
 *			result contains	the command's result.
 * TCL_ERROR		The command couldn't be completed successfully;
 *			the interpreter's result describes what went wrong.
 * TCL_RETURN		The command requests that the current procedure
 *			return; the interpreter's result contains the
 *			procedure's return value.
 * TCL_BREAK		The command requests that the innermost loop
 *			be exited; the interpreter's result is meaningless.
 * TCL_CONTINUE		Go on to the next iteration of the current loop;
 *			the interpreter's result is meaningless.
 */



/*
 * Argument descriptors for math function callbacks in expressions:
 */

//TODO typedef enum {TCL_INT, TCL_DOUBLE, TCL_EITHER} Tcl_ValueType;
typedef int Tcl_ValueType;
typedef struct Tcl_Value {
    Tcl_ValueType type;		/* Indicates intValue or doubleValue is
				 * valid, or both. */
    long intValue;		/* Integer value. */
    double doubleValue;		/* Double-precision floating value. */
} Tcl_Value;

/*
 * Forward declaration of Tcl_Obj to prevent an error when the forward
 * reference to Tcl_Obj is encountered in the procedure types declared 
 * below.
 */

struct Tcl_Obj;

/*
 * Procedure types defined by Tcl:
 */

typedef int (Tcl_AppInitProc) (Tcl_Interp *interp);
typedef int (Tcl_AsyncProc) (ClientData clientData,
	Tcl_Interp *interp, int code);
typedef void (Tcl_ChannelProc) (ClientData clientData, int mask);
typedef void (Tcl_CloseProc) (ClientData data);
typedef void (Tcl_CmdDeleteProc) (ClientData clientData);
typedef int (Tcl_CmdProc) (ClientData clientData,
	Tcl_Interp *interp, int argc, char **argv);
//	Tcl_Interp *interp, int argc, char *argv[]);
typedef void (Tcl_CmdTraceProc) (ClientData clientData,
	Tcl_Interp *interp, int level, char *command, Tcl_CmdProc *proc,
	ClientData cmdClientData, int argc, char **argv);
//TODO	ClientData cmdClientData, int argc, char *argv[]);
typedef void (Tcl_DupInternalRepProc) (struct Tcl_Obj *srcPtr, 
        struct Tcl_Obj *dupPtr);
typedef int (Tcl_EventProc) (Tcl_Event *evPtr, int flags);
typedef void (Tcl_EventCheckProc) (ClientData clientData,
	int flags);
typedef int (Tcl_EventDeleteProc) (Tcl_Event *evPtr,
        ClientData clientData);
typedef void (Tcl_EventSetupProc) (ClientData clientData,
	int flags);
typedef void (Tcl_ExitProc) (ClientData clientData);
typedef void (Tcl_FileProc) (ClientData clientData, int mask);
typedef void (Tcl_FileFreeProc) (ClientData clientData);
typedef void (Tcl_FreeInternalRepProc) (struct Tcl_Obj *objPtr);
typedef void (Tcl_FreeProc) (char *blockPtr);
typedef void (Tcl_IdleProc) (ClientData clientData);
typedef void (Tcl_InterpDeleteProc) (ClientData clientData,
	Tcl_Interp *interp);
typedef int (Tcl_MathProc) (ClientData clientData,
	Tcl_Interp *interp, Tcl_Value *args, Tcl_Value *resultPtr);
typedef void (Tcl_NamespaceDeleteProc) (ClientData clientData);
typedef int (Tcl_ObjCmdProc) (ClientData clientData,
	Tcl_Interp *interp, int objc, struct Tcl_Obj * * objv);
//	Tcl_Interp *interp, int objc, struct Tcl_Obj * const objv[]);
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);
typedef void (Tcl_TcpAcceptProc) (ClientData callbackData,
        Tcl_Channel chan, char *address, int port);
typedef void (Tcl_TimerProc) (ClientData clientData);
typedef int (Tcl_SetFromAnyProc) (Tcl_Interp *interp,
	struct Tcl_Obj *objPtr);
typedef void (Tcl_UpdateStringProc) (struct Tcl_Obj *objPtr);
typedef char *(Tcl_VarTraceProc) (ClientData clientData,
	Tcl_Interp *interp, char *part1, char *part2, int flags);

/*
 * The following structure represents a type of object, which is a
 * particular internal representation for an object plus a set of
 * procedures that provide standard operations on objects of that type.
 */

typedef struct Tcl_ObjType {
    char *name;			/* Name of the type, e.g. "int". */
    Tcl_FreeInternalRepProc *freeIntRepProc;
				/* Called to free any storage for the type's
				 * internal rep. NULL if the internal rep
				 * does not need freeing. */
    Tcl_DupInternalRepProc *dupIntRepProc;
    				/* Called to create a new object as a copy
				 * of an existing object. */
    Tcl_UpdateStringProc *updateStringProc;
    				/* Called to update the string rep from the
				 * type's internal representation. */
    Tcl_SetFromAnyProc *setFromAnyProc;
    				/* Called to convert the object's internal
				 * rep to this type. Frees the internal rep
				 * of the old type. Returns TCL_ERROR on
				 * failure. */
} Tcl_ObjType;

/*
 * One of the following structures exists for each object in the Tcl
 * system. An object stores a value as either a string, some internal
 * representation, or both.
 */

typedef struct Tcl_Obj {
    int refCount;		/* When 0 the object will be freed. */
    char *bytes;		/* This points to the first byte of the
				 * object's string representation. The array
				 * must be followed by a null byte (i.e., at
				 * offset length) but may also contain
				 * embedded null characters. The array's
				 * storage is allocated by ckalloc. NULL
				 * means the string rep is invalid and must
				 * be regenerated from the internal rep.
				 * Clients should use Tcl_GetStringFromObj
				 * to get a pointer to the byte array as a
				 * readonly value. */
    int length;			/* The number of bytes at *bytes, not
				 * including the terminating null. */
    Tcl_ObjType *typePtr;	/* Denotes the object's type. Always
				 * corresponds to the type of the object's
				 * internal rep. NULL indicates the object
				 * has no internal rep (has no type). */
// TODO
/*
    union {			
	long longValue;		
	double doubleValue;	
	void *otherValuePtr;
	struct {
	    void *ptr1;
	    void *ptr2;
	} twoPtrValue;
    } internalRep;
*/
} Tcl_Obj;

/*
 * Macros to increment and decrement a Tcl_Obj's reference count, and to
 * test whether an object is shared (i.e. has reference count > 1).
 * Note: clients should use Tcl_DecrRefCount() when they are finished using
 * an object, and should never call TclFreeObj() directly. TclFreeObj() is
 * only defined and made public in tcl.h to support Tcl_DecrRefCount's macro
 * definition. Note also that Tcl_DecrRefCount() refers to the parameter
 * "obj" twice. This means that you should avoid calling it with an
 * expression that is expensive to compute or has side effects.
 */

extern  void		Tcl_IncrRefCount (Tcl_Obj *objPtr);
extern  void		Tcl_DecrRefCount (Tcl_Obj *objPtr);
extern  int		Tcl_IsShared (Tcl_Obj *objPtr);




/*
 * Macros and definitions that help to debug the use of Tcl objects.
 * When TCL_MEM_DEBUG is defined, the Tcl_New* declarations are 
 * overridden to call debugging versions of the object creation procedures.
 */

extern  Tcl_Obj *	Tcl_NewBooleanObj (int boolValue);
extern  Tcl_Obj *	Tcl_NewDoubleObj (double doubleValue);
extern  Tcl_Obj *	Tcl_NewIntObj (int intValue);
extern  Tcl_Obj *	Tcl_NewListObj (int objc,
			    Tcl_Obj * * objv);
//			    Tcl_Obj *const objv[]);
extern  Tcl_Obj *	Tcl_NewLongObj (long longValue);
extern  Tcl_Obj *	Tcl_NewObj (void);
extern  Tcl_Obj *	Tcl_NewStringObj (char *bytes,
			    int length);

 /* TCL_MEM_DEBUG */

/*
 * The following definitions support Tcl's namespace facility.
 * Note: the first five fields must match exactly the fields in a
 * Namespace structure (see tcl.h). 
 */

typedef struct Tcl_Namespace {
    char *name;                 /* The namespace's name within its parent
				 * namespace. This contains no ::'s. The
				 * name of the global namespace is ""
				 * although "::" is an synonym. */
    char *fullName;             /* The namespace's fully qualified name.
				 * This starts with ::. */
    ClientData clientData;      /* Arbitrary value associated with this
				 * namespace. */
    Tcl_NamespaceDeleteProc* deleteProc;
                                /* Procedure invoked when deleting the
				 * namespace to, e.g., free clientData. */
//TODO    struct Tcl_Namespace* parentPtr;
                                /* Points to the namespace that contains
				 * this one. NULL if this is the global
				 * namespace. */
} Tcl_Namespace;

/*
 * The following structure represents a call frame, or activation record.
 * A call frame defines a naming context for a procedure call: its local
 * scope (for local variables) and its namespace scope (used for non-local
 * variables; often the global :: namespace). A call frame can also define
 * the naming context for a namespace eval or namespace inscope command:
 * the namespace in which the command's code should execute. The
 * Tcl_CallFrame structures exist only while procedures or namespace
 * eval/inscope's are being executed, and provide a Tcl call stack.
 * 
 * A call frame is initialized and pushed using Tcl_PushCallFrame and
 * popped using Tcl_PopCallFrame. Storage for a Tcl_CallFrame must be
 * provided by the Tcl_PushCallFrame caller, and callers typically allocate
 * them on the C call stack for efficiency. For this reason, Tcl_CallFrame
 * is defined as a structure and not as an opaque token. However, most
 * Tcl_CallFrame fields are hidden since applications should not access
 * them directly; others are declared as "dummyX".
 *
 * WARNING!! The structure definition must be kept consistent with the
 * CallFrame structure in tclInt.h. If you change one, change the other.
 */

typedef struct Tcl_CallFrame {
    Tcl_Namespace *nsPtr;
    int dummy1;
    int dummy2;
    char *dummy3;
    char *dummy4;
    char *dummy5;
    int dummy6;
    char *dummy7;
    char *dummy8;
    int dummy9;
    char* dummy10;
} Tcl_CallFrame;

/*
 * Information about commands that is returned by Tcl_GetCommandInfo and
 * passed to Tcl_SetCommandInfo. objProc is an objc/objv object-based
 * command procedure while proc is a traditional Tcl argc/argv
 * string-based procedure. Tcl_CreateObjCommand and Tcl_CreateCommand
 * ensure that both objProc and proc are non-NULL and can be called to
 * execute the command. However, it may be faster to call one instead of
 * the other. The member isNativeObjectProc is set to 1 if an
 * object-based procedure was registered by Tcl_CreateObjCommand, and to
 * 0 if a string-based procedure was registered by Tcl_CreateCommand.
 * The other procedure is typically set to a compatibility wrapper that
 * does string-to-object or object-to-string argument conversions then
 * calls the other procedure.
 */
     
typedef struct Tcl_CmdInfo {
    int isNativeObjectProc;	 /* 1 if objProc was registered by a call to
				  * Tcl_CreateObjCommand; 0 otherwise.
				  * Tcl_SetCmdInfo does not modify this
				  * field. */
    Tcl_ObjCmdProc *objProc;	 /* Command's object-based procedure. */
    ClientData objClientData;	 /* ClientData for object proc. */
    Tcl_CmdProc *proc;		 /* Command's string-based procedure. */
    ClientData clientData;	 /* ClientData for string proc. */
    Tcl_CmdDeleteProc *deleteProc;
                                 /* Procedure to call when command is
                                  * deleted. */
    ClientData deleteData;	 /* Value to pass to deleteProc (usually
				  * the same as clientData). */
    Tcl_Namespace *namespacePtr; /* Points to the namespace that contains
				  * this command. Note that Tcl_SetCmdInfo
				  * will not change a command's namespace;
				  * use Tcl_RenameCommand to do that. */

} Tcl_CmdInfo;

/*
 * The structure defined below is used to hold dynamic strings.  The only
 * field that clients should use is the string field, and they should
 * never modify it.
 */

typedef struct Tcl_DString {
    char *sstring;		/* Points to beginning of string:  either
				 * staticSpace below or a malloced array. */
    int length;			/* Number of non-NULL characters in the
				 * string. */
    int spaceAvl;		/* Total number of bytes available for the
				 * string and its terminating NULL char. */
    char staticSpace[200];
				/* Space to use in common case where string
				 * is small. */
} Tcl_DString;


/*
 * Definitions for the maximum number of digits of precision that may
 * be specified in the "tcl_precision" variable, and the number of
 * characters of buffer space required by Tcl_PrintDouble.
 */
 

/*
 * Flag that may be passed to Tcl_ConvertElement to force it not to
 * output braces (careful!  if you change this flag be sure to change
 * the definitions at the front of tclUtil.c).
 */


/*
 * Flag that may be passed to Tcl_GetIndexFromObj to force it to disallow
 * abbreviated strings.
 */


/*
 * Flag values passed to Tcl_RecordAndEval.
 * WARNING: these bit choices must not conflict with the bit choices
 * for evalFlag bits in tclInt.h!!
 */


/*
 * Special freeProc values that may be passed to Tcl_SetResult (see
 * the man page for details):
 */


/*
 * Flag values passed to variable-related procedures.
 */


/*
 * Types for linked variables:
 */


/*
 * The following declarations either map ckalloc and ckfree to
 * malloc and free, or they map them to procedures with all sorts
 * of debugging hooks defined in tclCkalloc.c.
 */

extern  char *		Tcl_Alloc (unsigned int size);
extern  void		Tcl_Free (char *ptr);
extern  char *		Tcl_Realloc (char *ptr,
			    unsigned int size);



/*
 * If USE_TCLALLOC is true, then we need to call Tcl_Alloc instead of
 * the native malloc/free.  The only time USE_TCLALLOC should not be
 * true is when compiling the Tcl/Tk libraries on Unix systems.  In this
 * case we can safely call the native malloc/free directly as a performance
 * optimization.
 */



 /* TCL_MEM_DEBUG */

/*
 * Forward declaration of Tcl_HashTable.  Needed by some C++ compilers
 * to prevent errors when the forward reference to Tcl_HashTable is
 * encountered in the Tcl_HashEntry structure.
 */



/*
 * Structure definition for an entry in a hash table.  No-one outside
 * Tcl should access any of these fields directly;  use the macros
 * defined below.
 */

typedef struct Tcl_HashEntry {
//    struct Tcl_HashEntry *nextPtr;	/* Pointer to next entry in this
//					 * hash bucket, or NULL for end of
//					 * chain. */
//    struct Tcl_HashTable *tablePtr;	/* Pointer to table containing entry. */
//    struct Tcl_HashEntry **bucketPtr;	/* Pointer to bucket that points to
//					 * first entry in this entry's chain:
//					 * used for deleting the entry. */
    ClientData clientData;		/* Application stores something here
					 * with Tcl_SetHashValue. */
//    union {				/* Key has one of these forms: */
//	char *oneWordValue;		/* One-word value for key. */
//	int words[1];			/* Multiple integer words for key.
//					 * The actual size will be as large
//					 * as necessary for this table's
//					 * keys. */
//	char string[4];			/* String for key.  The actual size
//					 * will be as large as needed to hold
//					 * the key. */
//    } key;				/* MUST BE LAST FIELD IN RECORD!! */
} Tcl_HashEntry;

/*
 * Structure definition for a hash table.  Must be in tcl.h so clients
 * can allocate space for these structures, but clients should never
 * access any fields in this structure.
 */

typedef struct Tcl_HashTable {
    Tcl_HashEntry **buckets;		/* Pointer to bucket array.  Each
					 * element points to first entry in
					 * bucket's hash chain, or NULL. */
    Tcl_HashEntry *staticBuckets[4];
					/* Bucket array used for small tables
					 * (to avoid mallocs and frees). */
    int numBuckets;			/* Total number of buckets allocated
					 * at **bucketPtr. */
    int numEntries;			/* Total number of entries present
					 * in table. */
    int rebuildSize;			/* Enlarge table when numEntries gets
					 * to be this large. */
    int downShift;			/* Shift count used in hashing
					 * function.  Designed to use high-
					 * order bits of randomized keys. */
    int mask;				/* Mask value used in hashing
					 * function. */
    int keyType;			/* Type of keys used in this table. 
					 * It's either TCL_STRING_KEYS,
					 * TCL_ONE_WORD_KEYS, or an integer
					 * giving the number of ints that
                                         * is the size of the key.
					 */
//TODO
//    Tcl_HashEntry *(*findProc) (struct Tcl_HashTable *tablePtr,
//	    const char *key);
//    Tcl_HashEntry *(*createProc) (struct Tcl_HashTable *tablePtr,
//		const char *key, int *newPtr);
} Tcl_HashTable;

/*
 * Structure definition for information used to keep track of searches
 * through hash tables:
 */

typedef struct Tcl_HashSearch {
    Tcl_HashTable *tablePtr;		/* Table being searched. */
    int nextIndex;			/* Index of next bucket to be
					 * enumerated after present one. */
    Tcl_HashEntry *nextEntryPtr;	/* Next entry to be enumerated in the
					 * the current bucket. */
} Tcl_HashSearch;

/*
 * Acceptable key types for hash tables:
 */


/*
 * Macros for clients to use to access fields of hash entries:
 */


/*
 * Macros to use for clients to use to invoke find and create procedures
 * for hash tables:
 */


/*
 * Flag values to pass to Tcl_DoOneEvent to disable searches
 * for some kinds of events:
 */


/*
 * The following structure defines a generic event for the Tcl event
 * system.  These are the things that are queued in calls to Tcl_QueueEvent
 * and serviced later by Tcl_DoOneEvent.  There can be many different
 * kinds of events with different fields, corresponding to window events,
 * timer events, etc.  The structure for a particular event consists of
 * a Tcl_Event header followed by additional information specific to that
 * event.
 */

struct Tcl_Event {
    Tcl_EventProc *proc;	/* Procedure to call to service this event. */
//    struct Tcl_Event *nextPtr;	/* Next in list of pending events, or NULL. */
};

/*
 * Positions to pass to Tcl_QueueEvent:
 */

//typedef enum {
//    TCL_QUEUE_TAIL, TCL_QUEUE_HEAD, TCL_QUEUE_MARK
//} Tcl_QueuePosition;

/*
 * Values to pass to Tcl_SetServiceMode to specify the behavior of notifier
 * event routines.
 */


/*
 * The following structure keeps is used to hold a time value, either as
 * an absolute time (the number of seconds from the epoch) or as an
 * elapsed time. On Unix systems the epoch is Midnight Jan 1, 1970 GMT.
 * On Macintosh systems the epoch is Midnight Jan 1, 1904 GMT.
 */

typedef struct Tcl_Time {
    long sec;			/* Seconds. */
    long usec;			/* Microseconds. */
} Tcl_Time;

/*
 * Bits to pass to Tcl_CreateFileHandler and Tcl_CreateChannelHandler
 * to indicate what sorts of events are of interest:
 */


/*
 * Flag values to pass to Tcl_OpenCommandChannel to indicate the
 * disposition of the stdio handles.  TCL_STDIN, TCL_STDOUT, TCL_STDERR,
 * are also used in Tcl_GetStdChannel.
 */


/*
 * Typedefs for the various operations in a channel type:
 */

typedef int	(Tcl_DriverBlockModeProc) (
		    ClientData instanceData, int mode);
typedef int	(Tcl_DriverCloseProc) (ClientData instanceData,
		    Tcl_Interp *interp);
typedef int	(Tcl_DriverInputProc) (ClientData instanceData,
		    char *buf, int toRead, int *errorCodePtr);
typedef int	(Tcl_DriverOutputProc) (ClientData instanceData,
		    char *buf, int toWrite, int *errorCodePtr);
typedef int	(Tcl_DriverSeekProc) (ClientData instanceData,
		    long offset, int mode, int *errorCodePtr);
typedef int	(Tcl_DriverSetOptionProc) (
		    ClientData instanceData, Tcl_Interp *interp,
	            char *optionName, char *value);
typedef int	(Tcl_DriverGetOptionProc) (
		    ClientData instanceData, Tcl_Interp *interp,
		    char *optionName, Tcl_DString *dsPtr);
typedef void	(Tcl_DriverWatchProc) (
		    ClientData instanceData, int mask);
typedef int	(Tcl_DriverGetHandleProc) (
		    ClientData instanceData, int direction,
		    ClientData *handlePtr);

/*
 * Enum for different end of line translation and recognition modes.
 */

//typedef enum Tcl_EolTranslation {
//    TCL_TRANSLATE_AUTO,			/* Eol == \r, \n and \r\n. */
//    TCL_TRANSLATE_CR,			/* Eol == \r. */
//    TCL_TRANSLATE_LF,			/* Eol == \n. */
//    TCL_TRANSLATE_CRLF			/* Eol == \r\n. */
//} Tcl_EolTranslation;

/*
 * struct Tcl_ChannelType:
 *
 * One such structure exists for each type (kind) of channel.
 * It collects together in one place all the functions that are
 * part of the specific channel type.
 */

typedef struct Tcl_ChannelType {
    char *typeName;			/* The name of the channel type in Tcl
                                         * commands. This storage is owned by
                                         * channel type. */
    Tcl_DriverBlockModeProc *blockModeProc;
    					/* Set blocking mode for the
                                         * raw channel. May be NULL. */
    Tcl_DriverCloseProc *closeProc;	/* Procedure to call to close
                                         * the channel. */
    Tcl_DriverInputProc *inputProc;	/* Procedure to call for input
                                         * on channel. */
    Tcl_DriverOutputProc *outputProc;	/* Procedure to call for output
                                         * on channel. */
    Tcl_DriverSeekProc *seekProc;	/* Procedure to call to seek
                                         * on the channel. May be NULL. */
    Tcl_DriverSetOptionProc *setOptionProc;
    					/* Set an option on a channel. */
    Tcl_DriverGetOptionProc *getOptionProc;
    					/* Get an option from a channel. */
    Tcl_DriverWatchProc *watchProc;	/* Set up the notifier to watch
                                         * for events on this channel. */
    Tcl_DriverGetHandleProc *getHandleProc;
					/* Get an OS handle from the channel
                                         * or NULL if not supported. */
    void *reserved;			/* reserved for future expansion */
} Tcl_ChannelType;

/*
 * The following flags determine whether the blockModeProc above should
 * set the channel into blocking or nonblocking mode. They are passed
 * as arguments to the blockModeProc procedure in the above structure.
 */


/*
 * Enum for different types of file paths.
 */
/*
typedef enum Tcl_PathType {
    TCL_PATH_ABSOLUTE,
    TCL_PATH_RELATIVE,
    TCL_PATH_VOLUME_RELATIVE
} Tcl_PathType;
*/
/*
 * Exported Tcl procedures:
 */

extern  void		Tcl_AddErrorInfo (Tcl_Interp *interp,
			    char *message);
extern  void		Tcl_AddObjErrorInfo (Tcl_Interp *interp,
			    char *message, int length);
extern  void		Tcl_AllowExceptions (Tcl_Interp *interp);
extern  int		Tcl_AppendAllObjTypes (
			    Tcl_Interp *interp, Tcl_Obj *objPtr);
extern  void		Tcl_AppendElement (Tcl_Interp *interp,
			    char *sstring);
extern  void		Tcl_AppendResult ();
extern  void		Tcl_AppendToObj (Tcl_Obj *objPtr,
			    char *bytes, int length);
extern  void		Tcl_AppendStringsToObj ();
extern  Tcl_AsyncHandler	Tcl_AsyncCreate (Tcl_AsyncProc *proc,
			    ClientData clientData);
extern  void		Tcl_AsyncDelete (Tcl_AsyncHandler async);
extern  int		Tcl_AsyncInvoke (Tcl_Interp *interp,
			    int code);
extern  void		Tcl_AsyncMark (Tcl_AsyncHandler async);
extern  int		Tcl_AsyncReady (void);
extern  void		Tcl_BackgroundError (Tcl_Interp *interp);
extern  char		Tcl_Backslash (const char *src,
			    int *readPtr);
extern  int		Tcl_BadChannelOption (Tcl_Interp *interp,
			    char *optionName, char *optionList);
extern  void		Tcl_CallWhenDeleted (Tcl_Interp *interp,
			    Tcl_InterpDeleteProc *proc,
			    ClientData clientData);
extern  void		Tcl_CancelIdleCall (Tcl_IdleProc *idleProc,
			    ClientData clientData);
extern  int		Tcl_Close (Tcl_Interp *interp,
        		    Tcl_Channel chan);
extern  int		Tcl_CommandComplete (char *cmd);
extern  char *		Tcl_Concat (int argc, char **argv);
extern  Tcl_Obj *	Tcl_ConcatObj (int objc,
			    Tcl_Obj * * objv);
//	    Tcl_Obj *const objv[]);
extern  int		Tcl_ConvertCountedElement (const char *src,
			    int length, char *dst, int flags);
extern  int		Tcl_ConvertElement (const char *src,
			    char *dst, int flags);
extern  int		Tcl_ConvertToType (Tcl_Interp *interp,
			    Tcl_Obj *objPtr, Tcl_ObjType *typePtr);
extern  int		Tcl_CreateAlias (Tcl_Interp *slave,
			    char *slaveCmd, Tcl_Interp *target,
        		    char *targetCmd, int argc, char **argv);
extern  int		Tcl_CreateAliasObj (Tcl_Interp *slave,
			    char *slaveCmd, Tcl_Interp *target,
        		    char *targetCmd, int objc,
		            Tcl_Obj * * objv);
//		            Tcl_Obj *const objv[]);
extern  Tcl_Channel	Tcl_CreateChannel (
    			    Tcl_ChannelType *typePtr, char *chanName,
                            ClientData instanceData, int mask);
extern  void		Tcl_CreateChannelHandler (
			    Tcl_Channel chan, int mask,
                            Tcl_ChannelProc *proc, ClientData clientData);
extern  void		Tcl_CreateCloseHandler (
			    Tcl_Channel chan, Tcl_CloseProc *proc,
                            ClientData clientData);
extern  Tcl_Command	Tcl_CreateCommand (Tcl_Interp *interp,
			    char *cmdName, Tcl_CmdProc *proc,
			    ClientData clientData,
			    Tcl_CmdDeleteProc *deleteProc);
extern  void		Tcl_CreateEventSource (
			    Tcl_EventSetupProc *setupProc,
			    Tcl_EventCheckProc *checkProc,
			    ClientData clientData);
extern  void		Tcl_CreateExitHandler (Tcl_ExitProc *proc,
			    ClientData clientData);
extern  void		Tcl_CreateFileHandler (
    			    int fd, int mask, Tcl_FileProc *proc,
			    ClientData clientData);
extern  Tcl_Interp *	Tcl_CreateInterp (void);
extern  void		Tcl_CreateMathFunc (Tcl_Interp *interp,
			    char *name, int numArgs, Tcl_ValueType *argTypes,
			    Tcl_MathProc *proc, ClientData clientData);
extern  Tcl_Command	Tcl_CreateObjCommand (
			    Tcl_Interp *interp, char *cmdName,
			    Tcl_ObjCmdProc *proc, ClientData clientData,
			    Tcl_CmdDeleteProc *deleteProc);
extern  Tcl_Interp *	Tcl_CreateSlave (Tcl_Interp *interp,
		            char *slaveName, int isSafe);
extern  Tcl_TimerToken	Tcl_CreateTimerHandler (int milliseconds,
			    Tcl_TimerProc *proc, ClientData clientData);
extern  Tcl_Trace	Tcl_CreateTrace (Tcl_Interp *interp,
			    int level, Tcl_CmdTraceProc *proc,
			    ClientData clientData);
extern  char *		Tcl_DbCkalloc (unsigned int size,
			    char *file, int line);
extern  int		Tcl_DbCkfree (char *ptr,
			    char *file, int line);
extern  char *		Tcl_DbCkrealloc (char *ptr,
			    unsigned int size, char *file, int line);
extern  void		Tcl_DbDecrRefCount (Tcl_Obj *objPtr,
			    char *file, int line);
extern  void		Tcl_DbIncrRefCount (Tcl_Obj *objPtr,
			    char *file, int line);
extern  int		Tcl_DbIsShared (Tcl_Obj *objPtr,
			    char *file, int line);
extern  Tcl_Obj *	Tcl_DbNewBooleanObj (int boolValue,
                            char *file, int line);
extern  Tcl_Obj *	Tcl_DbNewDoubleObj (double doubleValue,
                            char *file, int line);
extern  Tcl_Obj *	Tcl_DbNewListObj (int objc,
			    Tcl_Obj * * objv, char *file, int line);
//			    Tcl_Obj *const objv[], char *file, int line);
extern  Tcl_Obj *	Tcl_DbNewLongObj (long longValue,
                            char *file, int line);
extern  Tcl_Obj *	Tcl_DbNewObj (char *file, int line);
extern  Tcl_Obj *	Tcl_DbNewStringObj (char *bytes,
			    int length, char *file, int line);
extern  void		Tcl_DeleteAssocData (Tcl_Interp *interp,
                            char *name);
extern  int		Tcl_DeleteCommand (Tcl_Interp *interp,
			    char *cmdName);
extern  int		Tcl_DeleteCommandFromToken (
			    Tcl_Interp *interp, Tcl_Command command);
extern  void		Tcl_DeleteChannelHandler (
    			    Tcl_Channel chan, Tcl_ChannelProc *proc,
                            ClientData clientData);
extern  void		Tcl_DeleteCloseHandler (
			    Tcl_Channel chan, Tcl_CloseProc *proc,
                            ClientData clientData);
extern  void		Tcl_DeleteEvents (
			    Tcl_EventDeleteProc *proc,
                            ClientData clientData);
extern  void		Tcl_DeleteEventSource (
			    Tcl_EventSetupProc *setupProc,
			    Tcl_EventCheckProc *checkProc,
			    ClientData clientData);
extern  void		Tcl_DeleteExitHandler (Tcl_ExitProc *proc,
			    ClientData clientData);
extern  void		Tcl_DeleteFileHandler (int fd);
extern  void		Tcl_DeleteHashEntry (
			    Tcl_HashEntry *entryPtr);
extern  void		Tcl_DeleteHashTable (
			    Tcl_HashTable *tablePtr);
extern  void		Tcl_DeleteInterp (Tcl_Interp *interp);
extern  void		Tcl_DeleteTimerHandler (
			    Tcl_TimerToken token);
extern  void		Tcl_DeleteTrace (Tcl_Interp *interp,
			    Tcl_Trace trace);
extern  void		Tcl_DetachPids (int numPids, Tcl_Pid *pidPtr);
extern  void		Tcl_DontCallWhenDeleted (
			    Tcl_Interp *interp, Tcl_InterpDeleteProc *proc,
			    ClientData clientData);
extern  int		Tcl_DoOneEvent (int flags);
extern  void		Tcl_DoWhenIdle (Tcl_IdleProc *proc,
			    ClientData clientData);
extern  char *		Tcl_DStringAppend (Tcl_DString *dsPtr,
			    const char *sstring, int length);
extern  char *		Tcl_DStringAppendElement (
			    Tcl_DString *dsPtr, const char *sstring);
extern  void		Tcl_DStringEndSublist (Tcl_DString *dsPtr);
extern  void		Tcl_DStringFree (Tcl_DString *dsPtr);
extern  void		Tcl_DStringGetResult (Tcl_Interp *interp,
			    Tcl_DString *dsPtr);
extern  void		Tcl_DStringInit (Tcl_DString *dsPtr);
extern  void		Tcl_DStringResult (Tcl_Interp *interp,
			    Tcl_DString *dsPtr);
extern  void		Tcl_DStringSetLength (Tcl_DString *dsPtr,
			    int length);
extern  void		Tcl_DStringStartSublist (
			    Tcl_DString *dsPtr);
extern  Tcl_Obj *	Tcl_DuplicateObj (Tcl_Obj *objPtr);
extern  int		Tcl_Eof (Tcl_Channel chan);
extern  char *		Tcl_ErrnoId (void);
extern  char *		Tcl_ErrnoMsg (int err);
extern  int		Tcl_Eval (Tcl_Interp *interp,
			    char *sstring);
extern  int		Tcl_EvalFile (Tcl_Interp *interp,
			    char *fileName);
extern  void		Tcl_EventuallyFree (ClientData clientData,
			    Tcl_FreeProc *freeProc);
extern  int		Tcl_EvalObj (Tcl_Interp *interp,
			    Tcl_Obj *objPtr);
extern  void		Tcl_Exit (int status);
extern  int		Tcl_ExposeCommand (Tcl_Interp *interp,
        		    char *hiddenCmdToken, char *cmdName);
extern  int		Tcl_ExprBoolean (Tcl_Interp *interp,
			    char *sstring, int *ptr);
extern  int		Tcl_ExprBooleanObj (Tcl_Interp *interp,
			    Tcl_Obj *objPtr, int *ptr);
extern  int		Tcl_ExprDouble (Tcl_Interp *interp,
			    char *sstring, double *ptr);
extern  int		Tcl_ExprDoubleObj (Tcl_Interp *interp,
			    Tcl_Obj *objPtr, double *ptr);
extern  int		Tcl_ExprLong (Tcl_Interp *interp,
			    char *sstring, long *ptr);
extern  int		Tcl_ExprLongObj (Tcl_Interp *interp,
			    Tcl_Obj *objPtr, long *ptr);
extern  int		Tcl_ExprObj (Tcl_Interp *interp,
			    Tcl_Obj *objPtr, Tcl_Obj **resultPtrPtr);
extern  int		Tcl_ExprString (Tcl_Interp *interp,
			    char *sstring);
extern  void		Tcl_Finalize (void);
extern  void		Tcl_FindExecutable (char *argv0);
extern  Tcl_HashEntry *	Tcl_FirstHashEntry (
			    Tcl_HashTable *tablePtr,
			    Tcl_HashSearch *searchPtr);
extern  int		Tcl_Flush (Tcl_Channel chan);
extern  void		TclFreeObj (Tcl_Obj *objPtr);
extern  void		Tcl_FreeResult (Tcl_Interp *interp);
extern  int		Tcl_GetAlias (Tcl_Interp *interp,
       			    char *slaveCmd, Tcl_Interp **targetInterpPtr,
                            char **targetCmdPtr, int *argcPtr,
			    char ***argvPtr);
extern  int		Tcl_GetAliasObj (Tcl_Interp *interp,
       			    char *slaveCmd, Tcl_Interp **targetInterpPtr,
                            char **targetCmdPtr, int *objcPtr,
			    Tcl_Obj ***objv);
extern  ClientData	Tcl_GetAssocData (Tcl_Interp *interp,
                            char *name, Tcl_InterpDeleteProc **procPtr);
extern  int		Tcl_GetBoolean (Tcl_Interp *interp,
			    char *sstring, int *boolPtr);
extern  int		Tcl_GetBooleanFromObj (
			    Tcl_Interp *interp, Tcl_Obj *objPtr,
			    int *boolPtr);
extern  Tcl_Channel	Tcl_GetChannel (Tcl_Interp *interp,
	        	    char *chanName, int *modePtr);
extern  int		Tcl_GetChannelBufferSize (
    			    Tcl_Channel chan);
extern  int		Tcl_GetChannelHandle (Tcl_Channel chan,
	        	    int direction, ClientData *handlePtr);
extern  ClientData	Tcl_GetChannelInstanceData (
    			    Tcl_Channel chan);
extern  int		Tcl_GetChannelMode (Tcl_Channel chan);
extern  char *		Tcl_GetChannelName (Tcl_Channel chan);
extern  int		Tcl_GetChannelOption (Tcl_Interp *interp,
			    Tcl_Channel chan, char *optionName,
			    Tcl_DString *dsPtr);
extern  Tcl_ChannelType * Tcl_GetChannelType (Tcl_Channel chan);
extern  int		Tcl_GetCommandInfo (Tcl_Interp *interp,
			    char *cmdName, Tcl_CmdInfo *infoPtr);
extern  char *		Tcl_GetCommandName (Tcl_Interp *interp,
			    Tcl_Command command);
extern  int		Tcl_GetDouble (Tcl_Interp *interp,
			    char *sstring, double *doublePtr);
extern  int		Tcl_GetDoubleFromObj (
			    Tcl_Interp *interp, Tcl_Obj *objPtr,
			    double *doublePtr);
extern  int		Tcl_GetErrno (void);
extern  char *		Tcl_GetHostName (void);
extern  int		Tcl_GetIndexFromObj (Tcl_Interp *interp,
			    Tcl_Obj *objPtr, char **tablePtr, char *msg,
			    int flags, int *indexPtr);
extern  int		Tcl_GetInt (Tcl_Interp *interp,
			    char *sstring, int *intPtr);
extern  int		Tcl_GetInterpPath (Tcl_Interp *askInterp,
			    Tcl_Interp *slaveInterp);
extern  int		Tcl_GetIntFromObj (Tcl_Interp *interp,
			    Tcl_Obj *objPtr, int *intPtr);
extern  int		Tcl_GetLongFromObj (Tcl_Interp *interp,
			    Tcl_Obj *objPtr, long *longPtr);
extern  Tcl_Interp *	Tcl_GetMaster (Tcl_Interp *interp);
extern  /*const*/ char *	Tcl_GetNameOfExecutable (void);
extern  Tcl_Obj *	Tcl_GetObjResult (Tcl_Interp *interp);
extern  Tcl_ObjType *	Tcl_GetObjType (char *typeName);
extern  int		Tcl_GetOpenFile (Tcl_Interp *interp,
			    char *sstring, int write, int checkUsage,
			    ClientData *filePtr);
extern  Tcl_PathType	Tcl_GetPathType (char *path);
extern  int		Tcl_Gets (Tcl_Channel chan,
        		    Tcl_DString *dsPtr);
extern  int		Tcl_GetsObj (Tcl_Channel chan,
        		    Tcl_Obj *objPtr);
extern  int		Tcl_GetServiceMode (void);
extern  Tcl_Interp *	Tcl_GetSlave (Tcl_Interp *interp,
			    char *slaveName);
extern  Tcl_Channel	Tcl_GetStdChannel (int type);
extern  char *		Tcl_GetStringFromObj (Tcl_Obj *objPtr,
			    int *lengthPtr);
extern  char *		Tcl_GetStringResult (Tcl_Interp *interp);
extern  char *		Tcl_GetVar (Tcl_Interp *interp,
			    char *varName, int flags);
extern  char *		Tcl_GetVar2 (Tcl_Interp *interp,
			    char *part1, char *part2, int flags);
extern  int		Tcl_GlobalEval (Tcl_Interp *interp,
			    char *command);
extern  int		Tcl_GlobalEvalObj (Tcl_Interp *interp,
			    Tcl_Obj *objPtr);
extern  char *		Tcl_HashStats (Tcl_HashTable *tablePtr);
extern  int		Tcl_HideCommand (Tcl_Interp *interp,
		            char *cmdName, char *hiddenCmdToken);
extern  int		Tcl_Init (Tcl_Interp *interp);
extern  void		Tcl_InitHashTable (Tcl_HashTable *tablePtr,
			    int keyType);
extern  void		Tcl_InitMemory (Tcl_Interp *interp);
extern  int		Tcl_InputBlocked (Tcl_Channel chan);
extern  int		Tcl_InputBuffered (Tcl_Channel chan);
extern  int		Tcl_InterpDeleted (Tcl_Interp *interp);
extern  int		Tcl_IsSafe (Tcl_Interp *interp);
extern  void		Tcl_InvalidateStringRep (
			    Tcl_Obj *objPtr);
extern  char *		Tcl_JoinPath (int argc, char **argv,
			    Tcl_DString *resultPtr);
extern  int		Tcl_LinkVar (Tcl_Interp *interp,
			    char *varName, char *addr, int type);
extern  int		Tcl_ListObjAppendList (
			    Tcl_Interp *interp, Tcl_Obj *listPtr, 
			    Tcl_Obj *elemListPtr);
extern  int		Tcl_ListObjAppendElement (
			    Tcl_Interp *interp, Tcl_Obj *listPtr,
			    Tcl_Obj *objPtr);
extern  int		Tcl_ListObjGetElements (
			    Tcl_Interp *interp, Tcl_Obj *listPtr,
			    int *objcPtr, Tcl_Obj ***objvPtr);
extern  int		Tcl_ListObjIndex (Tcl_Interp *interp,
			    Tcl_Obj *listPtr, int index, 
			    Tcl_Obj **objPtrPtr);
extern  int		Tcl_ListObjLength (Tcl_Interp *interp,
			    Tcl_Obj *listPtr, int *intPtr);
extern  int		Tcl_ListObjReplace (Tcl_Interp *interp,
			    Tcl_Obj *listPtr, int first, int count,
			    int objc, Tcl_Obj * * objv);
//			    int objc, Tcl_Obj *const objv[]);
extern  void		Tcl_Main (int argc, char **argv,
			    Tcl_AppInitProc *appInitProc);
extern  Tcl_Channel	Tcl_MakeFileChannel (ClientData handle,
			    int mode);
extern  int		Tcl_MakeSafe (Tcl_Interp *interp);
extern  Tcl_Channel	Tcl_MakeTcpClientChannel (
    			    ClientData tcpSocket);
extern  char *		Tcl_Merge (int argc, char **argv);
extern  Tcl_HashEntry *	Tcl_NextHashEntry (
			    Tcl_HashSearch *searchPtr);
extern  void		Tcl_NotifyChannel (Tcl_Channel channel,
			    int mask);
extern  Tcl_Obj *	Tcl_ObjGetVar2 (Tcl_Interp *interp,
			    Tcl_Obj *part1Ptr, Tcl_Obj *part2Ptr,
			    int flags);
extern  Tcl_Obj *	Tcl_ObjSetVar2 (Tcl_Interp *interp,
			    Tcl_Obj *part1Ptr, Tcl_Obj *part2Ptr,
			    Tcl_Obj *newValuePtr, int flags);
extern  Tcl_Channel	Tcl_OpenCommandChannel (
    			    Tcl_Interp *interp, int argc, char **argv,
			    int flags);
extern  Tcl_Channel	Tcl_OpenFileChannel (Tcl_Interp *interp,
        		    char *fileName, char *modeString,
                            int permissions);
extern  Tcl_Channel	Tcl_OpenTcpClient (Tcl_Interp *interp,
			    int port, char *address, char *myaddr,
		            int myport, int async);
extern  Tcl_Channel	Tcl_OpenTcpServer (Tcl_Interp *interp,
		            int port, char *host,
        		    Tcl_TcpAcceptProc *acceptProc,
			    ClientData callbackData);
extern  char *		Tcl_ParseVar (Tcl_Interp *interp,
			    char *sstring, char **termPtr);
extern  int		Tcl_PkgProvide (Tcl_Interp *interp,
			    char *name, char *version);
extern  char *		Tcl_PkgRequire (Tcl_Interp *interp,
			    char *name, char *version, int exact);
extern  char *		Tcl_PosixError (Tcl_Interp *interp);
extern  void		Tcl_Preserve (ClientData data);
extern  void		Tcl_PrintDouble (Tcl_Interp *interp,
			    double value, char *dst);
extern  int		Tcl_PutEnv (const char *sstring);
//extern  void		Tcl_QueueEvent (Tcl_Event *evPtr, Tcl_QueuePosition position);
extern  int		Tcl_Read (Tcl_Channel chan,
	        	    char *bufPtr, int toRead);
extern  void		Tcl_ReapDetachedProcs (void);
extern  int		Tcl_RecordAndEval (Tcl_Interp *interp,
			    char *cmd, int flags);
extern  int		Tcl_RecordAndEvalObj (Tcl_Interp *interp,
			    Tcl_Obj *cmdPtr, int flags);
extern  Tcl_RegExp	Tcl_RegExpCompile (Tcl_Interp *interp,
			    char *sstring);
extern  int		Tcl_RegExpExec (Tcl_Interp *interp,
			    Tcl_RegExp regexp, char *sstring, char *start);
extern  int		Tcl_RegExpMatch (Tcl_Interp *interp,
			    char *sstring, char *pattern);
extern  void		Tcl_RegExpRange (Tcl_RegExp regexp,
			    int index, char **startPtr, char **endPtr);
extern  void		Tcl_RegisterChannel (Tcl_Interp *interp,
	        	    Tcl_Channel chan);
extern  void		Tcl_RegisterObjType (
			    Tcl_ObjType *typePtr);
extern  void		Tcl_Release (ClientData clientData);
extern  void		Tcl_ResetResult (Tcl_Interp *interp);
extern  int		Tcl_ScanCountedElement (const char *sstring,
			    int length, int *flagPtr);
extern  int		Tcl_ScanElement (const char *sstring,
			    int *flagPtr);
extern  int		Tcl_Seek (Tcl_Channel chan,
        		    int offset, int mode);
extern  int		Tcl_ServiceAll (void);
extern  int		Tcl_ServiceEvent (int flags);
extern  void		Tcl_SetAssocData (Tcl_Interp *interp,
                            char *name, Tcl_InterpDeleteProc *proc,
                            ClientData clientData);
extern  void		Tcl_SetBooleanObj (Tcl_Obj *objPtr, 
			    int boolValue);
extern  void		Tcl_SetChannelBufferSize (
			    Tcl_Channel chan, int sz);
extern  int		Tcl_SetChannelOption (
			    Tcl_Interp *interp, Tcl_Channel chan,
	        	    char *optionName, char *newValue);
extern  int		Tcl_SetCommandInfo (Tcl_Interp *interp,
			    char *cmdName, Tcl_CmdInfo *infoPtr);
extern  void		Tcl_SetDoubleObj (Tcl_Obj *objPtr, 
			    double doubleValue);
extern  void		Tcl_SetErrno (int err);
extern  void		Tcl_SetErrorCode ();
extern  void		Tcl_SetIntObj (Tcl_Obj *objPtr, 
			    int intValue);
extern  void		Tcl_SetListObj (Tcl_Obj *objPtr, 
			    int objc, Tcl_Obj * * objv);
//			    int objc, Tcl_Obj *const objv[]);
extern  void		Tcl_SetLongObj (Tcl_Obj *objPtr, 
			    long longValue);
extern  void		Tcl_SetMaxBlockTime (Tcl_Time *timePtr);
extern  void		Tcl_SetObjErrorCode (Tcl_Interp *interp,
			    Tcl_Obj *errorObjPtr);
extern  void		Tcl_SetObjLength (Tcl_Obj *objPtr,
			    int length);
extern  void		Tcl_SetObjResult (Tcl_Interp *interp,
			    Tcl_Obj *resultObjPtr);
//extern  void		Tcl_SetPanicProc (void (*proc)());
extern  int		Tcl_SetRecursionLimit (Tcl_Interp *interp,
			    int depth);
extern  void		Tcl_SetResult (Tcl_Interp *interp,
			    char *sstring, Tcl_FreeProc *freeProc);
extern  int		Tcl_SetServiceMode (int mode);
extern  void		Tcl_SetStdChannel (Tcl_Channel channel,
			    int type);
extern  void		Tcl_SetStringObj (Tcl_Obj *objPtr, 
			    char *bytes, int length);
extern  void		Tcl_SetTimer (Tcl_Time *timePtr);
extern  char *		Tcl_SetVar (Tcl_Interp *interp,
			    char *varName, char *newValue, int flags);
extern  char *		Tcl_SetVar2 (Tcl_Interp *interp,
			    char *part1, char *part2, char *newValue,
			    int flags);
extern  char *		Tcl_SignalId (int sig);
extern  char *		Tcl_SignalMsg (int sig);
extern  void		Tcl_Sleep (int ms);
extern  void		Tcl_SourceRCFile (Tcl_Interp *interp);
extern  int		Tcl_SplitList (Tcl_Interp *interp,
			    char *list, int *argcPtr, char ***argvPtr);
extern  void		Tcl_SplitPath (char *path,
			    int *argcPtr, char ***argvPtr);
extern  void		Tcl_StaticPackage (Tcl_Interp *interp,
			    char *pkgName, Tcl_PackageInitProc *initProc,
			    Tcl_PackageInitProc *safeInitProc);
extern  int		Tcl_StringMatch (char *sstring,
			    char *pattern);
extern  int		Tcl_Tell (Tcl_Channel chan);
extern  int		Tcl_TraceVar (Tcl_Interp *interp,
			    char *varName, int flags, Tcl_VarTraceProc *proc,
			    ClientData clientData);
extern  int		Tcl_TraceVar2 (Tcl_Interp *interp,
			    char *part1, char *part2, int flags,
			    Tcl_VarTraceProc *proc, ClientData clientData);
extern  char *		Tcl_TranslateFileName (Tcl_Interp *interp,
			    char *name, Tcl_DString *bufferPtr);
extern  int		Tcl_Ungets (Tcl_Channel chan, char *str,
			    int len, int atHead);
extern  void		Tcl_UnlinkVar (Tcl_Interp *interp,
			    char *varName);
extern  int		Tcl_UnregisterChannel (Tcl_Interp *interp,
			    Tcl_Channel chan);
extern  int		Tcl_UnsetVar (Tcl_Interp *interp,
			    char *varName, int flags);
extern  int		Tcl_UnsetVar2 (Tcl_Interp *interp,
			    char *part1, char *part2, int flags);
extern  void		Tcl_UntraceVar (Tcl_Interp *interp,
			    char *varName, int flags, Tcl_VarTraceProc *proc,
			    ClientData clientData);
extern  void		Tcl_UntraceVar2 (Tcl_Interp *interp,
			    char *part1, char *part2, int flags,
			    Tcl_VarTraceProc *proc, ClientData clientData);
extern  void		Tcl_UpdateLinkedVar (Tcl_Interp *interp,
			    char *varName);
extern  int		Tcl_UpVar (Tcl_Interp *interp,
			    char *frameName, char *varName,
			    char *localName, int flags);
extern  int		Tcl_UpVar2 (Tcl_Interp *interp,
			    char *frameName, char *part1, char *part2,
			    char *localName, int flags);
extern  int		Tcl_VarEval ();
extern  ClientData	Tcl_VarTraceInfo (Tcl_Interp *interp,
			    char *varName, int flags,
			    Tcl_VarTraceProc *procPtr,
			    ClientData prevClientData);
extern  ClientData	Tcl_VarTraceInfo2 (Tcl_Interp *interp,
			    char *part1, char *part2, int flags,
			    Tcl_VarTraceProc *procPtr,
			    ClientData prevClientData);
extern  int		Tcl_WaitForEvent (Tcl_Time *timePtr);
extern  Tcl_Pid		Tcl_WaitPid (Tcl_Pid pid, int *statPtr, 
			    int options);
extern  int		Tcl_Write (Tcl_Channel chan,
			    char *s, int slen);
extern  void		Tcl_WrongNumArgs (Tcl_Interp *interp,
			    int objc, Tcl_Obj * * objv, char *message);
//			    int objc, Tcl_Obj *const objv[], char *message);




/*
 * Convenience declaration of Tcl_AppInit for backwards compatibility.
 * This function is not *implemented* by the tcl library, so the storage
 * class is neither DLLEXPORT nor DLLIMPORT
 */

extern  int             Tcl_AppInit (Tcl_Interp *interp);

 /* RESOURCE_INCLUDED */



 /* _TCL */



