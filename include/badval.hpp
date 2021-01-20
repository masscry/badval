/**
 * @file badval.hpp
 * @author masscry
 * 
 * Small union value library and binding generator.
 * 
 */

#pragma once
#ifndef BAD_VALUE_HEADER
#define BAD_VALUE_HEADER

#include <cstdint>
#include <cassert>
#include <stdexcept>
#include <string>
#include <new>
#include <utility>

namespace bvl
{

  /**
   * Union value with type data.
   */
  class value_t final
  {
  public:

    /**
     * Data type for free functions used with pointer
     */
    using freeFuncPtr_t = void (*)(void* ptr);

    /**
     * Available data types for union.
     */
    enum type_t
    {
      number = 0, /**< Floating point number with 64-bit precision */
      string,     /**< Wrapped std::string */
      pointer     /**< Pointer to some data */
    };

    /**
     * Return stored number data.
     * 
     * @throws std::runtime_error when not a number
     */
    double AsNumber() const
    {
      if (this->type == number)
      {
        return this->value.num;
      }
      throw std::runtime_error("Value is not a number");
    }

    /**
     * Return stored string data.
     * 
     * @throws std::runtime_error when not a string
     */
    const std::string& AsString() const
    {
      if (this->type == string)
      {
        return *this->value.str;
      }
      throw std::runtime_error("Value is not a string");
    }

    /**
     * Return stored pointer.
     *
     * @throws std::runtime_error when not a pointer
     */
    const void* AsPointer() const
    {
      if (this->type == pointer)
      {
        return this->value.ptr;
      }
      throw std::runtime_error("Value is not a pointer");
    }

    /**
     * Get value type.
     * 
     * @see bvl::value_t::type_t
     */
    type_t Type() const noexcept
    {
      return this->type;
    }

    /**
     * Default constructor.
     * 
     * Initializes value as number == 0.0
     */
    value_t()
      : type(number)
    {
      this->value.num = 0.0;
    }

    /**
     * Number constructor.
     * 
     * @param [in] num number to store in value
     */
    explicit value_t(double num)
      : type(number)
    {
      this->value.num = num;
    }

    /**
     * String constructor.
     * 
     * @param [in] str string to store in value
     */
    template<typename... str_t, typename = std::enable_if_t<std::is_constructible<std::string, str_t...>::value>>
    explicit value_t(str_t&&... arg)
      : type(string)
    {
      this->value.str = new(std::nothrow) std::string(std::forward<str_t>(arg)...);
      if (this->value.str == nullptr)
      {
        throw std::bad_alloc();
      }
    }

    /**
     * Pointer constructor.
     * 
     * @param [in] ptr pointer to store int value
     * @param [in] free function to call, can be nullptr, if noting to call on destructor
     */
    explicit value_t(void* ptr, freeFuncPtr_t free)
      : type(pointer)
    {
      this->value.ptr = ptr;
      this->value.free = free;
    }

    /**
     * Copy constructor.
     * 
     * Only numbers and strings can be copied.
     * 
     * Copy of pointer causes exception.
     * 
     * @param [in] src value to copy
     */
    value_t(const value_t& src) 
      : type(src.type)
    {
      switch (this->type)
      {
        case number:
          this->value.num = src.value.num;
          break;
        case string:
          {
            this->value.str = new(std::nothrow) std::string(*src.value.str);
            if (this->value.str == nullptr)
            {
              throw std::bad_alloc();
            }
          }
          break;
        case pointer:
          throw std::runtime_error("Do not know how to copy pointer");
        default:
          throw std::logic_error("Impossible type");
      }
    }

    /**
     * Copy assignment.
     * 
     * Only numbers and string can be copied.
     * 
     * Left-hand variable stays in valid state, if exception
     * happen during assignment.
     * 
     * @param [in] rhs value to copy
     */
    value_t& operator=(const value_t& rhs)
    {
      if (this != &rhs)
      { 
        // this is done through copy constructor+move
        // because copying can throw exception
        // we do not want to be left with 
        // invalid value if that happens.
        *this = value_t(rhs);
      }
      return *this;
    }

    /**
     * Move constructor.
     * 
     * @param [in] src value to move data from
     */
    value_t(value_t&& src) noexcept
      : type(src.type)
    {
      switch (this->type)
      {
        case number:
          this->value.num = src.value.num;
          break;
        case string:
          this->value.str = src.value.str;
          src.value.str = nullptr;
          break;
        case pointer:
          this->value.ptr = src.value.ptr;
          this->value.free = src.value.free;
          src.value.ptr = nullptr;
          src.value.free = nullptr;
          break;
        default:
          assert(0);
      }
    }

    /**
     * Move assignment.
     * 
     * @param [in] rhs value to move data from
     */
    value_t& operator= (value_t&& rhs) noexcept
    {
      if (this != &rhs)
      {
        this->cleanup();

        this->type = rhs.type;
        switch (this->type)
        {
        case number:
          this->value.num = rhs.value.num;
          break;
        case string:
          this->value.str = rhs.value.str;
          rhs.value.str = nullptr;
          break;
        case pointer:
          this->value.ptr = rhs.value.ptr;
          this->value.free = rhs.value.free;
          rhs.value.ptr = nullptr;
          rhs.value.free = nullptr;
          break;
        default:
          assert(0);
        }
      }
      return *this;
    }

    /**
     * Destructor.
     */
    ~value_t()
    {
      this->cleanup();
    }

    /**
     * Class defines data type bind to given type ID.
     */
    template<value_t::type_t type>
    class typeID;

    /**
     * Helper typedef to type less.
     */
    template<value_t::type_t type>
    using typeID_t = typename typeID<type>::data_t;

    /**
     * Get stored value using type id.
     */
    template<value_t::type_t type>
    typeID_t<type> As() const;

  private:

    /**
     * Cleanup value, reset it to number 0.0
     */
    void cleanup() noexcept
    {
      switch (this->type)
      {
        case number:
          // nothing to cleanup for numbers.
          break;
        case string:
          delete this->value.str;
          break;
        case pointer:
          if (this->value.free != nullptr)
          {
            this->value.free(this->value.ptr);
          }
          break;
        default:
          assert(0);
      }
      this->type = number;
      this->value.num = 0.0;
    }

    type_t type; /**< Value type */
    union
    {
      double num; /**< Stored number */
      std::string* str; /**< Stored string */
      struct 
      {
        void* ptr; /**< Stored pointer */
        freeFuncPtr_t free; /**< Stored pointer cleanup function */
      };
    } value;
  };

  template<>
  class value_t::typeID<value_t::number>
  {
  public:
    using data_t = double;
  };

  template<>
  class value_t::typeID<value_t::string>
  {
  public:
    using data_t = const std::string&;
  };

  template<>
  class value_t::typeID<value_t::pointer>
  {
  public:
    using data_t = const void*;
  };

  template<>
  inline value_t::typeID_t<value_t::number> value_t::As<value_t::number>() const
  {
    return this->AsNumber();
  }

  template<>
  inline value_t::typeID_t<value_t::string> value_t::As<value_t::string>() const
  {
    return this->AsString();
  }

  template<>
  inline value_t::typeID_t<value_t::pointer> value_t::As<value_t::pointer>() const
  {
    return this->AsPointer();
  }

} // namespace bvl

#endif /* BAD_VALUE_HEADER */