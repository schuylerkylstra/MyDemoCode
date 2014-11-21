/* 
*
*	seenPair.h
*   Created by Schuyler Kylstra
* 	11/12/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef META_seenPair_H
#define META_seenPair_H

struct seenPair
{
	int index;
	float T;

	seenPair()
	{
		index = 4;
		T = FLT_MAX;
	}

	seenPair(int i, float f)
	{
		index = i;
		T = f;
	}

	~seenPair()
	{

	}
};

#endif