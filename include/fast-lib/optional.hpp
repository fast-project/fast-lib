/*
 * This file is part of fast-lib.
 * Copyright (C) 2015 RWTH Aachen University - ACS
 *
 * This file is licensed under the GNU Lesser General Public License Version 3
 * Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
 */

#ifndef FAST_LIB_OPTIONAL_HPP
#define FAST_LIB_OPTIONAL_HPP

#include <fast-lib/serializable.hpp>
#include <yaml-cpp/yaml.h>

#include <memory>

namespace fast
{

// boost.optional?
template<typename T>
class Optional :
	public fast::Serializable
{
public:
	using value_type = T;

	/**
	 * \brief Construct an empty Optional.
	 *
	 * \param tag The tag used for serialization.
	 */
	Optional(std::string tag) noexcept;
	/**
	 * \brief Construct an Optional with an object passed using ptr.
	 *
	 * \param tag The tag used for serialization.
	 * \param ptr A pointer to the object to be used.
	 */
	Optional(std::string tag, std::unique_ptr<T> ptr) noexcept;
	/**
	 * \brief Construct an Optional with an object copied from val.
	 *
	 * \param tag The tag used for serialization.
	 * \param val The value used to initilize the contained object.
	 */
	Optional(std::string tag, const T &val);
	/**
	 * \brief Copy construct from l-value.
	 *
	 * \param rhs The Optional to copy from.
	 */
	Optional(const Optional<T> &rhs);
	/**
	 * \brief Move construct from r-value (e.g. temporary objects).
	 *
	 * \param rhs The Optional to copy from.
	 */
	Optional(Optional<T> &&rhs) noexcept;
	/**
	 * \brief Copy-assign a value to this Optional from an l-value.
	 *
	 * \param val The value to be assigned.
	 */
	Optional<T> & operator=(const T &val);
	/**
	 * \brief Move-assign a value to this Optional from an r-value.
	 *
	 * \param val The value to be assigned.
	 */
	Optional<T> & operator=(T &&val);
	/**
	 * \brief Copy-assign a value to this Optional from another Optional.
	 *
	 * \param rhs Another optional to copy assign from.
	 */
	Optional<T> & operator=(Optional<T> rhs);

	/**
	 * \brief Check two Optionals for equality in validity and value.
	 *
	 * \param rhs The other Optional to compare with.
	 */
	bool operator==(const Optional<T> &rhs) const;
	/**
	 * \brief Check this Optional and a value for equality.
	 *
	 * \param val The value to compare with.
	 */
	bool operator==(const T &val) const;

	/**
	 * \brief Check if this Optional is valid i.e. contains a value.
	 */
	bool is_valid() const noexcept;
	/**
	 * \brief bool operator to make checks for validity implicit.
	 */
	operator bool() const noexcept;

	/**
	 * \brief Get reference to value of the contained object and throw exception if not valid.
	 */
	T & get();
	/**
	 * \brief Get const reference to value of the contained object and throw exception if not valid.
	 */
	const T & get() const;

	/**
	 * \brief Use get_or to simplify checks for validity with subsequent getting of value.
	 *
	 * \param default_value This value is returned if this Optional is not valid.
	 * Usage:
	 * `int num = opt_num.get_or(42);`
	 * num will be either the value of opt_num if it is valid or the default value 42 if not.
	 */
	constexpr const T & get_or(const T &default_value) const;

	/**
	 * \brief Shortcut for get().
	 */
	T & operator*();
	/**
	 * \brief Shortcut for get().
	 */
	const T & operator*() const;

	/**
	 * \brief Shortcut for get() on pointer.
	 */
	T * operator->();
	/**
	 * \brief Shortcut for get() on pointer.
	 */
	const T * operator->() const;

	/**
	 * \brief Return the tag of this optional.
	 */
	std::string get_tag() const;

	/**
	 * \brief Set the value of this Optional.
	 *
	 * \param val The value to initilize the object from (copy).
	 */
	void set(const T &val);
	/**
	 * \brief Set the value of this Optional.
	 *
	 * \param val The value to initilize the object from (move).
	 */
	void set(T &&val);

	/**
	 * \brief Used to serialize to YAML.
	 */
	YAML::Node emit() const override;
	/**
	 * \brief Used to deserialize from YAML.
	 */
	void load(const YAML::Node &node) override;
private:
	/**
	 * \brief The tag of this Optional used for serialization.
	 */
	std::string tag;
	/**
	 * \brief The pointer to the contained object.
	 */
	std::unique_ptr<T> ptr;
	/**
	 * \brief Denote if the Optional actually contains a valid object.
	 */
	bool valid;
};

template<typename T>
Optional<T>::Optional(std::string tag) noexcept :
	tag(std::move(tag)),
	valid(false)
{
}

template<typename T>
Optional<T>::Optional(std::string tag, std::unique_ptr<T> ptr) noexcept :
	tag(std::move(tag)),
	ptr(std::move(ptr)),
	valid(true)
{
}

template<typename T>
Optional<T>::Optional(std::string tag, const T &val) :
	tag(std::move(tag)),
	ptr(new T(val)),
	valid(true)
{
}

template<typename T>
Optional<T>::Optional(const Optional<T> &rhs) :
	Serializable(rhs),
	tag(rhs.tag),
	ptr(rhs.is_valid() ? new T(*rhs.ptr) : nullptr),
	valid(rhs.valid)
{
}

template<typename T>
Optional<T>::Optional(Optional<T> &&rhs) noexcept :
	Serializable(rhs),
	tag(std::move(rhs.tag)),
	ptr(std::move(rhs.ptr)),
	valid(rhs.valid)
{
	rhs.valid = false;
}

template<typename T>
Optional<T> & Optional<T>::operator=(const T &val)
{
	ptr.reset(new T(val));
	valid = true;
	return *this;
}

template<typename T>
Optional<T> & Optional<T>::operator=(T &&val)
{
	ptr.reset(new T(std::move(val)));
	valid = true;
	return *this;
}

template<typename T>
Optional<T> & Optional<T>::operator=(Optional<T> rhs)
{
	tag = std::move(rhs.tag);
	valid = rhs.valid;
	ptr = std::move(rhs.ptr);
	return *this;
}

template<typename T>
bool Optional<T>::operator==(const Optional<T> &rhs) const
{
	if (is_valid() != rhs.is_valid())
		return false;
	if (is_valid())
		return get() == rhs.get();
	return true;
}

template<typename T>
bool Optional<T>::operator==(const T &rhs) const
{
	return is_valid() && (get() == rhs);
}

template<typename T>
bool Optional<T>::is_valid() const noexcept
{
	return valid;
}

template<typename T>
Optional<T>::operator bool() const noexcept
{
	return valid;
}

template<typename T>
T & Optional<T>::get()
{
	if (!valid)
		throw std::runtime_error("Optional value not valid.");
	return *ptr;
}

template<typename T>
const T & Optional<T>::get() const
{
	if (!valid)
		throw std::runtime_error("Optional value not valid.");
	return *ptr;
}

template<typename T>
constexpr const T & Optional<T>::get_or(const T &default_value) const
{
	return valid ? get() : default_value;
}

template<typename T>
T & Optional<T>::operator*()
{
	return get();
}

template<typename T>
const T & Optional<T>::operator*() const
{
	return get();
}

template<typename T>
T * Optional<T>::operator->()
{
	return &get();
}

template<typename T>
const T * Optional<T>::operator->() const
{
	return &get();
}

template<typename T>
std::string Optional<T>::get_tag() const
{
	return tag;
}

template<typename T>
void Optional<T>::set(const T &rhs)
{
	ptr.reset(new T(rhs));
	valid = true;
}

template<typename T>
void Optional<T>::set(T &&rhs)
{
	ptr.reset(new T(std::move(rhs)));
	valid = true;
}

template<typename T>
YAML::Node Optional<T>::emit() const
{
	YAML::Node node;
	if (valid)
		node[tag] = *ptr;
	return node;
}

template<typename T>
void Optional<T>::load(const YAML::Node &node)
{
	if (tag != "" && node[tag]) {
		ptr.reset(new T(node[tag].template as<T>()));
		valid = true;
	}
}

}
#endif
