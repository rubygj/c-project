#pragma once

//头文件要求
#include <cmath>

//实现抛物线运动

struct vector2 {
	//c++构造函数
	vector2(int _x = 0, int _y = 0) :x(_x), y(_y) {}
	vector2(int* data) :x(data[0]), y(data[1]) {}
	long long x, y;
};
/*
*这是一个C++中的类构造函数，用于创建一个名为vector2的二维向量对象。它有两个重载的构造函数。
*第一个构造函数使用两个整型参数 _x 和 _y 来初始化向量的 x 和 y 分量。如果没有提供参数，则默认将 x 和 y 分量都设置为 0。
*第二个构造函数接受一个指向整型数组的指针 data，并使用数组中的前两个元素来初始化向量的 x 和 y 分量。
*这样设计的构造函数允许用户以不同的方式初始化 vector2 对象，提供了更大的灵活性和方便性。
*/

//加法
vector2 operator +(vector2 x, vector2 y);
/*
*这是一个重载的加法运算符函数，用于将两个 vector2 对象相加并返回结果。
*在这个函数中，参数 x 和 y 是两个 vector2 对象，表示要相加的两个向量。
*函数的返回类型应该是 vector2，表示返回的结果也是一个 vector2 对象。
*/

//减法
vector2 operator -(vector2 x, vector2 y);

// 乘法
vector2 operator *(vector2 x, vector2 y);
vector2 operator *(vector2, float);
vector2 operator *(float, vector2);

//叉积
long long cross(vector2 x, vector2 y);

//数量积 点积
long long dot(vector2 x, vector2 y);

//四舍五入除法
long long dv(long long a, long long b);


//模长平方
long long len(vector2 x);

//模长
long long dis(vector2 x);

//向量除法
vector2 operator /(vector2 x, vector2 y);

//向量膜
vector2 operator %(vector2 x, vector2 y);

//向量GCD 
vector2 gcd(vector2 x, vector2 y);

//贝塞尔曲线
vector2 calcBezierPoint(float t, vector2 p0, vector2 p1, vector2 p2, vector2 p3);