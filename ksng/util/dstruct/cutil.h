#pragma once
#include "sda.h"
#include "sll.h"

namespace cutil {

	template <typename T>
	sda::SecureDataArray<T> linkedListToDataArray(sll::SecureLinkedList<T> &l) {
		sda::SDA<T> a(l.getLength());
		for (int i = 0; i < l.getLength(); i++) { a.set(i, l.get(i)); }
		return a;
	}

};