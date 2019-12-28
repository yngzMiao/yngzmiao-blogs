# -*- coding:UTF-8 -*-
import os,sys
import proto_pb2.person_pb2 as person_proto

if __name__ == "__main__":
  person = person_proto.Person()
  person.id = 1
  person.name = "zhangsan"
  person.age = 18

  person.email.append("1.qq.com")
  person.email.append("2.qq.com")
  phone1 = person.phone.add()
  phone2 = person.phone.add()
  phone1.number = "123456"
  phone1.type = person_proto.PhoneType.HOME
  phone2.number = "234567"
  phone2.type = person_proto.PhoneType.MOBILE
  addr = person.address
  addr.country = "China"
  addr.detail = "Jiangsu"

  print("write to main.proto")
  fw = open("main.proto", "w")
  fw.write(person.SerializeToString())
  fw.close()

  print("read from main.proto")
  fr = open("main.proto", "r")
  data = fr.read()
  person_tmp = person_proto.Person()
  person_tmp.ParseFromString(data)
  print("id : " + str(person_tmp.id))
  print("name : " + str(person_tmp.name))
  print("age : " + str(person_tmp.age))
  print("email : " + str(person_tmp.email))
  print("phone : " + str(person_tmp.phone))
  print("address : " + str(person_tmp.address))
  fr.close()