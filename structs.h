/*
********************************************************************************
* name	 : 	srtucts.h
* author : 	Michel Bitar, 2017
* brief  :	Contains definitions for data structures 
********************************************************************************
*/

#ifndef STRUCTS_H
#define	STRUCTS_H

/* Structs -------------------------------------------------------------------*/
/* Brief  : Struct for all moving objects
 * Author : Michel Bitar */
struct actor
{
    float x;
    float y;
    int w;
    int h;
    float dx;
    float dy;
}	actor;

#endif /* STRUCTS_H */