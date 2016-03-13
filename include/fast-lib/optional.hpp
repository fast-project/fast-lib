/*
 * This file is part of fast-lib.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#ifndef FAST_LIB_OPTIONAL_HPP
#define FAST_LIB_OPTIONAL_HPP

#include <memory>

namespace fast
{

// boost.optional?
template<typename T>
class Optional
{
public:
	Optional(); // noexcept?
	Optional(T rhs);
	Optional(std::unique_ptr<T> rhs);
	
	Optional<T> & operator=(const T &rhs); // C++11 (move-)assignment operator?

	T & operator(); // noexcept?
	const T & operator() const;

	bool is_valid() const noexcept;

private:
	bool valid;
	std::unique_ptr<T> ptr;
};

// Template scope?
Optional::Optional() :
	valid(false)
{
}

Optional::Optional(T rhs) :
	valid(true),
	ptr(new T(rhs))
{
}

Optional::Optional(std::unique_ptr<T> rhs) :
	valid(true),
	rhs(std::move(rhs))
{
}

T & operator=(const T &rhs)
{
	valid = true;
	ptr = new T(rhs);
	return *this;
}

T & operator()
{
	return *ptr;
}

const T & operator() const
{
	return *ptr;
}

bool is_valid() const
{
	return valid;
}

}

#endif
