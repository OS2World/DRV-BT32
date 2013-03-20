# $Id: envchk.mak,v 1.10 2004/02/22 23:03:22 smilcke Exp $

#===================================================================
# Check environment
#===================================================================
ifndef LX_BASE
error No LX_BASE paths defined.
endif

ifndef DDK
error DDK is not defined.
endif

ifndef WATCOM
error WATCOM is not defined
endif

ifndef DRV_BASE
error No DRV_BASE paths defined.
endif

ifndef GCC
error No GCC paths defined.
endif

ifndef EMXUTILS
error No EMXUTILS paths defined.
endif
