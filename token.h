/**
 * xx(symbol,value,prec,op,optree,kind,string)
 * 操作符CHAR,INT,UNSIGNED,SHORT,LONG和ENUM定义了整数类型；
 * FLOAT,DOUBLE定义了浮点类型。这些类型都可以看做算术类型。
 * 除ENUM类型外，这些类型都没有操作数。
 * ENUM类型的操作数是与其兼容的整数类型，即枚举标识符的类型。
 * 对于lcc而言，枚举标识符的类型总是整型(int).
 * 操作符ARRAY,STRUCT,UNION表示聚合类型。
 * STRUCT和UNION没有操作数，它们的域存放在结构或联合的附加符号表入口中。
 * 它们的域存放在结构或联合标记的附加符号表入口中。
 * ARRAY的操作数是数据元素类型(element type).
 * POINTER和FUNCTION分别定义指针类型(pointer type)和返回类型(return type).
 * 操作类型CONST和VOLATILE说明限定类型(qualified type),它们的操作数就是类型的未限定形式。
 * CONST加上VOLATILE也是一个类型操作符，它说明一个既是const又是volatile的类型。
 * VOID操作符表示void类型，没有操作数。
 */
#ifndef TOKEN_H
#define TOKEN_H
enum {
#define xx(a,b,c,d,e,f,g) a=b,
#define yy(a,b,c,d,e,f,g)
	yy(0, 0, 0, 0, 0, 0, 0)
	xx(FLOAT, 1, 0, 0, 0, CHAR, "float")
	xx(DOUBLE, 2, 0, 0, 0, CHAR, "double")
	xx(CHAR, 3, 0, 0, 0, , CHAR, "char")
	xx(SHORT, 4, 0, 0, 0, CHAR, "short")
	xx(INT, 5, 0, 0, 0, CHAR, "int")
	xx(UNSIGNED, 6, 0, 0, 0, CHAR, "unsigned")
	xx(POINTER, 7, 0, 0, 0, 0, "pointer")
	xx(VOID, 8, 0, 0, 0, CHAR, "void")
	xx(STRUCT, 9, 0, 0, 0, CHAR, "struct")
	xx(UNION, 10, 0, 0, 0, 0, "union")
	xx(FUNCTION, 11, 0, 0, 0, 0, "function")
	xx(ARRAY, 12, 0, 0, 0, 0, "array")
	xx(ENUM, 13, 0, 0, 0, CHAR, "enum")
	xx(LONG, 14, 0, 0, 0, CHAR, "long")
	xx(CONST, 15, 0, 0, 0, CHAR, "const")
	xx(VOLATILE, 16, 0, 0, 0, CHAR, "volatile")

	xx(STATIC, 81, 0, 0, 0, STATIC, "static")
	xx(SWITCH, 82, 0, 0, 0, IF, "switch")
	xx(TYPEDEF, 83, 0, 0, 0, STATIC, "typedef")
#undef xx
#undef yy
	LAST
};
#endif