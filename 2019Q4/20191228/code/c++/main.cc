#include <iostream>
#include <fstream>
#include "person.pb.h"

int main(int argc, char const *argv[])
{
  Person *person = new Person();
  person->set_id(1);
  person->set_name("zhangsan");
  person->set_age(18);
  person->add_email("1.qq.com");
  person->add_email("2.qq.com");
  PhoneNumber *phone1 = person->add_phone();
  phone1->set_number("123456");
  phone1->set_type(PhoneType::HOME);
  PhoneNumber *phone2 = person->add_phone();
  phone2->set_number("234567");
  phone2->set_type(PhoneType::MOBILE);
  Address *address = person->mutable_address();
  address->set_country("China");
  address->set_detail("Jiangsu");

  std::cout << "write to main.proto" << std::endl;
  std::ofstream ofs;
  ofs.open("main.proto", std::ofstream::binary);
  person->SerializeToOstream(&ofs);
  ofs.close();

  std::cout << "read from main.proto" << std::endl;
  Person *person_tmp = new Person();
  std::ifstream ifs;
  ifs.open("main.proto", std::ofstream::binary);
  person_tmp->ParseFromIstream(&ifs);
  std::cout << "id : " << person_tmp->id() << std::endl;
  std::cout << "name : " << person_tmp->name() << std::endl;
  std::cout << "age : " << person_tmp->age() << std::endl;
  std::cout << "email : " << person_tmp->email(0) << " " << person_tmp->email(1) << std::endl;
  std::cout << "phone : " << person_tmp->phone(0).number() << " " << person_tmp->phone(1).number() << std::endl;
  std::cout << "address : " << person_tmp->address().country() << " " << person_tmp->address().detail() << std::endl;
  return 0;
}
