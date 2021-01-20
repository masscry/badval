#include <badval.hpp>
#include <iostream>

template<bvl::value_t::type_t typeID>
int checkValueT(const bvl::value_t& value)
{
  try 
  {
    auto asVal = value.As<typeID>();
    std::cout << "value = " << asVal << std::endl;
    if (typeID != value.Type())
    {
      std::cerr << __FILE__ <<":"<< __LINE__ << ": error:  This must not happen! (" << value.Type() << ")" << std::endl;
      return -1;
    }
  }
  catch (std::runtime_error&)
  {
    if (typeID == value.Type())
    {
      std::cerr << __FILE__ <<":"<< __LINE__ << ": error:  This must not happen! (" << value.Type() << ")" << std::endl;
      return -1;
    }
  }
  return 0;
}

int checkValue(const bvl::value_t& value)
{
  if (checkValueT<bvl::value_t::number>(value) != 0)
  {
    return -1;
  }
  if (checkValueT<bvl::value_t::string>(value) != 0)
  {
    return -1;
  }
  if (checkValueT<bvl::value_t::pointer>(value) != 0)
  {
    return -1;
  }
  return 0;
}


int main(int argc, char* argv[])
{
  using namespace bvl;

  // test numbers
  value_t number(10);

  // test strings
  std::string test3("test3");
  value_t string1("test");
  value_t string2(std::string("test2"));
  value_t stirng3(test3);

  // test pointers
  value_t ptr1(
    malloc(128),
    [](void* ptr)
    {
      free(ptr);
    }
  );

  value_t ptr2(nullptr, nullptr);

  if (checkValue(number) != 0)
  {
    return EXIT_FAILURE;
  }
  if (checkValue(string1) != 0)
  {
    return EXIT_FAILURE;
  }
  if (checkValue(ptr1) != 0)
  {
    return EXIT_FAILURE;
  }

  string1 = string2;
  std::cout << string1.As<value_t::string>() << std::endl;

  number = string1;
  std::cout << number.As<value_t::string>() << std::endl;

  number = value_t();
  std::cout << number.As<value_t::number>() <<std::endl;

  number = value_t(nullptr, nullptr);
  std::cout << number.As<value_t::pointer>() <<std::endl;

  auto val = value_t(123.0);

  number = val;

  std::cout << number.As<value_t::number>() <<std::endl;

  auto val3 = value_t(std::move(number));
  std::cout << val3.As<value_t::number>() <<std::endl;

  auto val2 = value_t(std::move(ptr1));
  std::cout << val2.As<value_t::pointer>() <<std::endl;

  auto val4 = value_t(std::move(string2));
  std::cout << val4.As<value_t::string>() <<std::endl;


  try
  {
    auto val5 = value_t(val2);
    std::cout << val5.As<value_t::string>() <<std::endl;
  }
  catch(const std::runtime_error& error)
  {
    std::cout << "This must happen" <<std::endl;
  }



  return EXIT_SUCCESS;
}