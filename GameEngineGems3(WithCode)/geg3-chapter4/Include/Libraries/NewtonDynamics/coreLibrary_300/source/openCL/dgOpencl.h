/* Copyright (c) <2003-2011> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _DG_OPENCL_H_
#define _DG_OPENCL_H_


#include "dgPhysicsStdafx.h"

class dgMemoryAllocator;

class dgOpencl
{
	public:
	static dgOpencl* GetOpenCL(dgMemoryAllocator* const allocator); 

	dgInt32 GetPlatformsCount() const;
	void SelectPlaform(dgInt32 deviceIndex);
	void GetVendorString(dgInt32 deviceIndex, char* const name, dgInt32 maxlength) const;

	void CleanUp();

	private:
	dgOpencl(dgMemoryAllocator* const allocator);
	~dgOpencl(void);
	
	void CompileProgram ();
	

	dgMemoryAllocator* m_allocator;
	void* m_currentPlatform;
	void* m_context;
//	void* m_cmd_queue;
	void* m_program;
	
	void* m_devices[16];
	void* m_platforms[16];
	char m_platformName[128];
	dgInt32 m_devicesCount;
	dgInt32 m_platformsCount;
	dgInt32 m_aligment;
//	dgList<void*> m_kernels;
//	dgTree<void*, dgUnsigned32> m_programs;
};

#endif