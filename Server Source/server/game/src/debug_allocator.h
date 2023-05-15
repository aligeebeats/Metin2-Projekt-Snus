#pragma once
#include <cstdlib>
#include <new>

#define M2_NEW new
#define M2_DELETE(p) delete (p)
#define M2_DELETE_ARRAY(p) delete[] (p)
#define M2_PTR_REF(p) (p)
#define M2_PTR_DEREF(p) (*(p))

template<typename T>
T* get_pointer(T* p) {
	return p;
}
