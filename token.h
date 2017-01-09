/**
 * xx(symbol,value,prec,op,optree,kind,string)
 * ������CHAR,INT,UNSIGNED,SHORT,LONG��ENUM�������������ͣ�
 * FLOAT,DOUBLE�����˸������͡���Щ���Ͷ����Կ����������͡�
 * ��ENUM�����⣬��Щ���Ͷ�û�в�������
 * ENUM���͵Ĳ�������������ݵ��������ͣ���ö�ٱ�ʶ�������͡�
 * ����lcc���ԣ�ö�ٱ�ʶ����������������(int).
 * ������ARRAY,STRUCT,UNION��ʾ�ۺ����͡�
 * STRUCT��UNIONû�в����������ǵ������ڽṹ�����ϵĸ��ӷ��ű�����С�
 * ���ǵ������ڽṹ�����ϱ�ǵĸ��ӷ��ű�����С�
 * ARRAY�Ĳ�����������Ԫ������(element type).
 * POINTER��FUNCTION�ֱ���ָ������(pointer type)�ͷ�������(return type).
 * ��������CONST��VOLATILE˵���޶�����(qualified type),���ǵĲ������������͵�δ�޶���ʽ��
 * CONST����VOLATILEҲ��һ�����Ͳ���������˵��һ������const����volatile�����͡�
 * VOID��������ʾvoid���ͣ�û�в�������
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