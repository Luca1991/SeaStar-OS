#ifndef __NULL_H
#define	__NULL_H

#ifdef NULL
#undef NULL
#endif

#ifdef __cplusplus
extern "C"
{
#endif
/* standard NULL declaration */
#define	NULL	0
#ifdef __cplusplus
}
#else
/* standard NULL declaration */
#define NULL	(void*)0
#endif


#endif
