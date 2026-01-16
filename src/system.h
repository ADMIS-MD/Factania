//-----------------------------------------------------------------------------
//
//	File:   System.h
//  Author: Nicholas Brennan
//  Date:   01/15/2026
//
//-----------------------------------------------------------------------------

namespace core {

	class system
	{
		virtual ~system() {}

		virtual void update() = 0;
		virtual void draw() = 0;
		
	};

}