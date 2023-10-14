#include <iostream>

//函数模板
template<typename T>
T max(T a, T b) {

    std::cout << typeid(a).name() << "," << typeid(b).name() << std::endl;

    return a > b ? a : b;
}


//模板函数的原理：
//编译阶段，对每一个使用该模板的类型产生一个独立的实体。

template<typename T>
T foo(T*) {
}

//模板分为两步编译
//1.模板定义阶段，模板的检查 并不包含类型参数的检查。只包含下面几个方面:
// 语法检查。比如少了个分号
// 使用了未定义的不依赖于模板参数的名称
// 未使用模板参数的stats assertions

//2.模板实例化阶段，为确保所有代码都是有效的，模板会再次被检查，尤其是那些依赖于类型的部分

template<typename T>
void foo(T t) {
    //undeclare();         //第一阶段报错，与模板参数无关
    //undeclare(t);        //第二阶段报错 因为与模板参数有关
}

//编译和链接
//两阶段的编译检查给模板的处理带来了一个问题：当实例化一个模板的时候，编译器需要看到模板的完整定义。这不同于函数编译和链接分离的思想，函数在编译阶段只需要声明就够了。


//类型推断中的类型转换
//类型推断的时候自动的类型转换是受限制的
//如果调用参数是按引用传递的，任何类型转换都不被允许。通过模板类型参数T定义的两个参数，他们实参的类型必须完全一样。
//如果是按值传递的，那么只有退化这一简单类型转化是被允许的 const和volatile限制符会被忽略，引用被转换成引用的类型。


//默认调用参数的类型推断
template<typename T>
void f(T = "") {

}

//为了应对这个情况  你需要给模板类型参数也声明一个默认参数
template<typename T = std::string>
void f1(T = "") {

}

//多个模板参数
template<typename T1,typename T2>
T1 max_ver2(T1 a, T2 b) {
    return b < a ? a : b;
}

//这样会带来问题 如果顺序不同结果会不要一直  如42 66.66 结果 确是 66
//C++提供多种方式应对这一问题
//1.引入第三个模板参数作为返回类型
//2.让编译器照出返回类型
//3.将返回类型定义为两个参数类型的公共类型

//1
//当模板参数和调用参数之间没有必然的联系，且模板参数不能确定的时候，就需要显示的指明模板参数。比如你可以引入第三个模板来指定函数模板的返回类型
template<typename T1,typename T2,typename RT>
RT max_ver3(T1 a, T2 b) {
    return a > b ? a : b;
}

//但是模板类型推断是不会考虑返回类型的，而RT又美亚由被用作调用参数的类型，因此RT不会被推断。这样的话就必须显示指定模板参数的类型。

//返回类型推断 C++14
//template<typename T1,typename T2>
//auto max_ver4(T1 a, T2 b) {
//    return a > b ? a : b;
//}

//C++11
template<typename T1,typename T2>
auto max_ver5(T1 a, T2 b) -> decltype(a > b ? a : b) {
    return a > b ? a : b;
}

//将返回类型声明为公共类型
//C++11开始，标准库提供了一种指定 更一般类型的方式  std::common_type<>::type产生的类型是他们两个模板参数的公共类型
template<typename T1, typename T2>
std::common_type<T1,T2> max_ver6(T1 a, T2 b) {
    return a > b ? a : b;
}

//默认模板参数
//std::decay<T> 退化引用和const等
//The compiler can't (according the C++ language rules) make the following reasoning: "true is always true, so I always return a, so I return the type of a". The compiler must always return (isn't important if the value of the condition is known at compile time or not) a common type between a and b.
//https://stackoverflow.com/questions/58093341/why-do-these-two-code-snippets-have-the-same-effect
template<typename T1,typename T2,typename RT=std::decay<decltype(true?(std::declval<T1>()):(std::declval<T2>()))>>
RT max_rt(T1 a, T2 b) {
 //   return a > b ? a : b;
 //   return ;
}

//std::declval是C++标准库中的一个模板函数，用于在编译时期构造一个指定类型的右值引用，
//通常与decltype一起使用来推导表达式中某类型的成员的类型。但需要注意的是，std::declval只能用于decltype上下文，不能真正用于运行时创建对象。
//注意这里使用std::decay确保返回的值不是引用类型

template<typename T1,typename T2,typename RT=std::common_type<T1,T2>>
RT max_cRt(T1 a, T2 b) {
    return a  > b ? a : b;
}

//函数模板重载

int max1(int a, int b) {
    return a > b ? a : b;
}

template<typename T>
T max1(T a, T b) {
    return a>b?a:b;
}

//如你所见  一个非模板函数可以和一个与其同名的函数模板共存，并且这个同名的函数模板可以被实例化为非模板函数具有相同类型的调用参数。
//在所有其他因素都相同的情况下，模板解析过程将有限选择非模板函数，而不是从模板实例化出来的函数
//max(1,2);
//如果模板可以实例化出一个更匹配的函数，那么就会选择这个模板
//max(1.2,3);
//也可以显示指定一个空的模板列表，这表明他会被解析成一个模板调用，其所有的模板参数会被通过调用参数推断出来
//max<>(1,2)
//由于在模板参数推断时不允许自动类型转换，而常规函数时允许的，因此下面这个也会调用非模板函数
//max('a',3);

//std::common_type 也是会做类型退化的，因此返回类型不会是引用

int main() {

    //使用默认返回类型
    auto x = max_rt(1,2.21);
    //or 显示指定返回类型
    auto x1 = max_rt<int,double,int>(1,2.2);

    auto xx = true ? 1 : 2.2;
    std::cout << typeid(xx).name() << std::endl;

    max_ver3<int,double,double>(4,22.22);   //OK 显示指定返回参数类型  但是太麻烦

    auto r = max_ver2(2.22,2);  //ok  返回值类型根据第一个参数决定


    f(1);   //ok T被推断为int   f<int>(1)
    //f();    //error: no matching function for call to 'f()'   无法推断T
    f1();       //ok  T使用默认参数类型 std::string

    void *p = nullptr;
    foo(p); //ok
    //foo(void*);

    const int a = 41;
    int i = 42;
    std::cout << max(a,i) << std::endl;         //T被推断为int,  a的const被退化掉
    int& j = i;
    std::cout << max(j, a) << std::endl;    //T被推断为int， j的引用被退化掉

    //无法确定T是double还是int
    //std::cout << max(1.2, 4) << std::endl;      //error: no matching function for call to 'max(double, int)'
    std::string s = "111";
    //max("hello", s);                              //error: no matching function for call to 'max(const char [6], std::string&)'

    //2种办法解决上述错误
    //1.
    max(static_cast<double>(4),4.72);           //ok  T被推断为double
    //2
    max<double>(4,4.11);                        //ok  T已经被指定为double

    std::cout << max<int>(1,2) << std::endl;
    return 0;
}
