#include <iostream>
#include "media.h"
#include "mediasource.h"
using namespace std;


/*001 在自定义数据类型中嵌入其他自定义数据类型:比较数据成员的初始化顺序
**    (1) 析构函数不能带参数
**    (2) 数据成员按照类定义中的顺序进行初始化
**    (3) 嵌入对象的构造函数在执行对象的构造函数体前被调用
**    (4) 不同嵌入对象的构建顺序由它们的顺序决定
**    (5) 嵌入对象按构建它们的相反顺序销毁
*/
#if 0
class Matter
{
public:
	Matter(int idx):m_id(idx)
	{
		cout << m_id << ":" << "This is a matter!\n";
	}

	~Matter()
	{
		cout << m_id << ":" << "matter destructor!\n";
	}

private:
	const int m_id;
};


class World
{
public:
	World(int idx):m_Matter(idx),m_identifier(idx)
	{
		cout << m_identifier << ":" << "Hello!" << endl;
	}

	~World()
	{
		cout << m_identifier << ":" << "Good Bye!" << endl;
	}

	Matter m_Matter;

private:
	const int m_identifier;
};

World TheWorld(1);
#endif


/*002 类的继承: 比较基类和派生类的构造函数及析构函数的调用顺序
**    (1)基类的构造函数先于派生类被调用
**    (2)派生类的析构函数先于基类被调用
*/
#if 0
class CelestialBody
{
public:
	CelestialBody(float mass):m_mass(mass)
	{
		cout << "CelestialBody Constructor from " << m_mass << endl;
	}

	~CelestialBody()
	{
		cout << "CelestialBody Destructor from " << m_mass << endl;
	}

private:
	const float m_mass;
};


class Star:public CelestialBody
{
public:
	Star(float mass,float brightness):CelestialBody(mass),m_brightness(brightness)
	{
		cout << "Star Constructor from " << m_brightness << endl;
	}

	~Star()
	{
		cout << "Star Destructor from " << m_brightness << endl;
	}

private:
	const float m_brightness;
};
#endif


/*003 成员函数和接口,成员函数的作用域
**    (1)使用成员函数访问数据成员没有运行时开销,所以没有太多理由在程序中使用公有数据成员
*/
#if 0
class InputNum
{
public:
	InputNum(char aString[])
	{
		cout << aString << endl;
		cin >> m_num;
	}

	int GetValue()
	{
		return m_num;
	}

	void AddNum(char bString[])
	{
		InputNum aInputNum(bString);
		m_num += aInputNum.GetValue();
	}

	~InputNum()
	{
		cout << "destructor from " << m_num << endl;
	}

private:
	int m_num;
};
#endif


/*004 
**
*/

int main()
{
	MediaSource* pMediaSource = new MediaSource(1);
	cout << "Here is Main function!\n";
	pMediaSource->Print();
	delete pMediaSource;
    return 0;
}



