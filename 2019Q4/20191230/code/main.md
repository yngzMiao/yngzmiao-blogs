08      // (1<<3)|0，1为id的field_bumber，0为id对应的wire type
01      // 0x01，id为1

12      // (2<<3)|1，2为name的field_bumber，1为name对应的wire type
08      // name字段的字符串长度
7A68616E6773616E      // "zhangsan"的ASCII码

18      // (3<<3)|0，3为age的field_bumber，0为age对应的wire type
12      // 0x12，age为18

22      // (4<<3)|2，4为email的field_bumber，2为email对应的wire type
08      // email字段的字符串长度
312E71712E636F6D      // "1.qq.com"的ASCII码
22      // (4<<3)|2，4为email的field_bumber，2为email对应的wire type
08      // email字段的字符串长度
322E71712E636F6D      // "2.qq.com"的ASCII码

2A      // (5<<3)|2，5为phone的field_bumber，2为phone对应的wire type
0A      // 0x10，phone的长度为10，1+1+6+1+1
0A      // (1<<3)|2，1为number的field_bumber，2为number对应的wire type
06      // number字段的字符串长度
313233343536      // "123456"的ASCII码
10      // (2<<3)|1，2为type的field_bumber，1为type对应的wire type
01      // enum为1，表示HOME

2A      // (5<<3)|2，5为phone的field_bumber，2为phone对应的wire type
0A      // 0x10，phone的长度为10，1+1+6+1+1
0A      // (1<<3)|2，1为number的field_bumber，2为number对应的wire type
06      // number字段的字符串长度
323334353637      // "234567"的ASCII码
10      // (2<<3)|1，2为type的field_bumber，1为type对应的wire type
00      // enum为0，表示MOBILE

32      // (6<<3)|2，6为address的field_bumber，2为address对应的wire type
10      // 0x10，address的长度为16，1+1+5+1+1+7
0A      // (1<<3)|2，1为country的field_bumber，2为country对应的wire type
05      // country字段的字符串长度
4368696E61      // "China"的ASCII码
12      // (2<<3)|2，2为detail的field_bumber，2为detail对应的wire type
07      // detail字段的字符串长度
4A69616E677375      // "Jiangsu"的ASCII码