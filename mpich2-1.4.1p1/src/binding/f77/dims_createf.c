/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*  
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 * This file is automatically generated by buildiface 
 * DO NOT EDIT
 */
#include "mpi_fortimpl.h"


/* Begin MPI profiling block */
#if defined(USE_WEAK_SYMBOLS) && !defined(USE_ONLY_MPI_NAMES) 
#if defined(HAVE_MULTIPLE_PRAGMA_WEAK)
extern FORT_DLL_SPEC void FORT_CALL MPI_DIMS_CREATE( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_dims_create__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_dims_create( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_dims_create_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#if defined(F77_NAME_UPPER)
#pragma weak MPI_DIMS_CREATE = PMPI_DIMS_CREATE
#pragma weak mpi_dims_create__ = PMPI_DIMS_CREATE
#pragma weak mpi_dims_create_ = PMPI_DIMS_CREATE
#pragma weak mpi_dims_create = PMPI_DIMS_CREATE
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak MPI_DIMS_CREATE = pmpi_dims_create__
#pragma weak mpi_dims_create__ = pmpi_dims_create__
#pragma weak mpi_dims_create_ = pmpi_dims_create__
#pragma weak mpi_dims_create = pmpi_dims_create__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak MPI_DIMS_CREATE = pmpi_dims_create_
#pragma weak mpi_dims_create__ = pmpi_dims_create_
#pragma weak mpi_dims_create_ = pmpi_dims_create_
#pragma weak mpi_dims_create = pmpi_dims_create_
#else
#pragma weak MPI_DIMS_CREATE = pmpi_dims_create
#pragma weak mpi_dims_create__ = pmpi_dims_create
#pragma weak mpi_dims_create_ = pmpi_dims_create
#pragma weak mpi_dims_create = pmpi_dims_create
#endif



#elif defined(HAVE_PRAGMA_WEAK)

#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL MPI_DIMS_CREATE( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak MPI_DIMS_CREATE = PMPI_DIMS_CREATE
#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL mpi_dims_create__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_dims_create__ = pmpi_dims_create__
#elif !defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL mpi_dims_create( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_dims_create = pmpi_dims_create
#else
extern FORT_DLL_SPEC void FORT_CALL mpi_dims_create_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_dims_create_ = pmpi_dims_create_
#endif

#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#if defined(F77_NAME_UPPER)
#pragma _HP_SECONDARY_DEF PMPI_DIMS_CREATE  MPI_DIMS_CREATE
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma _HP_SECONDARY_DEF pmpi_dims_create__  mpi_dims_create__
#elif !defined(F77_NAME_LOWER_USCORE)
#pragma _HP_SECONDARY_DEF pmpi_dims_create  mpi_dims_create
#else
#pragma _HP_SECONDARY_DEF pmpi_dims_create_  mpi_dims_create_
#endif

#elif defined(HAVE_PRAGMA_CRI_DUP)
#if defined(F77_NAME_UPPER)
#pragma _CRI duplicate MPI_DIMS_CREATE as PMPI_DIMS_CREATE
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma _CRI duplicate mpi_dims_create__ as pmpi_dims_create__
#elif !defined(F77_NAME_LOWER_USCORE)
#pragma _CRI duplicate mpi_dims_create as pmpi_dims_create
#else
#pragma _CRI duplicate mpi_dims_create_ as pmpi_dims_create_
#endif
#endif /* HAVE_PRAGMA_WEAK */
#endif /* USE_WEAK_SYMBOLS */
/* End MPI profiling block */


/* These definitions are used only for generating the Fortran wrappers */
#if defined(USE_WEAK_SYMBOLS) && defined(HAVE_MULTIPLE_PRAGMA_WEAK) && \
    defined(USE_ONLY_MPI_NAMES)
extern FORT_DLL_SPEC void FORT_CALL MPI_DIMS_CREATE( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_dims_create__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_dims_create( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_dims_create_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#if defined(F77_NAME_UPPER)
#pragma weak mpi_dims_create__ = MPI_DIMS_CREATE
#pragma weak mpi_dims_create_ = MPI_DIMS_CREATE
#pragma weak mpi_dims_create = MPI_DIMS_CREATE
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak MPI_DIMS_CREATE = mpi_dims_create__
#pragma weak mpi_dims_create_ = mpi_dims_create__
#pragma weak mpi_dims_create = mpi_dims_create__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak MPI_DIMS_CREATE = mpi_dims_create_
#pragma weak mpi_dims_create__ = mpi_dims_create_
#pragma weak mpi_dims_create = mpi_dims_create_
#else
#pragma weak MPI_DIMS_CREATE = mpi_dims_create
#pragma weak mpi_dims_create__ = mpi_dims_create
#pragma weak mpi_dims_create_ = mpi_dims_create
#endif

#endif

/* Map the name to the correct form */
#ifndef MPICH_MPI_FROM_PMPI
#if defined(USE_WEAK_SYMBOLS) && defined(HAVE_MULTIPLE_PRAGMA_WEAK)
/* Define the weak versions of the PMPI routine*/
#ifndef F77_NAME_UPPER
extern FORT_DLL_SPEC void FORT_CALL PMPI_DIMS_CREATE( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER_2USCORE
extern FORT_DLL_SPEC void FORT_CALL pmpi_dims_create__( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER_USCORE
extern FORT_DLL_SPEC void FORT_CALL pmpi_dims_create_( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER
extern FORT_DLL_SPEC void FORT_CALL pmpi_dims_create( MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#endif

#if defined(F77_NAME_UPPER)
#pragma weak pmpi_dims_create__ = PMPI_DIMS_CREATE
#pragma weak pmpi_dims_create_ = PMPI_DIMS_CREATE
#pragma weak pmpi_dims_create = PMPI_DIMS_CREATE
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak PMPI_DIMS_CREATE = pmpi_dims_create__
#pragma weak pmpi_dims_create_ = pmpi_dims_create__
#pragma weak pmpi_dims_create = pmpi_dims_create__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak PMPI_DIMS_CREATE = pmpi_dims_create_
#pragma weak pmpi_dims_create__ = pmpi_dims_create_
#pragma weak pmpi_dims_create = pmpi_dims_create_
#else
#pragma weak PMPI_DIMS_CREATE = pmpi_dims_create
#pragma weak pmpi_dims_create__ = pmpi_dims_create
#pragma weak pmpi_dims_create_ = pmpi_dims_create
#endif /* Test on name mapping */
#endif /* Use multiple pragma weak */

#ifdef F77_NAME_UPPER
#define mpi_dims_create_ PMPI_DIMS_CREATE
#elif defined(F77_NAME_LOWER_2USCORE)
#define mpi_dims_create_ pmpi_dims_create__
#elif !defined(F77_NAME_LOWER_USCORE)
#define mpi_dims_create_ pmpi_dims_create
#else
#define mpi_dims_create_ pmpi_dims_create_
#endif /* Test on name mapping */

/* This defines the routine that we call, which must be the PMPI version
   since we're renaming the Fortran entry as the pmpi version.  The MPI name
   must be undefined first to prevent any conflicts with previous renamings,
   such as those put in place by the globus device when it is building on
   top of a vendor MPI. */
#undef MPI_Dims_create
#define MPI_Dims_create PMPI_Dims_create 

#else

#ifdef F77_NAME_UPPER
#define mpi_dims_create_ MPI_DIMS_CREATE
#elif defined(F77_NAME_LOWER_2USCORE)
#define mpi_dims_create_ mpi_dims_create__
#elif !defined(F77_NAME_LOWER_USCORE)
#define mpi_dims_create_ mpi_dims_create
/* Else leave name alone */
#endif


#endif /* MPICH_MPI_FROM_PMPI */

/* Prototypes for the Fortran interfaces */
#include "fproto.h"
FORT_DLL_SPEC void FORT_CALL mpi_dims_create_ ( MPI_Fint *v1, MPI_Fint *v2, MPI_Fint *v3, MPI_Fint *ierr ){
    *ierr = MPI_Dims_create( *v1, *v2, v3 );
}